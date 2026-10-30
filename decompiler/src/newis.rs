//! Frontend for InstallShield 6+ scripts: compiled setup scripts (`aLuZ`,
//! `setup.inx`) and script objects (`.obs`).

use crate::Result;
use crate::debug;
use crate::expr::BinOp;
use crate::format::HeaderKind;
use crate::script::{
    Action, Arg, BasicBlock, DataDeclList, Extern, Format, Function, Op, Prototype, Script,
    ScriptStruct, StructMember, parse_object_table, parse_string_entries, proto_flags,
};
use crate::stream::Stream;
use crate::types::{ArgumentTypeInfo, ConcreteType, ScriptType, VarType};

struct ObsHeader {
    num_bbs: u16,
    num_global_numbers: u16,
    string_table: u32,
    variant_table: u32,
    extern_table: u32,
    prototypes_table: u32,
    typedefs_table: u32,
    bbs_table: u32,
}

const OBS_HEADER_SIZE: usize = 0x100;

impl ObsHeader {
    fn read(data: &[u8]) -> Result<Self> {
        if data.len() < OBS_HEADER_SIZE {
            return Err("Header too small".into());
        }
        let u16_at = |o| Stream::at(data, o).u16();
        let u32_at = |o| Stream::at(data, o).u32();
        Ok(ObsHeader {
            num_bbs: u16_at(0x62)?,
            num_global_numbers: u16_at(0x74)?,
            string_table: u32_at(0x7c)?,
            variant_table: u32_at(0x80)?,
            extern_table: u32_at(0x84)?,
            prototypes_table: u32_at(0x88)?,
            typedefs_table: u32_at(0x8c)?,
            bbs_table: u32_at(0xd8)?,
        })
    }
}

struct ALuZHeader {
    variables_table: u32,
    prototypes_table: u32,
    typedefs_table: u32,
    bbs_table: u32,
    /// Where the compiler records debug symbols; 0 if built without them.
    /// InstallShield 6 headers have text here, so it is range-checked.
    debug_section: u32,
}

const ALUZ_HEADER_SIZE: usize = 124;

impl ALuZHeader {
    fn read(data: &[u8]) -> Result<Self> {
        if data.len() < ALUZ_HEADER_SIZE {
            return Err("Header too small".into());
        }
        let u32_at = |o| Stream::at(data, o).u32();
        Ok(ALuZHeader {
            variables_table: u32_at(104)?,
            prototypes_table: u32_at(108)?,
            typedefs_table: u32_at(112)?,
            bbs_table: u32_at(116)?,
            debug_section: u32_at(120)?,
        })
    }
}

pub fn parse(data: &[u8], kind: HeaderKind) -> Result<Script> {
    let mut script = Script {
        format: Format::New,
        fns: Vec::new(),
        structs: Vec::new(),
        externs: Vec::new(),
        globals: DataDeclList::default(),
        debug_section: debug::Section::None,
        debug: None,
        debug_note: None,
    };
    match kind {
        HeaderKind::Obs => {
            let h = ObsHeader::read(data)?;
            script.externs = read_externs(data, h.extern_table)?;
            script.globals = read_obs_globals(data, &h)?;
            script.structs = read_structs(data, h.typedefs_table, true)?;
            script.fns = read_prototypes(data, h.prototypes_table)?;
            let mut table = Stream::at(data, h.bbs_table as usize);
            let addrs = (0..h.num_bbs)
                .map(|_| table.u32())
                .collect::<Result<Vec<_>>>()?;
            read_bbs(data, &addrs, &mut script.fns)?;
        }
        HeaderKind::ALuZ => {
            let h = ALuZHeader::read(data)?;
            script.structs = read_structs(data, h.typedefs_table, false)?;
            script.globals =
                DataDeclList::parse(&mut Stream::at(data, h.variables_table as usize))?;
            script.fns = read_prototypes(data, h.prototypes_table)?;
            let mut table = Stream::at(data, h.bbs_table as usize);
            let count = table.u16()?;
            let addrs = (0..count)
                .map(|_| table.u32())
                .collect::<Result<Vec<_>>>()?;
            read_bbs(data, &addrs, &mut script.fns)?;
            let offset = h.debug_section as usize;
            if (ALUZ_HEADER_SIZE..data.len()).contains(&offset) {
                script.debug_section = debug::read_section(data, offset);
                match &script.debug_section {
                    debug::Section::Embedded(info) => {
                        let info = info.clone();
                        if !debug::attach(&mut script, info) {
                            script.debug_note =
                                Some("The embedded debug symbols don't match the code.".into());
                        }
                    }
                    debug::Section::Unreadable(e) => {
                        script.debug_note =
                            Some(format!("The embedded debug symbols could not be read: {e}"));
                    }
                    _ => {}
                }
            }
        }
        _ => return Err("Not an InstallShield 6+ script.".into()),
    }
    Ok(script)
}

