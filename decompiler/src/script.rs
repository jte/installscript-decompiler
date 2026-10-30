//! Format-independent model of a parsed script, shared by the `.inx`/`.obs`
//! frontend ([`crate::newis`]) and the `.ins` frontend ([`crate::oldis`]).

use std::fmt::{self, Write};
use std::ops::Range;

use crate::Result;
use crate::debug::{self, DebugInfo};
use crate::expr::BinOp;
use crate::stream::Stream;
use crate::types::{ArgumentTypeInfo, ScriptType, VarType};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Format {
    /// InstallShield 6 and later (`.inx`, `.obs`).
    New,
    /// InstallShield 5 and earlier (`.ins`).
    Old,
}

pub const VB_ARRAY: u16 = 0x2000;

#[derive(Clone, Copy, Debug)]
pub struct ObjectVarInfo {
    pub flags: u16,
    /// Element count for arrays, typedef index (as `i16`, -1 for none) otherwise.
    pub extra: u16,
}

impl ObjectVarInfo {
    pub fn is_array(&self) -> bool {
        self.flags & VB_ARRAY != 0
    }

    pub fn typedef_id(&self) -> i16 {
        self.extra as i16
    }
}

#[derive(Clone, Copy, Debug)]
pub struct StringVarInfo {
    pub var_id: u16,
    pub size: u16,
}

/// Variable declarations of the global scope or of one function.
#[derive(Clone, Debug, Default)]
pub struct DataDeclList {
    pub num_numbers: u16,
    pub num_strings: u16,
    pub objects: Vec<ObjectVarInfo>,
    pub strings: Vec<StringVarInfo>,
}

impl DataDeclList {
    pub fn parse(s: &mut Stream) -> Result<Self> {
        let num_numbers = s.u16()?;
        let objects = parse_object_table(s)?;
        let num_strings = s.u16()?;
        let count = s.u16()?;
        let strings = parse_string_entries(s, count)?;
        Ok(DataDeclList {
            num_numbers,
            num_strings,
            objects,
            strings,
        })
    }

    /// Declared size of string variable `index`, if it has one.
    pub fn string_size(&self, index: usize) -> Option<u16> {
        self.strings
            .iter()
            .rev()
            .find(|s| s.var_id as usize == index)
            .map(|s| s.size)
    }
}

pub fn parse_object_table(s: &mut Stream) -> Result<Vec<ObjectVarInfo>> {
    let count = s.u16()?;
    (0..count)
        .map(|_| {
            Ok(ObjectVarInfo {
                flags: s.u16()?,
                extra: s.u16()?,
            })
        })
        .collect()
}

pub fn parse_string_entries(s: &mut Stream, count: u16) -> Result<Vec<StringVarInfo>> {
    (0..count)
        .map(|_| {
            Ok(StringVarInfo {
                var_id: s.u16()?,
                size: s.u16()?,
            })
        })
        .collect()
}

impl fmt::Display for DataDeclList {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        writeln!(f, "numNumbers: {}", self.num_numbers)?;
        writeln!(f, "numStrings: {}", self.num_strings)?;
        for (i, obj) in self.objects.iter().enumerate() {
            if obj.is_array() {
                writeln!(f, "[{i}]:  flags: {:x} count: {}", obj.flags, obj.extra)?;
            } else {
                writeln!(
                    f,
                    "[{i}]:  flags: {:x} structId: {}",
                    obj.flags,
                    obj.typedef_id()
                )?;
            }
        }
        for (i, s) in self.strings.iter().enumerate() {
            writeln!(f, "[{i}]:  varId: {} count: {}", s.var_id, s.size)?;
        }
        Ok(())
    }
}

pub mod proto_flags {
    pub const IS_DLL_FUNCTION: u8 = 1;
    pub const IS_INTERNAL_FUNCTION: u8 = 2;
    pub const IS_PREDEFINED_FUNCTION: u8 = 4;
    pub const IS_EXPORTED: u8 = 8;
    /// A DLL function with the `cdecl` calling convention. InstallShield 6
    /// let these take extra arguments without being declared variadic.
    pub const IS_CDECL: u8 = 0x10;
    /// Takes extra arguments after the declared ones (`...`).
    pub const IS_VARIADIC: u8 = 0x80;
}

