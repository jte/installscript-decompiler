//! Frontend for InstallShield 5 and earlier scripts (`setup.ins`).

use crate::Result;
use crate::expr::BinOp;
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
        let return_type = ScriptType(s.u8()?);
        let mut name = String::new();
        let mut bb_id = 0xffff;
        match kind {
            2 => {
                let _dll = s.ins_string()?;
                name = s.ins_string()?;
                bb_id = s.u16()?;
            }
            1 => {
                let _unknown = s.u16()?;
                let _dll = s.ins_string()?;
                name = s.ins_string()?;
            }
            _ => {}
        }
        let n = s.u16()?;
        let args = (0..n)
            .map(|_| {
                let script_type = ScriptType(s.u16()? as u8);
                let internal_type = ConcreteType::from_u8(s.u16()? as u8);
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
            flags: proto_flags::IS_EXPORTED,
            args,
            name,
            return_type,
            bb_id,
        };
        fns.push(Function {
            decls: DataDeclList::default(),
            proto,
            bbs: Vec::new(),
        });
    }
    Ok(fns)
}

fn read_events(s: &mut Stream, num_events: u16, fns: &mut [Function]) -> Result<()> {
    let mut func = 0usize;
    for i in 0..num_events {
        let _reserved = s.u16()?;
        let count = s.u16()?;
        let mut bb = BasicBlock {
            id: i as u32,
            actions: Vec::new(),
        };
        if func >= fns.len() {
            return Err(format!("Function #{func} not found"));
        }
        for _ in 0..count {
            match parse_action(s, fns)? {
                Parsed::Prolog(decls) => {
                    func += 1;
                    let f = fns
                        .get_mut(func)
                        .ok_or_else(|| format!("Function #{func} not found"))?;
                    f.proto.bb_id = i;
                    f.decls = decls;
                }
                Parsed::Action(a) => bb.actions.push(a),
            }
        }
        fns[func].bbs.push(bb);
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
        let n = if id == 0x128 { 4 } else { 3 };
        return action(Op::Binary(op), parse_args(s, n)?);
    }
    match id {
        0x01 => action(
            Op::Builtin {
                name: "Enable",
                used: 1,
            },
            parse_args(s, 1)?,
        ),
        0x02 => action(
            Op::Builtin {
                name: "Disable",
                used: 1,
            },
            parse_args(s, 1)?,
        ),
        0x04 => action(
            Op::Builtin {
                name: "SetTitle",
                used: 1,
            },
            parse_args(s, 3)?,
        ),
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
        0xb4 => {
            let _marker = s.u8()?;
            let proto = s.u16()?;
            let args = parse_args(s, arg_count(fns, proto)?)?;
            action(Op::Placeholder(id), args)
        }
        0x103 | 0x104 => {
            let skipped = if id == 0x103 { 2 } else { 3 };
            let _marker = s.u8()?;
            let n = s.u16()? as usize;
            parse_args(s, skipped)?;
            let n = n
                .checked_sub(skipped)
                .ok_or_else(|| format!("Invalid operand count in action {id:#x}"))?;
            action(Op::Placeholder(id), parse_args(s, n)?)
        }
        0x16d | 0x20 => Err(format!("Unimplemented action {id:#x}")),
        _ => {
            let n = NUM_OPERANDS
                .get(id as usize)
                .ok_or_else(|| format!("Unknown action id {id:#x}"))?;
            action(Op::Placeholder(id), parse_args(s, (n & 0x0f) as usize)?)
        }
    }
}