fn read_externs(data: &[u8], offset: u32) -> Result<Vec<Extern>> {
    let mut s = Stream::at(data, offset as usize);
    let count = s.u16()?;
    let mut out = Vec::new();
    for _ in 0..count {
        let code = s.u8()?;
        let address = s.i16()?;
        let name = s.ins_string()?;
        out.extend(Extern::from_code(code, name, address));
    }
    Ok(out)
}

fn read_obs_globals(data: &[u8], h: &ObsHeader) -> Result<DataDeclList> {
    let objects = parse_object_table(&mut Stream::at(data, h.variant_table as usize))?;
    let mut s = Stream::at(data, h.string_table as usize);
    let count = s.u16()?;
    let _unknown = s.u16()?;
    let strings = parse_string_entries(&mut s, count)?;
    Ok(DataDeclList {
        num_numbers: h.num_global_numbers,
        num_strings: strings.len() as u16,
        objects,
        strings,
    })
}

fn read_structs(data: &[u8], offset: u32, named: bool) -> Result<Vec<ScriptStruct>> {
    let mut s = Stream::at(data, offset as usize);
    let count = s.u16()?;
    let mut out = Vec::new();
    for i in 0..count {
        let name = if named {
            s.ins_string()?
        } else {
            format!("STRUCT_{}", i + 1)
        };
        let n = s.u16()?;
        let mut members = Vec::new();
        for _ in 0..n {
            let ty = ScriptType(s.u8()?);
            let rep_count = s.u16()?;
            let name = s.ins_string()?;
            members.push(StructMember {
                name,
                rep_count,
                ty,
            });
        }
        out.push(ScriptStruct { name, members });
    }
    Ok(out)
}

fn read_args(s: &mut Stream) -> Result<Vec<ArgumentTypeInfo>> {
    let n = s.u16()?;
    (0..n)
        .map(|_| {
            Ok(ArgumentTypeInfo {
                script_type: ScriptType(s.u8()?),
                internal_type: ConcreteType::from_u8(s.u8()?),
            })
        })
        .collect()
}