#[derive(Clone, Debug)]
pub struct Prototype {
    pub flags: u8,
    pub args: Vec<ArgumentTypeInfo>,
    pub name: String,
    pub return_type: ScriptType,
    /// Index of the basic block where the function body starts; 0xffff if none.
    pub bb_id: u16,
}

impl Prototype {
    pub fn is_exported(&self) -> bool {
        self.flags & proto_flags::IS_EXPORTED != 0
    }
}

impl fmt::Display for Prototype {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        if self.is_exported() {
            f.write_str("export ")?;
        }
        f.write_str("prototype ")?;
        if self.bb_id != 0xffff {
            write!(f, "{} ", self.return_type)?;
        }
        f.write_str(if self.is_exported() {
            &self.name
        } else {
            "predefined_func"
        })?;
        if !self.args.is_empty() {
            let args: Vec<String> = self
                .args
                .iter()
                .map(|a| a.script_type.to_string())
                .collect();
            write!(f, "({});", args.join(","))?;
        }
        Ok(())
    }
}

#[derive(Clone, Debug)]
pub struct StructMember {
    pub name: String,
    pub rep_count: u16,
    pub ty: ScriptType,
}

#[derive(Clone, Debug)]
pub struct ScriptStruct {
    pub name: String,
    pub members: Vec<StructMember>,
}

impl ScriptStruct {
    pub fn write(&self, out: &mut String, structs: &[ScriptStruct]) {
        let _ = writeln!(out, "typedef {}", self.name);
        out.push_str("\tbegin\n");
        for m in &self.members {
            out.push_str("\t\t");
            if m.ty == ScriptType::STRUCT {
                match structs.get(m.rep_count as usize) {
                    Some(s) => {
                        let _ = write!(out, "{} {}", s.name, m.name);
                    }
                    None => {
                        let _ = write!(out, "STRUCT_UNKNOWN_{} {}", m.rep_count, m.name);
                    }
                }
            } else {
                let _ = write!(out, "{} {}", m.ty, m.name);
                if m.rep_count > 1 {
                    if m.ty == ScriptType::STRING || m.ty == ScriptType::WSTRING {
                        let _ = write!(out, "[{}]", m.rep_count);
                    } else {
                        let _ = write!(out, "({})", m.rep_count);
                    }
                }
            }
            out.push_str(";\n");
        }
        out.push_str("\tend;\n");
    }
}

/// A global variable that the script imports or exports by name.
#[derive(Clone, Debug)]
pub struct Extern {
    pub ty: VarType,
    pub name: String,
    pub address: i16,
}

impl Extern {
    pub fn from_code(code: u8, name: String, address: i16) -> Option<Self> {
        let ty = match code {
            // The original decompiler has 2 and 3 the other way round, but
            // externs such as TARGETDIR (a STRING) are stored with code 2.
            1 => VarType::Variant,
            2 => VarType::String,
            3 => VarType::Number,
            _ => return None,
        };
        Some(Extern { ty, name, address })
    }
}

/// An instruction operand.
#[derive(Clone, Debug)]
pub enum Arg {
    Var {
        ty: VarType,
        address: i16,
        param: bool,
    },
    Str(String),
    Num(i32),
}

/// Escapes a string for an InstallScript string literal.
pub fn quote(s: &str) -> String {
    let mut out = String::with_capacity(s.len() + 2);
    out.push('"');
    for c in s.chars() {
        match c {
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            '\\' => out.push_str("\\\\"),
            '"' => out.push_str("\\\""),
            // The compiler takes octal escapes (not hex); a raw control
            // character such as NUL would end the line or the literal.
            c if (c as u32) < 0x20 || c == '\x7f' => {
                let _ = write!(out, "\\{:03o}", c as u32);
            }
            c => out.push(c),
        }
    }
    out.push('"');
    out
}

impl fmt::Display for Arg {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Arg::Var { ty, address, param } => {
                let ty = match ty {
                    VarType::String => "{CStrArg}",
                    VarType::Number => "{CNumArg}",
                    VarType::Variant => "{CVariantArg}",
                };
                write!(f, "{} {ty} {address}", if *param { "p" } else { "v" })
            }
            Arg::Str(s) => f.write_str(&quote(s)),
            Arg::Num(n) => write!(f, "{n}"),
        }
    }
}

