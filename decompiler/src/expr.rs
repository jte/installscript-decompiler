//! Expressions and the flat statement list that actions lower to.

use crate::script::quote;
use crate::vars::{Symbols, VarId};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum BinOp {
    Add,
    Sub,
    Mod,
    Lt,
    Gt,
    Lte,
    Gte,
    Eq,
    Neq,
    Mul,
    Div,
    BitAnd,
    BitOr,
    BitXor,
    Shl,
    Shr,
    LogAnd,
    LogOr,
    /// `.ins` binary operation whose operator is a runtime operand.
    CodeDefined,
}

impl BinOp {
    pub fn symbol(self) -> &'static str {
        match self {
            BinOp::Add => "+",
            BinOp::Sub => "-",
            BinOp::Mod => "%",
            BinOp::Lt => "<",
            BinOp::Gt => ">",
            BinOp::Lte => "<=",
            BinOp::Gte => ">=",
            BinOp::Eq => "==",
            BinOp::Neq => "!=",
            BinOp::Mul => "*",
            BinOp::Div => "/",
            BinOp::BitAnd => "&",
            BinOp::BitOr => "|",
            BinOp::BitXor => "^",
            BinOp::Shl => "<<",
            BinOp::Shr => ">>",
            BinOp::LogAnd => "&&",
            BinOp::LogOr => "||",
            BinOp::CodeDefined => "undefined",
        }
    }
}

#[derive(Clone, Debug)]
pub enum Expr {
    Number(i32),
    Str(String),
    Var(VarId),
    Binary(BinOp, Box<Expr>, Box<Expr>),
    Unary(char, Box<Expr>),
    AddressOf(Box<Expr>),
    ArrayAccess(Box<Expr>, Box<Expr>),
    SetRef(Box<Expr>),
    Call(String, Vec<Expr>),
    PropPut(Box<Expr>, Vec<Expr>),
    PropGet(Box<Expr>, Vec<Expr>),
    StringConstant(Box<Expr>),
}

impl Expr {
    pub fn render(&self, syms: &Symbols) -> String {
        let mut out = String::new();
        self.write(&mut out, syms);
        out
    }

    fn write(&self, out: &mut String, syms: &Symbols) {
        match self {
            Expr::Number(n) => out.push_str(&n.to_string()),
            Expr::Str(s) => out.push_str(&quote(s)),
            Expr::Var(v) => out.push_str(&syms.vars[*v].name),
            Expr::Binary(op, l, r) => {
                l.write(out, syms);
                out.push(' ');
                out.push_str(op.symbol());
                out.push(' ');
                r.write(out, syms);
            }
            Expr::Unary(op, e) => {
                out.push(*op);
                e.write(out, syms);
            }
            Expr::AddressOf(e) => {
                out.push('&');
                e.write(out, syms);
            }
            Expr::ArrayAccess(v, i) => {
                v.write(out, syms);
                out.push('[');
                i.write(out, syms);
                out.push(']');
            }
            Expr::SetRef(e) => {
                out.push_str("set ");
                e.write(out, syms);
            }
            Expr::Call(name, args) => {
                out.push_str(name);
                out.push('(');
                for (i, a) in args.iter().enumerate() {
                    if i > 0 {
                        out.push_str(", ");
                    }
                    a.write(out, syms);
                }
                out.push(')');
            }
            Expr::PropPut(var, accessors) | Expr::PropGet(var, accessors) => {
                var.write(out, syms);
                for a in accessors {
                    match a {
                        Expr::Str(s) => {
                            out.push('.');
                            out.push_str(s);
                        }
                        _ => {
                            out.push('(');
                            a.write(out, syms);
                            out.push(')');
                        }
                    }
                }
            }
            Expr::StringConstant(e) => {
                out.push('@');
                match &**e {
                    Expr::Str(s) => out.push_str(s),
                    e => e.write(out, syms),
                }
            }
        }
    }
}

#[derive(Clone, Debug)]
pub enum Stmt {
    Assign(Expr, Expr),
    /// `then` is the statement after the `if`, `else_` the first statement of
    /// the block the condition jumps to when false; both index the list.
    If {
        cond: Expr,
        then: Option<usize>,
        else_: Option<usize>,
    },
    Goto {
        target: Option<usize>,
    },
    Return(Option<Expr>),
    Abort,
    Exit,
    Nop,
}

/// One statement of a function's flat statement list.
#[derive(Clone, Debug)]
pub struct Node {
    pub stmt: Stmt,
    /// Label printed before the statement when a `goto` targets it.
    pub label: String,
    /// For gotos: the label of the target.
    pub target_label: String,
}

impl Node {
    pub fn new(stmt: Stmt) -> Self {
        Node {
            stmt,
            label: String::new(),
            target_label: String::new(),
        }
    }
}
