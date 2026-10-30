//! Turns a function's actions into a flat statement list and resolves the
//! targets of `if` and `goto`.

use crate::Result;
use crate::expr::{Expr, Node, Stmt};
use crate::script::{Action, Arg, BasicBlock, Format, Op, Script};
use crate::types::VarType;
use crate::vars::{Symbols, TableId, is_global_address};

pub struct Ctx<'a> {
    pub script: &'a Script,
    pub syms: &'a Symbols,
    pub table: TableId,
}

impl Ctx<'_> {
    /// The variable that receives call results.
    fn last_result(&self) -> Result<Expr> {
        let v = match self.script.format {
            Format::New => self
                .syms
                .by_name(self.table, "LAST_RESULT", VarType::Variant, true)?,
            Format::Old => self.syms.by_address(self.table, 0, VarType::Number, true)?,
        };
        Ok(Expr::Var(v))
    }

    fn arg_expr(&self, arg: &Arg) -> Result<Expr> {
        Ok(match arg {
            Arg::Num(n) => Expr::Number(*n),
            Arg::Str(s) => Expr::Str(s.clone()),
            Arg::Var { ty, address, .. } => {
                let address = *address as i32;
                Expr::Var(self.syms.by_address(
                    self.table,
                    address,
                    *ty,
                    is_global_address(address),
                )?)
            }
        })
    }

    fn lower(&self, a: &Action) -> Result<Option<Stmt>> {
        let arg = |i: usize| match a.args.get(i) {
            Some(x) => self.arg_expr(x),
            None => Err(format!("{:?} is missing operand {i}", a.op)),
        };
        let args = |n: usize| (0..n).map(arg).collect::<Result<Vec<_>>>();
        let b = Box::new;
        let stmt = match &a.op {
            Op::Nop | Op::ReturnNop => Stmt::Assign(self.last_result()?, self.last_result()?),
            Op::Abort => Stmt::Abort,
            Op::Exit => Stmt::Exit,
            Op::Marker(_) => Stmt::Nop,
            Op::If => Stmt::If {
                cond: arg(1)?,
                then: None,
                else_: None,
            },
            Op::OldIf { .. } => Stmt::If {
                cond: arg(0)?,
                then: None,
                else_: None,
            },
            Op::Goto | Op::OldGoto { .. } => Stmt::Goto { target: None },
            Op::Assign => Stmt::Assign(arg(0)?, arg(1)?),
            Op::AddressOf => Stmt::Assign(arg(0)?, Expr::AddressOf(b(arg(1)?))),
            Op::Binary(op) => Stmt::Assign(arg(0)?, Expr::Binary(*op, b(arg(1)?), b(arg(2)?))),
            Op::Unary(op) => Stmt::Assign(arg(0)?, Expr::Unary(*op, b(arg(1)?))),
            Op::IndirectStruct { .. } => {
                Stmt::Assign(self.last_result()?, Expr::Unary('*', b(arg(0)?)))
            }
            Op::SetByte => Stmt::Assign(Expr::ArrayAccess(b(arg(0)?), b(arg(1)?)), arg(2)?),
            Op::GetByte => Stmt::Assign(arg(0)?, Expr::ArrayAccess(b(arg(1)?), b(arg(2)?))),
            Op::Call { .. } | Op::OldCall { .. } => {
                let name = self.script.callee_name(&a.op)?.to_string();
                Stmt::Assign(self.last_result()?, Expr::Call(name, args(a.args.len())?))
            }
            Op::Return => Stmt::Return(if a.args.is_empty() {
                None
            } else {
                Some(arg(0)?)
            }),
            Op::Builtin { name, used } => Stmt::Assign(
                self.last_result()?,
                Expr::Call(name.to_string(), args(*used)?),
            ),
            Op::PropPut => {
                if a.args.len() < 2 {
                    return Err("PropPut needs at least two operands".into());
                }
                let last = a.args.len() - 1;
                let accessors = (1..last).map(arg).collect::<Result<Vec<_>>>()?;
                Stmt::Assign(Expr::PropPut(b(arg(0)?), accessors), arg(last)?)
            }
            Op::PropPutRef => Stmt::Assign(Expr::SetRef(b(arg(0)?)), arg(1)?),
            Op::PropGet => {
                let mut accessors = Vec::new();
                for i in 1..a.args.len() {
                    match arg(i)? {
                        Expr::Str(s) if s.is_empty() => {}
                        e => accessors.push(e),
                    }
                }
                Stmt::Assign(self.last_result()?, Expr::PropGet(b(arg(0)?), accessors))
            }
            Op::OldProp(_) => return Ok(None),
            Op::LoadStringConst => Stmt::Assign(arg(2)?, Expr::StringConstant(b(arg(1)?))),
            Op::Placeholder(id) => Stmt::Assign(
                self.last_result()?,
                Expr::Call(format!("PlaceholderAction_0x{id:x}"), args(a.args.len())?),
            ),
        };
        Ok(Some(stmt))
    }
}

