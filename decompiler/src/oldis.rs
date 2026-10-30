//! Frontend for InstallShield 5 and earlier scripts (`setup.ins`).

use crate::Result;
use crate::expr::BinOp;
use crate::is5_builtins;
use crate::script::{
    Action, Arg, BasicBlock, DataDeclList, Extern, Format, Function, Op, Prototype, Script,
    ScriptStruct, StructMember, proto_flags,
};
use crate::stream::Stream;
use crate::types::{ArgumentTypeInfo, ConcreteType, ScriptType, VarType};

pub fn parse(data: &[u8]) -> Result<Script> {
    let mut s = Stream::at(data, 0xd);
    let _copyright = s.ins_string()?;
    let num_events = s.u16()?;

    let mut globals = DataDeclList::default();
    let mut externs = Vec::new();

    let count = s.u16()?;
    s.bytes(count as usize * 2)?;
    globals.num_strings = globals.num_strings.wrapping_add(count);

    let count = s.u16()?;
    for _ in 0..count {
        let id = s.u16()?;
        let name = s.ins_string()?;
        externs.push(Extern {
            ty: VarType::String,
            name,
            address: id as i16,
        });
    }
    globals.num_strings = globals.num_strings.wrapping_add(count);

    let global_numbers = s.u16()?;
    globals.num_numbers = globals.num_numbers.wrapping_add(global_numbers);

    let count = s.u16()?;
    for _ in 0..count {
        let id = s.u16()?;
        let name = s.ins_string()?;
        externs.push(Extern {
            ty: VarType::Number,
            name,
            address: id as i16,
        });
    }
    globals.num_numbers = globals.num_numbers.wrapping_add(count);

    let structs = read_structs(&mut s)?;
    let mut fns = read_prototypes(&mut s)?;
    read_events(&mut s, num_events, &mut fns)?;

    Ok(Script {
        format: Format::Old,
        fns,
        structs,
        externs,
        globals,
        debug_section: Default::default(),
        debug: None,
        debug_note: None,
    })
}

fn read_structs(s: &mut Stream) -> Result<Vec<ScriptStruct>> {
    let count = s.u16()?;
    let mut out = Vec::new();
    for _ in 0..count {
        let _packing = s.u16()?;
        let name = s.ins_string()?;
        let fields = s.u16()?;
        let mut members = Vec::new();
        for _ in 0..fields {
            let ty = s.u16()?;
            let _size = s.u16()?;
            let name = s.ins_string()?;
            members.push(StructMember {
                name,
                rep_count: 0,
                ty: ScriptType(ty as u8),
            });
        }
        out.push(ScriptStruct { name, members });
    }
    Ok(out)
}

fn read_prototypes(s: &mut Stream) -> Result<Vec<Function>> {
    let count = s.u16()?;
    let mut fns = Vec::new();
    for i in 0..count {
        let kind = s.u8()?;
        let _reserved = s.u8()?;
        let mut name = String::new();
        let bb_id = 0xffff;
        // Script functions have no stored return type; they return a number.
        let mut return_type = ScriptType(4);
        let mut flags = proto_flags::IS_EXPORTED;
        match kind {
            2 => {
                let _dll = s.ins_string()?;
                name = s.ins_string()?;
                // Always 0; the body is found through the calls instead.
                let _bb_id = s.u16()?;
            }
            1 => {
                return_type = ScriptType(s.u16()? as u8);
                let dll = s.ins_string()?;
                name = format!("{dll}.{}", s.ins_string()?);
                flags |= proto_flags::IS_DLL_FUNCTION;
            }
            _ => {}
        }
        let n = s.u16()?;
        let args = (0..n)
            .map(|_| {
                let script_type = ScriptType(s.u16()? as u8);
                // How the value is passed; not the codes of later versions.
                let internal_type = match s.u16()? {
                    2 => ConcreteType::StringPointer,
                    3 => ConcreteType::NumberPointer,
                    4 => ConcreteType::String,
                    5 => ConcreteType::Number,
                    t => ConcreteType::Invalid(t as u8),
                };
                Ok(ArgumentTypeInfo {
                    script_type,
                    internal_type,
                })
            })
            .collect::<Result<Vec<_>>>()?;
        if name.is_empty() {
            name = format!("Func_{i}");
        }
        // .ins prototypes carry no export flag; mark them all so the functions
        // keep their names.
        let proto = Prototype {
            flags,
            args,
            name,
            return_type,
            bb_id,
        };
        fns.push(Function::new(proto));
    }
    Ok(fns)
}