#[derive(Clone, Debug)]
pub enum Op {
    /// `NOP`; lowers to `LAST_RESULT = LAST_RESULT`.
    Nop,
    /// `.ins` return-from-program / empty return; lowers like [`Op::Nop`].
    ReturnNop,
    Abort,
    Exit,
    /// Instructions with no source-level effect (try/catch markers, handlers).
    Marker(&'static str),
    /// `.inx`: args are `[else offset (relative block index), condition]`.
    If,
    /// `.ins`: jumps to block `target` when the condition `args[0]` is false.
    OldIf {
        label: u8,
        target: u16,
    },
    /// `.inx`: `args[0]` is the target as a relative block index.
    Goto,
    /// `.ins`: absolute target block index.
    OldGoto {
        target: u16,
    },
    Assign,
    /// Ties an external system variable (`args[0]`) to an id (`args[1]`);
    /// the result is left in `LAST_RESULT`.
    BindVariable,
    AddressOf,
    Binary(BinOp),
    Unary(char),
    IndirectStruct {
        struct_id: u16,
    },
    SetByte,
    GetByte,
    /// `.inx` call; `fn_id` indexes the prototype table.
    Call {
        fn_id: u16,
    },
    /// `.ins` call; the callee is the function starting at block `event`.
    OldCall {
        proto: u16,
        event: u16,
    },
    Return,
    /// Built-in function: `LAST_RESULT = name(args[..used])`.
    Builtin {
        name: &'static str,
        used: usize,
    },
    PropPut,
    PropPutRef,
    PropGet,
    /// `.ins` property access; no decompiled form.
    OldProp(&'static str),
    LoadStringConst,
    /// `.ins` instruction without a dedicated decoder.
    Placeholder(u16),
}

#[derive(Clone, Debug)]
pub struct Action {
    pub op: Op,
    pub args: Vec<Arg>,
}

#[derive(Clone, Debug)]
pub struct BasicBlock {
    pub id: u32,
    pub actions: Vec<Action>,
}

#[derive(Clone, Debug)]
pub struct Function {
    pub decls: DataDeclList,
    pub proto: Prototype,
    pub bbs: Vec<BasicBlock>,
    /// Indices of the function's actions among all actions of the script,
    /// prolog included; what debug symbols refer to.
    pub actions: Range<u32>,
    /// Index into the script's [`DebugInfo::functions`].
    pub debug: Option<usize>,
}

impl Function {
    pub fn new(proto: Prototype) -> Self {
        Function {
            decls: DataDeclList::default(),
            proto,
            bbs: Vec::new(),
            actions: 0..0,
            debug: None,
        }
    }
}

pub struct Script {
    pub format: Format,
    pub fns: Vec<Function>,
    pub structs: Vec<ScriptStruct>,
    pub externs: Vec<Extern>,
    pub globals: DataDeclList,
    /// What the script itself says about debug symbols.
    pub debug_section: debug::Section,
    /// Symbols in use, embedded or supplied separately.
    pub debug: Option<DebugInfo>,
    /// Why available symbols are not in use, for the output's header.
    pub debug_note: Option<String>,
}

impl Script {
    pub fn fn_by_id(&self, id: usize) -> Result<&Function> {
        self.fns
            .get(id)
            .ok_or_else(|| format!("Function #{id} not found"))
    }

    pub fn fn_index_by_bb_id(fns: &[Function], bb: usize) -> Result<usize> {
        fns.iter()
            .position(|f| f.proto.bb_id as usize == bb)
            .ok_or_else(|| format!("No function starts at block {bb}"))
    }

    pub fn fn_by_bb_id(&self, bb: usize) -> Result<&Function> {
        Ok(&self.fns[Self::fn_index_by_bb_id(&self.fns, bb)?])
    }

    /// Name of the function an instruction calls.
    pub fn callee_name(&self, op: &Op) -> Result<&str> {
        match op {
            Op::Call { fn_id } => Ok(&self.fn_by_id(*fn_id as usize)?.proto.name),
            Op::OldCall { event, .. } => Ok(&self.fn_by_bb_id(*event as usize)?.proto.name),
            _ => Err("not a call".into()),
        }
    }

    /// The disassembly listing (`--show-actions`).
    pub fn listing(&self) -> String {
        let mut out = String::new();
        if self.format == Format::Old {
            for s in &self.structs {
                s.write(&mut out, &self.structs);
                out.push('\n');
            }
            out.push('\n');
        }
        for f in &self.fns {
            if !f.proto.is_exported() && f.bbs.is_empty() {
                continue;
            }
            let _ = writeln!(out, "{}", f.proto);
            let _ = writeln!(out, "{}", f.decls);
            for bb in &f.bbs {
                for a in &bb.actions {
                    let _ = writeln!(out, "{:x}: {}", bb.id, self.action_text(a));
                }
            }
            out.push_str("\n\n\n");
        }
        out
    }

    fn action_text(&self, a: &Action) -> String {
        let arg = |i: usize| {
            a.args
                .get(i)
                .map_or_else(|| "?".to_string(), |a| a.to_string())
        };
        let all = |sep: &str| {
            a.args
                .iter()
                .map(|a| a.to_string())
                .collect::<Vec<_>>()
                .join(sep)
        };
        match &a.op {
            Op::Nop => "NOP".into(),
            Op::ReturnNop => "RETURN".into(),
            Op::Abort => "abort".into(),
            Op::Exit => "EXIT".into(),
            Op::Marker(name) => {
                if a.args.is_empty() {
                    name.to_string()
                } else {
                    format!("{name} {}", all(" "))
                }
            }
            Op::If => format!("If (else:{}) {}", arg(0), arg(1)),
            Op::OldIf { label, target } => {
                format!("IF (label: {label:x}, realAddress: {target:x}) {}", arg(0))
            }
            Op::Goto => format!("goto {};", arg(0)),
            Op::OldGoto { target } => format!("GOTO {target:x}"),
            Op::Assign => format!("{} = {}", arg(0), arg(1)),
            Op::BindVariable => format!("BindVariable({}, {})", arg(0), arg(1)),
            Op::AddressOf => format!("{} = &{}", arg(0), arg(1)),
            Op::Binary(op) => {
                if a.args.len() == 4 {
                    format!("{} = {} {} {}", arg(0), arg(1), arg(2), arg(3))
                } else {
                    format!("{} = {} {} {}", arg(0), arg(1), op.symbol(), arg(2))
                }
            }
            Op::Unary(op) => format!("{} = {op}{}", arg(0), arg(1)),
            Op::IndirectStruct { struct_id } => {
                format!("IndirectStruct (struct: {struct_id}) {}", arg(0))
            }
            Op::SetByte => format!("SET BYTE {} = {} ({})", arg(0), arg(1), arg(2)),
            Op::GetByte => format!("GET BYTE {} = {}( {}) ", arg(0), arg(1), arg(2)),
            Op::Call { .. } | Op::OldCall { .. } => {
                let name = self.callee_name(&a.op).unwrap_or("<unknown function>");
                format!("{name}({})", all(", "))
            }
            Op::Return => format!("RETURN {}", all(" ")),
            Op::Builtin { name, .. } => format!("{name}({})", all(", ")),
            Op::PropPut => format!("Put (arg size: {}) {} ", a.args.len(), all(" ")),
            Op::PropPutRef => format!("PutRef {} {}", arg(0), arg(1)),
            Op::PropGet => format!("Get {}", all(" ")),
            Op::OldProp(name) => {
                let parts: Vec<String> = a
                    .args
                    .iter()
                    .enumerate()
                    .map(|(i, x)| format!("[{i}]: {x}"))
                    .collect();
                format!("{name} {}", parts.join(" "))
            }
            Op::LoadStringConst => format!("{} = {} ({})", arg(2), arg(1), arg(0)),
            Op::Placeholder(id) => {
                let mut s = format!("PlaceholderAction (id: 0x{id:x}): ");
                for (i, x) in a.args.iter().enumerate() {
                    let _ = write!(s, " [{i}]: {x}");
                }
                s
            }
        }
    }
}
