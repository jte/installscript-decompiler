//! Assembles the decompiled `.rul` source: typedefs, globals, prototypes,
//! then functions.

use std::fmt::Write;

use crate::Result;
use crate::debug;
use crate::ir;
use crate::lower::{self, Ctx};
use crate::script::{DataDeclList, Format, Function, ObjectVarInfo, Op, Script, proto_flags};
use crate::simplify;
use crate::structure;
use crate::types::{ConcreteType, ScriptType, VarType};
use crate::vars::{Decl, Symbols, TableId, VarId, Variable};

pub struct Decompiled {
    pub source: String,
    pub dot: String,
}

/// The low bits of the flags are a COM `VARTYPE`: the element type of an
/// array, and `VT_DISPATCH` (9) for an `OBJECT` or a struct.
fn variant_decl(obj: &ObjectVarInfo, script: &Script) -> Result<Decl> {
    let vt = obj.flags & 0xff;
    if obj.is_array() {
        // Every numeric type (NUMBER, BOOL, INT, HWND, LIST…) is stored as 3.
        let element = match vt {
            3 => "NUMBER",
            8 => "STRING",
            _ => "VARIANT",
        };
        return Ok(Decl::Array {
            element,
            count: obj.extra,
        });
    }
    match obj.typedef_id() {
        -1 if vt == 9 && script.format == Format::New => Ok(Decl::Object),
        -1 => Ok(Decl::Variant),
        id => {
            let s = usize::try_from(id)
                .ok()
                .and_then(|i| script.structs.get(i))
                .ok_or_else(|| format!("Invalid typedef id {id}"))?;
            Ok(Decl::Struct {
                type_name: s.name.clone(),
            })
        }
    }
}

fn add_globals(script: &Script, syms: &mut Symbols, table: TableId) -> Result<()> {
    let g = &script.globals;
    for i in 0..g.num_numbers {
        syms.add(table, Variable::new(i as i32, Decl::Number));
    }
    for i in 0..g.num_strings as usize {
        syms.add(
            table,
            Variable::new(
                i as i32,
                Decl::String {
                    size: g.string_size(i),
                },
            ),
        );
    }
    for (i, obj) in g.objects.iter().enumerate() {
        syms.add(table, Variable::new(i as i32, variant_decl(obj, script)?));
    }

    let mut rename = |address: i32, ty: VarType, name: &str, external: bool| -> Result<()> {
        let v = syms.by_address(table, address, ty, true)?;
        syms.vars[v].name = name.to_string();
        syms.vars[v].external = external;
        Ok(())
    };
    if script.format == Format::New {
        rename(0, VarType::Variant, "LAST_RESULT", false)?;
        rename(1, VarType::Variant, "NOTHING", false)?;
        rename(2, VarType::Variant, "Err", false)?;
    }
    for e in &script.externs {
        // `.ins` externs are InstallShield 5 system variables, which have no
        // `external` declaration.
        let builtin = e.ty == VarType::Variant && BUILTIN_GLOBALS.contains(&e.name.as_str());
        rename(
            e.address as i32,
            e.ty,
            &e.name,
            script.format == Format::New && !builtin,
        )?;
    }
    if let Some(info) = &script.debug {
        name_from_symbols(syms, &[table], &info.globals, |v| v.address);
    }
    Ok(())
}

/// Renames the variables of `tables` after debug symbols. Names are listed per
/// type in slot order; `slot` gives a variable's position in that list. The
/// compiler's temporaries come after the declared variables and have no
/// names. A type with more names than variables is left alone, since the
/// symbols can't belong to this code.
fn name_from_symbols(
    syms: &mut Symbols,
    tables: &[TableId],
    names: &debug::Names,
    slot: impl Fn(&Variable) -> i32,
) {
    for ty in [VarType::Number, VarType::String, VarType::Variant] {
        let names = names.of(ty);
        let vars: Vec<_> = tables
            .iter()
            .flat_map(|&t| syms.table_vars(t).iter().copied())
            .filter(|&v| syms.vars[v].ty == ty)
            .collect();
        if names.len() > vars.len() {
            continue;
        }
        for v in vars {
            let name = usize::try_from(slot(&syms.vars[v]))
                .ok()
                .and_then(|i| names.get(i));
            // Compiler temporaries can be listed with an empty name.
            if let Some(name) = name.filter(|n| !n.is_empty()) {
                syms.vars[v].name = name.clone();
                syms.vars[v].named = true;
            }
        }
    }
}

struct FunctionOut {
    nodes: Vec<crate::expr::Node>,
    cfg: ir::Cfg,
    args: TableId,
    locals: TableId,
}