/// A function body: the blocks from one prolog to the next.
struct Body {
    start: u16,
    decls: DataDeclList,
    bbs: Vec<BasicBlock>,
}

fn read_events(s: &mut Stream, num_events: u16, fns: &mut Vec<Function>) -> Result<()> {
    // The code before the first prolog is the program block.
    let mut bodies = vec![Body {
        start: 0,
        decls: DataDeclList::default(),
        bbs: Vec::new(),
    }];
    for i in 0..num_events {
        let _reserved = s.u16()?;
        let count = s.u16()?;
        let mut bb = BasicBlock {
            id: i as u32,
            actions: Vec::new(),
        };
        for _ in 0..count {
            match parse_action(s, fns)? {
                Parsed::Prolog(decls) => bodies.push(Body {
                    start: i,
                    decls,
                    bbs: Vec::new(),
                }),
                Parsed::Action(a) => bb.actions.push(a),
            }
        }
        bodies.last_mut().unwrap().bbs.push(bb);
    }

    // Prototypes are in declaration order and bodies in definition order;
    // each call names both the prototype and the block its body starts at.
    let mut proto_at = std::collections::HashMap::new();
    for a in bodies
        .iter()
        .flat_map(|b| &b.bbs)
        .flat_map(|bb| &bb.actions)
    {
        if let Op::OldCall { proto, event } = a.op {
            proto_at.entry(event).or_insert(proto as usize);
        }
    }
    for (k, body) in bodies.into_iter().enumerate() {
        let attach = |f: &mut Function| {
            f.proto.bb_id = body.start;
            f.decls = body.decls;
            f.bbs = body.bbs;
        };
        match proto_at.get(&body.start) {
            Some(&p) if k > 0 && fns.get(p).is_some_and(|f| f.bbs.is_empty()) => {
                attach(&mut fns[p]);
            }
            _ => {
                let name = if k == 0 {
                    "program".to_string()
                } else {
                    format!("Func_{}", fns.len())
                };
                let mut f = Function::new(Prototype {
                    flags: proto_flags::IS_EXPORTED,
                    args: Vec::new(),
                    name,
                    return_type: ScriptType(if k == 0 { 8 } else { 4 }),
                    bb_id: 0xffff,
                });
                attach(&mut f);
                fns.push(f);
            }
        }
    }
    Ok(())
}

enum Parsed {
    Action(Action),
    Prolog(DataDeclList),
}

fn parse_arg(s: &mut Stream) -> Result<Arg> {
    let id = s.u8()?;
    let flags = id & 0x0f;
    let var = |s: &mut Stream, ty| {
        Ok(Arg::Var {
            ty,
            address: s.i16()?,
            param: false,
        })
    };
    match id & 0xf0 {
        0x10 => {
            s.u8()?;
        }
        0x30 => return var(s, VarType::Number),
        0x40 if flags & 1 != 0 => return Ok(Arg::Num(s.i32()?)),
        0x40 => return var(s, VarType::Number),
        0x50 => return var(s, VarType::String),
        0x60 if flags & 1 != 0 => return Ok(Arg::Str(s.ins_string()?)),
        0x60 => return var(s, VarType::String),
        _ => {}
    }
    Err(format!("Invalid argument type {}", id as i8))
}

fn parse_args(s: &mut Stream, n: usize) -> Result<Vec<Arg>> {
    (0..n).map(|_| parse_arg(s)).collect()
}

fn arg_count(fns: &[Function], proto: u16) -> Result<usize> {
    fns.get(proto as usize)
        .map(|f| f.proto.args.len())
        .ok_or_else(|| format!("Function #{proto} not found"))
}