fn read_prototypes(data: &[u8], offset: u32) -> Result<Vec<Function>> {
    let mut s = Stream::at(data, offset as usize);
    let count = s.u16()?;
    let mut fns = Vec::new();
    for _ in 0..count {
        let flags = s.u8()?;
        let named = |name: &str| flags & proto_flags::IS_EXPORTED != 0 || !name.is_empty();
        let proto = if flags & proto_flags::IS_PREDEFINED_FUNCTION != 0 {
            Prototype {
                flags,
                args: Vec::new(),
                name: String::new(),
                return_type: ScriptType(0),
                bb_id: 0xffff,
            }
        } else if flags & proto_flags::IS_DLL_FUNCTION != 0 {
            let return_type = ScriptType(s.u8()?);
            let dll = s.ins_string()?;
            let name = s.ins_string()?;
            let bb_id = s.u16()?;
            let args = read_args(&mut s)?;
            let name = if named(&name) {
                format!("{dll}.{name}")
            } else {
                format!("Func_{bb_id}")
            };
            Prototype {
                flags,
                args,
                name,
                return_type,
                bb_id,
            }
        } else if flags & proto_flags::IS_INTERNAL_FUNCTION != 0 {
            let return_type = ScriptType(s.u8()?);
            let _align = s.u16()?;
            let name = s.ins_string()?;
            let bb_id = s.u16()?;
            let args = read_args(&mut s)?;
            let name = if named(&name) {
                name
            } else {
                format!("Func_{bb_id}")
            };
            Prototype {
                flags,
                args,
                name,
                return_type,
                bb_id,
            }
        } else {
            return Err(format!("Unknown prototype found (flags {flags:#x})"));
        };
        fns.push(Function::new(proto));
    }
    Ok(fns)
}

fn read_bbs(data: &[u8], addrs: &[u32], fns: &mut [Function]) -> Result<()> {
    let mut current: Option<usize> = None;
    // Index of the block's first action among all actions, as debug symbols
    // count them.
    let mut action = 0u32;
    for (i, &addr) in addrs.iter().enumerate() {
        // A function starts at the block its prototype names. The `program`
        // block has no prolog, so this is the only way to see where it begins.
        if let Ok(f) = Script::fn_index_by_bb_id(fns, i) {
            current = Some(f);
            fns[f].actions = action..action;
        }
        let mut s = Stream::at(data, addr as usize);
        let count = s.u16()?;
        action = action.saturating_add(count as u32);
        let mut bb = BasicBlock {
            id: addr,
            actions: Vec::new(),
        };
        for _ in 0..count {
            match parse_action(&mut s)? {
                Parsed::Prolog(decls) => {
                    let f = Script::fn_index_by_bb_id(fns, i)?;
                    fns[f].decls = decls;
                    current = Some(f);
                }
                Parsed::Action(a) => bb.actions.push(a),
            }
        }
        if let Some(f) = current {
            fns[f].bbs.push(bb);
            fns[f].actions.end = action;
        }
    }
    Ok(())
}

enum Parsed {
    Action(Action),
    Prolog(DataDeclList),
}

fn parse_arg(s: &mut Stream, param: bool) -> Result<Arg> {
    let id = s.i8()?;
    Ok(match id as i32 - 4 {
        0 => Arg::Var {
            ty: VarType::String,
            address: s.i16()?,
            param,
        },
        1 => Arg::Var {
            ty: VarType::Number,
            address: s.i16()?,
            param,
        },
        2 => Arg::Str(s.ins_string()?),
        3 => Arg::Num(s.i32()?),
        4 => Arg::Var {
            ty: VarType::Variant,
            address: s.i16()?,
            param,
        },
        _ => return Err(format!("Invalid argument type {id}")),
    })
}

fn parse_args(s: &mut Stream, n: u16, param: bool) -> Result<Vec<Arg>> {
    (0..n).map(|_| parse_arg(s, param)).collect()
}

/// Reads an action whose operands follow its `id, operand count` header.
fn with_args(s: &mut Stream, op: Op) -> Result<Parsed> {
    let _id = s.u16()?;
    let n = s.u16()?;
    let args = parse_args(s, n, false)?;
    Ok(Parsed::Action(Action { op, args }))
}

fn no_args(s: &mut Stream, op: Op) -> Result<Parsed> {
    let _id = s.u16()?;
    let _n = s.u16()?;
    Ok(Parsed::Action(Action {
        op,
        args: Vec::new(),
    }))
}

fn builtin(s: &mut Stream, name: &'static str, used: usize) -> Result<Parsed> {
    with_args(s, Op::Builtin { name, used })
}