fn add_locals(
    script: &Script,
    syms: &mut Symbols,
    locals: TableId,
    decls: &DataDeclList,
    counts: [usize; 3],
) -> Result<()> {
    let [nums, strs, objs] = counts;
    for i in 0..(decls.num_numbers as usize).saturating_sub(nums) {
        syms.add(
            locals,
            Variable::new(-100 - (i + 1 + nums) as i32, Decl::Number),
        );
    }
    // The string and object tables list the parameters too, ahead of the
    // locals. (The original decompiler indexed them from 0, giving each local
    // the declaration of the slot before it.)
    for i in 0..(decls.num_strings as usize).saturating_sub(strs) {
        let size = decls.string_size(i + strs);
        syms.add(
            locals,
            Variable::new(-100 - (i + 1 + strs) as i32, Decl::String { size }),
        );
    }
    for i in 0..decls.objects.len().saturating_sub(objs) {
        let decl = variant_decl(&decls.objects[i + objs], script)?;
        syms.add(locals, Variable::new(-100 - (i + 1 + objs) as i32, decl));
    }
    Ok(())
}

fn function(
    script: &Script,
    syms: &mut Symbols,
    globals: TableId,
    last_result: Option<VarId>,
    f: &Function,
) -> Result<FunctionOut> {
    let args = syms.new_table(Some(globals));
    let locals = syms.new_table(Some(args));

    let mut counts = [0usize; 3];
    for a in &f.proto.args {
        let (slot, decl) = match a.internal_type {
            ConcreteType::Number | ConcreteType::NumberPointer => (0, Decl::Number),
            ConcreteType::String | ConcreteType::StringPointer => (1, Decl::String { size: None }),
            ConcreteType::Object | ConcreteType::ObjectPointer => (2, Decl::Variant),
            ConcreteType::Invalid(t) => return Err(format!("Invalid argument type info {t}")),
        };
        counts[slot] += 1;
        syms.add(args, Variable::new(-100 - counts[slot] as i32, decl));
    }
    add_locals(script, syms, locals, &f.decls, counts)?;
    if let Some(df) = debug_function(script, f) {
        // Parameters and locals share one numbering, from -101 down.
        name_from_symbols(syms, &[args, locals], &df.vars, |v| -v.address - 101);
    }

    let mut nodes = lower::lower_function(
        &Ctx {
            script,
            syms,
            table: locals,
        },
        &f.bbs,
    )?;
    // Only unnamed locals: a name from debug symbols means the source had
    // the variable, and a sized string truncates what is stored in it.
    let syms_ref = &*syms;
    let foldable = |v: VarId| {
        let var = &syms_ref.vars[v];
        !var.is_global()
            && !var.named
            && !matches!(var.decl, Decl::String { size: Some(_) })
            && !syms_ref.table_vars(args).contains(&v)
    };
    simplify::fold_temporaries(&mut nodes, foldable, last_result);
    let cfg = ir::build(&nodes);
    Ok(FunctionOut {
        nodes,
        cfg,
        args,
        locals,
    })
}

/// The variable that receives call results.
fn last_result_var(script: &Script, syms: &Symbols, globals: TableId) -> Option<VarId> {
    match script.format {
        Format::New => syms
            .by_name(globals, "LAST_RESULT", VarType::Variant, true)
            .ok(),
        Format::Old => syms.by_address(globals, 0, VarType::Number, true).ok(),
    }
}

/// A prototype's type. `VARIANT` and `OBJECT` compile to the same code;
/// `VARIANT` is the one that also accepts numbers and strings.
fn proto_type(t: ScriptType) -> String {
    match t.0 {
        10 => "VARIANT".into(),
        _ => t.to_string(),
    }
}

/// Functions some call passes more arguments than they declare.
fn called_with_extra_args(script: &Script) -> Vec<bool> {
    let mut extra = vec![false; script.fns.len()];
    for f in &script.fns {
        for a in f.bbs.iter().flat_map(|bb| &bb.actions) {
            if let Op::Call { fn_id } = a.op
                && let Some(callee) = script.fns.get(fn_id as usize)
                && a.args.len() > callee.proto.args.len()
            {
                extra[fn_id as usize] = true;
            }
        }
    }
    extra
}

/// `prototype` line for a function the script defines or a DLL function.
/// `export` only exists in InstallShield 6 and later; `.ins` prototypes are
/// marked exported just so their functions keep their names.
fn prototype(f: &Function, name: &str, variadic: bool, format: Format) -> String {
    let mut args: Vec<String> = f
        .proto
        .args
        .iter()
        .map(|a| {
            // A DLL's STRING parameter is stored the same either way.
            let dll_string = f.proto.flags & proto_flags::IS_DLL_FUNCTION != 0
                && a.internal_type == ConcreteType::StringPointer;
            let byref = !dll_string
                && matches!(
                    a.internal_type,
                    ConcreteType::NumberPointer
                        | ConcreteType::StringPointer
                        | ConcreteType::ObjectPointer
                );
            format!(
                "{}{}",
                if byref { "BYREF " } else { "" },
                proto_type(a.script_type)
            )
        })
        .collect();
    if variadic || f.proto.flags & proto_flags::IS_VARIADIC != 0 {
        args.push("...".into());
    }
    format!(
        "{}prototype {}{} {name}({});",
        if f.proto.is_exported() && format == Format::New {
            "export "
        } else {
            ""
        },
        if f.proto.flags & proto_flags::IS_CDECL != 0 {
            "cdecl "
        } else {
            ""
        },
        proto_type(f.proto.return_type),
        args.join(", ")
    )
}