fn const_operand(a: &Action) -> Result<i32> {
    match a.args.first() {
        Some(Arg::Num(n)) => Ok(*n),
        _ => Err(format!("{:?} has no constant jump offset", a.op)),
    }
}

fn label(nodes: &mut [Node], target: usize, counter: &mut usize) {
    if nodes[target].label.is_empty() {
        nodes[target].label = format!("label_{counter}");
        *counter += 1;
    }
}

pub fn lower_function(ctx: &Ctx, bbs: &[BasicBlock]) -> Result<Vec<Node>> {
    let mut nodes = Vec::new();
    let mut index: Vec<Vec<Option<usize>>> = Vec::with_capacity(bbs.len());
    for bb in bbs {
        let mut row = Vec::with_capacity(bb.actions.len());
        for a in &bb.actions {
            row.push(match ctx.lower(a)? {
                Some(stmt) => {
                    nodes.push(Node::new(stmt));
                    Some(nodes.len() - 1)
                }
                None => None,
            });
        }
        index.push(row);
    }

    // First statement of block `bb`, as a jump target.
    let block_start = |bb: i64, what: &str| -> Result<usize> {
        usize::try_from(bb)
            .ok()
            .and_then(|bb| index.get(bb))
            .and_then(|row| row.first().copied().flatten())
            .ok_or_else(|| format!("Invalid {what} expression found"))
    };
    let block_by_id = |id: u16, what: &str| -> Result<i64> {
        bbs.iter()
            .position(|bb| bb.id == id as u32)
            .map(|i| i as i64)
            .ok_or_else(|| format!("Invalid bb address for {what}"))
    };

    let mut label_counter = 1;
    for (i, bb) in bbs.iter().enumerate() {
        for (j, a) in bb.actions.iter().enumerate() {
            let Some(node) = index[i][j] else { continue };
            let else_block = match a.op {
                // Negative for the backward jump that closes a `repeat` loop.
                Op::If => Some(i as i64 + const_operand(a)? as i64),
                Op::OldIf { target, .. } => Some(block_by_id(target, "continue branch in if")?),
                _ => None,
            };
            if let Some(else_block) = else_block {
                let else_node = block_start(else_block, "else")?;
                if else_node <= node {
                    label(&mut nodes, else_node, &mut label_counter);
                    nodes[node].target_label = nodes[else_node].label.clone();
                    let stmt = std::mem::replace(&mut nodes[node].stmt, Stmt::Nop);
                    if let Stmt::If { cond, .. } = stmt {
                        nodes[node].stmt = Stmt::GotoUnless {
                            cond,
                            target: else_node,
                        };
                    }
                    continue;
                }
                let then_node = match bb.actions.get(j + 1) {
                    Some(_) => Some(index[i][j + 1].ok_or("Invalid then expression found")?),
                    None => None,
                };
                if let Stmt::If { then, else_, .. } = &mut nodes[node].stmt {
                    *then = then_node;
                    *else_ = Some(else_node);
                }
                continue;
            }
            let goto_block = match a.op {
                Op::Goto => i as i64 + const_operand(a)? as i64,
                Op::OldGoto { target } => block_by_id(target, "target address in goto")?,
                _ => continue,
            };
            let target = block_start(goto_block, "goto target")?;
            label(&mut nodes, target, &mut label_counter);
            nodes[node].target_label = nodes[target].label.clone();
            nodes[node].stmt = Stmt::Goto {
                target: Some(target),
            };
        }
    }
    Ok(nodes)
}