fn parse_action(s: &mut Stream) -> Result<Parsed> {
    let id = s.peek_u16()?;
    let binary = match id {
        7 => Some(BinOp::Add),
        8 => Some(BinOp::Mod),
        9 => Some(BinOp::Lt),
        10 => Some(BinOp::Gt),
        11 => Some(BinOp::Lte),
        12 => Some(BinOp::Gte),
        13 => Some(BinOp::Eq),
        14 => Some(BinOp::Neq),
        15 => Some(BinOp::Sub),
        16 => Some(BinOp::Mul),
        17 => Some(BinOp::Div),
        18 => Some(BinOp::BitAnd),
        19 => Some(BinOp::BitOr),
        20 => Some(BinOp::BitXor),
        22 => Some(BinOp::Shl),
        23 => Some(BinOp::Shr),
        // The original decompiler has these two the other way round; the
        // InstallShield compiler emits 24 for `||` and 25 for `&&`.
        24 => Some(BinOp::LogOr),
        25 => Some(BinOp::LogAnd),
        _ => None,
    };
    if let Some(op) = binary {
        return with_args(s, Op::Binary(op));
    }
    match id {
        1 | 39 => no_args(s, Op::Nop),
        2 => no_args(s, Op::Abort),
        3 => no_args(s, Op::Exit),
        4 => with_args(s, Op::If),
        5 => with_args(s, Op::Goto),
        6 | 59 => with_args(s, Op::Assign),
        21 => with_args(s, Op::Unary('~')),
        26 | 60 => with_args(s, Op::AddressOf),
        27 => with_args(s, Op::Unary('*')),
        28 => {
            let _id = s.u16()?;
            let struct_id = s.u16()?;
            let args = parse_args(s, 1, false)?;
            Ok(Parsed::Action(Action {
                op: Op::IndirectStruct { struct_id },
                args,
            }))
        }
        29 => with_args(s, Op::SetByte),
        30 => with_args(s, Op::GetByte),
        32 | 33 => {
            let _id = s.u16()?;
            let fn_id = s.u16()?;
            let n = s.u16()?;
            let args = parse_args(s, n, id == 33)?;
            Ok(Parsed::Action(Action {
                op: Op::Call { fn_id },
                args,
            }))
        }
        34 => {
            let _id = s.u16()?;
            let _n = s.u16()?;
            let _mark = s.i8()?;
            let base = s.pos();
            let offset = s.u32()? as usize;
            let decls = DataDeclList::parse(&mut s.fork_at(base.saturating_add(offset)))?;
            Ok(Parsed::Prolog(decls))
        }
        35..=37 => with_args(s, Op::Return),
        38 => {
            let parsed = no_args(s, Op::Marker("EndFuncAction"))?;
            DataDeclList::parse(s)?;
            Ok(parsed)
        }
        40 => builtin(s, "StrLengthChars", 1),
        41 => builtin(s, "StrSub", 4),
        42 => builtin(s, "StrFind", 2),
        43 => builtin(s, "StrCompare", 2),
        44 => builtin(s, "StrToNum", 2),
        45 => builtin(s, "NumToStr", 2),
        46 => with_args(s, Op::Marker("Handler")),
        47 => with_args(s, Op::Marker("HandlerEx")),
        48 => with_args(s, Op::Marker("DoHandler")),
        49 => builtin(s, "Resize", 2),
        50 => builtin(s, "SizeOf", 1),
        51 => with_args(s, Op::PropPut),
        52 => with_args(s, Op::PropPutRef),
        53 => with_args(s, Op::PropGet),
        54 => with_args(s, Op::Marker("TryAction")),
        55 => with_args(s, Op::Marker("EndTryAction")),
        56 => with_args(s, Op::Marker("EndCatchAction")),
        57 => builtin(s, "UseDLL", 1),
        58 => builtin(s, "UnUseDLL", 1),
        _ => Err(format!("No handler for action id {id} found")),
    }
}