/// Operand counts of the instructions without a dedicated decoder, indexed by
/// action id (low nibble).
const NUM_OPERANDS: [u8; 408] = [
    0xff, 0x1, 0x1, 0x2, 0x3, 0x1, 0x0, 0x0, 0x2, 0x2, 0x1, 0x1, 0x1, 0x43, 0x2, 0x2, 0x81, 0x83,
    0x82, 0x2, 0x2, 0x1, 0x2, 0x1, 0x4, 0x3, 0x2, 0xff, 0x82, 0x1, 0x3, 0x81, 0xc0, 0x2, 0x4, 0x2,
    0x3, 0x84, 0x4, 0x4, 0x22, 0x3, 0x2, 0x0, 0x1, 0xff, 0x2, 0x1, 0x4, 0x2, 0x2, 0x62, 0x2, 0x2,
    0x5, 0x2, 0x2, 0x2, 0x1, 0x2, 0x3, 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x1, 0x0, 0x4, 0x3, 0x1, 0x1,
    0x3, 0x3, 0x4, 0x1, 0x1, 0x3, 0x3, 0x2, 0x2, 0x1, 0x1, 0x3, 0x3, 0x4, 0x1, 0x2, 0x2, 0x1, 0x1,
    0x1, 0x4, 0x5, 0x4, 0x2, 0x3, 0x1, 0x1, 0x2, 0x1, 0x1, 0x2, 0x4, 0x3, 0x3, 0x4, 0x84, 0x2, 0x2,
    0x1, 0x2, 0x1, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2, 0x5, 0x3, 0x3, 0x4, 0x4, 0x1, 0x2, 0x2, 0x2,
    0x1, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1, 0x2, 0x2, 0x8, 0x41, 0x1, 0x1, 0x2, 0x3, 0x3, 0x3, 0x2, 0x3,
    0x3, 0x2, 0x4, 0x2, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3, 0x2, 0x3, 0x2, 0x2, 0x2, 0x2,
    0x1, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0xff, 0xff, 0xff, 0x1, 0x1, 0xf0, 0xf0, 0xf0, 0x1, 0x0, 0x1,
    0x1, 0x1, 0x5, 0x5, 0x5, 0x5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1, 0x2, 0x1, 0x8, 0x2, 0x3,
    0x8, 0x3, 0x1, 0x1, 0x9, 0x1, 0x41, 0x1, 0x2, 0x82, 0x1, 0x4, 0x2, 0x3, 0x2, 0x2, 0x3, 0x3,
    0x5, 0x4, 0x3, 0x3, 0x2, 0x43, 0x3, 0x2, 0x4, 0x5, 0x3, 0x2, 0x3, 0x5, 0x2, 0x1, 0x84, 0x85,
    0x3, 0x3, 0x3, 0x1, 0x2, 0x2, 0x1, 0x2, 0x1, 0x4, 0x83, 0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x3, 0x3,
    0xf0, 0xf0, 0x3, 0x3, 0x5, 0x1, 0x1, 0x2, 0x1, 0x2, 0x1, 0x3, 0x3, 0x1, 0x1, 0x3, 0x4, 0x1,
    0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x4, 0x0, 0x2, 0x1, 0x1, 0x2, 0xf0, 0x0, 0x5, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x44, 0x45, 0x43,
    0x8, 0x44, 0x1, 0x1, 0x2, 0xc2, 0x1, 0x4, 0x4, 0x9, 0x42, 0x4, 0x4, 0x2, 0x2, 0x44, 0x4, 0x2,
    0x3, 0x43, 0x2, 0x1, 0x2, 0x5, 0x5, 0x1, 0x41, 0x2, 0x3, 0x3, 0x44, 0x0, 0x2, 0x1, 0x1, 0x5,
    0x5, 0x3, 0x4, 0x4, 0x4, 0x7, 0x3, 0x3, 0x2, 0x3, 0x3, 0x0, 0x3, 0x1, 0x2, 0xf0, 0x2, 0x1, 0x4,
    0x1, 0x3, 0x5, 0x5, 0x6, 0x4, 0x5, 0x2, 0x83, 0x82, 0x5, 0x3, 0x3, 0x1, 0x5, 0x5, 0x5, 0x5,
    0x5, 0x5, 0x1, 0x4, 0x2, 0x4, 0x2, 0x2, 0x2, 0x4, 0x4, 0x2, 0x1, 0x1, 0x2, 0x3, 0x2, 0x5, 0x1,
    0x4, 0x3,
];