/// Globals every InstallScript 6+ script has without declaring them.
const BUILTIN_GLOBALS: [&str; 3] = ["LAST_RESULT", "NOTHING", "Err"];

fn is_program(f: &Function) -> bool {
    f.proto.is_exported() && f.proto.name == "program"
}

fn debug_function<'a>(script: &'a Script, f: &Function) -> Option<&'a debug::DebugFunction> {
    script.debug.as_ref()?.functions.get(f.debug?)
}

fn function_name(f: &Function) -> String {
    if f.proto.is_exported() || f.debug.is_some() {
        f.proto.name.clone()
    } else {
        String::new()
    }
}

pub fn decompile(script: &Script) -> Result<Decompiled> {
    let mut syms = Symbols::default();
    let globals = syms.new_table(None);
    add_globals(script, &mut syms, globals)?;

    let mut source = String::new();
    match (&script.debug, &script.debug_section) {
        (Some(info), _) => {
            let files: Vec<_> = (0..info.files.len() as u16)
                .filter_map(|i| info.file_name(i))
                .collect();
            let _ = writeln!(
                source,
                "// Names restored from debug symbols. Source files: {}\n",
                files.join(", ")
            );
        }
        (None, debug::Section::External(path)) => {
            let _ = writeln!(
                source,
                "// Compiled with debug symbols in {path}\n\
                 // Supply that .dbg file to restore the original names."
            );
        }
        _ => {}
    }
    if let Some(note) = &script.debug_note {
        let _ = writeln!(source, "// {note}");
    }
    if script.debug.is_none()
        && (script.debug_note.is_some()
            || matches!(script.debug_section, debug::Section::External(_)))
    {
        source.push('\n');
    }
    for s in &script.structs {
        s.write(&mut source, &script.structs);
        source.push('\n');
    }
    let mut any = false;
    for &v in syms.table_vars(globals) {
        let var = &syms.vars[v];
        if script.format == Format::New
            && var.ty == VarType::Variant
            && var.address < 3
            && BUILTIN_GLOBALS.contains(&var.name.as_str())
        {
            continue;
        }
        // Defined in another script object: `.obs` files list these.
        let external = if var.external { "external " } else { "" };
        let _ = writeln!(source, "{external}{var};");
        any = true;
    }
    if any {
        source.push('\n');
    }

    let defined = |f: &&Function| !f.bbs.is_empty();
    let display_name = |f: &Function| {
        let name = function_name(f);
        if name.is_empty() {
            format!("Func_{}", f.proto.bb_id)
        } else {
            name
        }
    };
    {
        let extra = called_with_extra_args(script);
        let mut any = false;
        for (i, f) in script.fns.iter().enumerate() {
            let dll = f.proto.flags & proto_flags::IS_DLL_FUNCTION != 0;
            if (dll || defined(&f)) && !is_program(f) {
                let name = if dll {
                    f.proto.name.clone()
                } else {
                    display_name(f)
                };
                let _ = writeln!(source, "{}", prototype(f, &name, extra[i], script.format));
                any = true;
            }
        }
        if any {
            source.push('\n');
        }
    }

    let last_result = last_result_var(script, &syms, globals);
    let mut dot = String::new();
    let mut unnamed = 0;
    for f in script.fns.iter().filter(defined) {
        let name = function_name(f);
        let display = display_name(f);
        match function(script, &mut syms, globals, last_result, f) {
            Ok(out) => {
                if let Some((file, first, last)) = script
                    .debug
                    .as_ref()
                    .zip(debug_function(script, f))
                    .and_then(|(info, df)| df.location(info))
                {
                    let _ = writeln!(source, "// {file}, lines {first}-{last}");
                }
                let program = is_program(f);
                if program {
                    source.push_str("program\n");
                } else {
                    // Types are in the prototype.
                    let params: Vec<String> = syms
                        .table_vars(out.args)
                        .iter()
                        .map(|&v| syms.vars[v].name.clone())
                        .collect();
                    let return_type = proto_type(f.proto.return_type);
                    let _ = writeln!(
                        source,
                        "function {return_type} {display}({})",
                        params.join(", ")
                    );
                    // Temporaries that folding emptied out need no
                    // declaration.
                    let used = simplify::mentioned_vars(&out.nodes);
                    for &v in syms.table_vars(out.locals) {
                        if syms.vars[v].named || used.binary_search(&v).is_ok() {
                            let _ = writeln!(source, "{};", syms.vars[v]);
                        }
                    }
                    source.push_str("begin\n");
                }
                structure::write_body(&mut source, &out.cfg, &out.nodes, &syms, last_result);
                source.push_str(if program {
                    "endprogram\n\n"
                } else {
                    "end;\n\n"
                });

                let dot_name = if name.is_empty() {
                    unnamed += 1;
                    format!("Func_{unnamed}")
                } else {
                    name
                };
                out.cfg.write_dot(&mut dot, &dot_name);
            }
            Err(e) => {
                let _ = writeln!(source, "// function {display}: could not decompile: {e}\n");
            }
        }
    }
    Ok(Decompiled { source, dot })
}
