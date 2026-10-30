//! Assembles the decompiled `.rul` source: typedefs, globals, then functions.

use std::fmt::Write;

use crate::Result;
use crate::ir;
use crate::lower::{self, Ctx};
use crate::script::{DataDeclList, Format, Function, ObjectVarInfo, Script};
use crate::types::{ConcreteType, VarType};
use crate::vars::{Decl, Symbols, TableId, Variable};

pub struct Decompiled {
    pub source: String,
    pub dot: String,
}

fn variant_decl(obj: &ObjectVarInfo, script: &Script) -> Result<Decl> {
    if obj.is_array() {
        return Ok(Decl::VariantArray { count: obj.extra });
    }
    match obj.typedef_id() {
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

    let mut rename = |address: i32, ty: VarType, name: &str| -> Result<()> {
        let v = syms.by_address(table, address, ty, true)?;
        syms.vars[v].name = name.to_string();
        Ok(())
    };
    if script.format == Format::New {
        rename(0, VarType::Variant, "LAST_RESULT")?;
        rename(1, VarType::Variant, "NOTHING")?;
        rename(2, VarType::Variant, "Err")?;
    }
    for e in &script.externs {
        rename(e.address as i32, e.ty, &e.name)?;
    }
    Ok(())
}

struct FunctionOut {
    header: String,
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
    for i in 0..(decls.num_strings as usize).saturating_sub(strs) {
        let size = decls.string_size(i);
        syms.add(
            locals,
            Variable::new(-100 - (i + 1 + strs) as i32, Decl::String { size }),
        );
    }
    for i in 0..decls.objects.len().saturating_sub(objs) {
        let decl = variant_decl(&decls.objects[i], script)?;
        syms.add(locals, Variable::new(-100 - (i + 1 + objs) as i32, decl));
    }
    Ok(())
}

fn function(
    script: &Script,
    syms: &mut Symbols,
    globals: TableId,
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
    // .ins locals are numbered from the start of the frame, overlapping the
    // parameters.
    if script.format == Format::Old {
        counts = [0; 3];
    }
    add_locals(script, syms, locals, &f.decls, counts)?;

    let nodes = lower::lower_function(
        &Ctx {
            script,
            syms,
            table: locals,
        },
        &f.bbs,
    )?;
    let cfg = ir::build(&nodes);
    Ok(FunctionOut {
        header: String::new(),
        nodes,
        cfg,
        args,
        locals,
    })
}

fn function_name(f: &Function) -> String {
    if f.proto.is_exported() {
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
    for s in &script.structs {
        s.write(&mut source, &script.structs);
        source.push('\n');
    }
    source.push('\n');
    for &v in syms.table_vars(globals) {
        let _ = writeln!(source, "{}", syms.vars[v]);
    }
    source.push('\n');

    let mut dot = String::new();
    let mut unnamed = 0;
    for f in script.fns.iter().filter(|f| !f.bbs.is_empty()) {
        let name = function_name(f);
        let display = if name.is_empty() {
            format!("Func_{}", f.proto.bb_id)
        } else {
            name.clone()
        };
        match function(script, &mut syms, globals, f) {
            Ok(mut out) => {
                let arg_list: Vec<String> = syms
                    .table_vars(out.args)
                    .iter()
                    .map(|&v| syms.vars[v].to_string())
                    .collect();
                let _ = writeln!(
                    out.header,
                    "function {} {display}({})",
                    f.proto.return_type,
                    arg_list.join(",")
                );
                source.push_str(&out.header);
                for &v in syms.table_vars(out.locals) {
                    let _ = writeln!(source, "{};", syms.vars[v]);
                }
                source.push_str("begin\n");
                out.cfg.write_body(&mut source, &out.nodes, &syms);
                source.push_str("end;\n\n");

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