fn parse_action(s: &mut Stream, fns: &[Function]) -> Result<Parsed> {
    let id = s.u16()?;
    let action = |op, args| Ok(Parsed::Action(Action { op, args }));
    let binary = match id {
        // String `+` and the path operator `^`.
        0x124 => Some(BinOp::Add),
        0x125 => Some(BinOp::BitXor),
        0x119 => Some(BinOp::Add),
        0x11a => Some(BinOp::Sub),
        0x11b => Some(BinOp::Mul),
        0x11c => Some(BinOp::Div),
        0x11d => Some(BinOp::BitAnd),
        0x11e => Some(BinOp::BitOr),
        0x11f => Some(BinOp::BitXor),
        0x121 => Some(BinOp::Shl),
        0x122 => Some(BinOp::Shr),
        0x123 => Some(BinOp::Mod),
        0x126 => Some(BinOp::LogOr),
        0x127 => Some(BinOp::LogAnd),
        // DEST = LHS OP RHS, with the operator as an operand.
        0x128 => Some(BinOp::CodeDefined),
        _ => None,
    };
    if let Some(op) = binary {
        if id == 0x128 {
            // DEST = LHS code RHS: comparisons, with the operator as a code.
            let mut args = parse_args(s, 4)?;
            let op = match args[2] {
                Arg::Num(1) => BinOp::Lt,
                Arg::Num(2) => BinOp::Gt,
                Arg::Num(3) => BinOp::Lte,
                Arg::Num(4) => BinOp::Gte,
                Arg::Num(5) => BinOp::Eq,
                Arg::Num(6) => BinOp::Neq,
                _ => return action(Op::Binary(op), args),
            };
            args.remove(2);
            return action(Op::Binary(op), args);
        }
        return action(Op::Binary(op), parse_args(s, 3)?);
    }
    match id {
        0x13 | 0x21 => action(Op::Assign, parse_args(s, 2)?),
        0x22 => {
            let label = s.u8()?;
            let target = s.u16()?;
            let _padding = s.u8()?;
            let args = parse_args(s, 1)?;
            s.bytes(5)?;
            action(Op::OldIf { label, target }, args)
        }
        0x2b | 0xb8 => action(Op::ReturnNop, Vec::new()),
        0x2c => {
            let _kind = s.u8()?;
            let target = s.u16()?;
            action(Op::OldGoto { target }, Vec::new())
        }
        0xb5 => {
            let _proto_code = s.u8()?;
            let proto = s.u16()?;
            let _label_code = s.u8()?;
            let event = s.u16()?;
            let args = parse_args(s, arg_count(fns, proto)?)?;
            action(Op::OldCall { proto, event }, args)
        }
        0xb6 => {
            let _reserved = s.u16()?;
            let num_strings = s.u16()?;
            s.bytes(num_strings as usize * 2)?;
            let _zeros = s.u16()?;
            let num_numbers = s.u16()?;
            let _zeros = s.u16()?;
            Ok(Parsed::Prolog(DataDeclList {
                num_numbers,
                num_strings,
                ..Default::default()
            }))
        }
        0xb7 => action(Op::Return, parse_args(s, 1)?),
        0x112 => action(Op::LoadStringConst, parse_args(s, 3)?),
        0x120 => action(Op::Unary('~'), parse_args(s, 3)?),
        0x12f => action(Op::Nop, Vec::new()),
        0x181 => action(Op::OldProp("PropGet"), parse_args(s, 5)?),
        0x183 => action(Op::OldProp("PropPut"), parse_args(s, 5)?),
        // A call to a DLL function, by prototype index.
        0xb4 => {
            let _marker = s.u8()?;
            let proto = s.u16()?;
            let args = parse_args(s, arg_count(fns, proto)?)?;
            action(Op::Call { fn_id: proto }, args)
        }
        // Sprintf(dest, format, ...) and SprintfBox(type, title, format, ...).
        0x103 | 0x104 => {
            let _marker = s.u8()?;
            let n = s.u16()? as usize;
            let name = if id == 0x103 { "Sprintf" } else { "SprintfBox" };
            action(Op::Builtin { name, used: n }, parse_args(s, n)?)
        }
        // Marks where a `switch` starts; the cases follow as `if`s.
        0x129 => action(Op::Nop, Vec::new()),
        0x159 => action(Op::Abort, Vec::new()),
        // AskOptions(type, message, text, BYREF check, ...): a count of
        // text/check pairs after the two fixed arguments.
        0x20 => {
            let _marker = s.u8()?;
            let pairs = s.u16()? as usize;
            let n = 2 + 2 * pairs;
            action(
                Op::Builtin {
                    name: "AskOptions",
                    used: n,
                },
                parse_args(s, n)?,
            )
        }
        0x16d => Err(format!(
            "Unimplemented action {id:#x} at offset {:#x}",
            s.pos() - 2
        )),
        _ => {
            let n = NUM_OPERANDS
                .get(id as usize)
                .ok_or_else(|| format!("Unknown action id {id:#x}"))?;
            let args = parse_args(s, (n & 0x0f) as usize)?;
            match is5_builtins::name(id) {
                Some(name) => action(
                    Op::Builtin {
                        name,
                        used: args.len(),
                    },
                    args,
                ),
                None => action(Op::Placeholder(id), args),
            }
        }
    }
}
