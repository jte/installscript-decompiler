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
    /// A call argument passed `BYREF`; the callee may assign to it.
    Ref(Box<Expr>),
    /// A function named as a value, as `BindVariable` takes one.
    Function(String),
    PropPut(Box<Expr>, Vec<Expr>),
    PropGet(Box<Expr>, Vec<Expr>),
    StringConstant(Box<Expr>),
}

/// How many times an expression mentions a variable: in all, and in value
/// positions, where another expression could stand in its place.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Uses {
    pub total: usize,
    pub values: usize,
}

impl std::ops::AddAssign for Uses {
    fn add_assign(&mut self, o: Uses) {
        self.total += o.total;
        self.values += o.values;
    }
}

impl Expr {
    pub fn render(&self, syms: &Symbols) -> String {
        let mut out = String::new();
        self.write(&mut out, syms);
        out
    }

    /// Mentions of `v`. Only plain values count as substitutable: the
    /// operand of `&`, an indexed or accessed base and a `set` target need a
    /// variable.
    pub fn uses(&self, v: VarId) -> Uses {
        let mut u = Uses::default();
        self.count(v, true, &mut u);
        u
    }

    fn count(&self, v: VarId, value: bool, u: &mut Uses) {
        match self {
            Expr::Var(x) if *x == v => {
                u.total += 1;
                u.values += value as usize;
            }
            Expr::Number(_) | Expr::Str(_) | Expr::Var(_) | Expr::Function(_) => {}
            Expr::Binary(_, l, r) => {
                l.count(v, true, u);
                r.count(v, true, u);
            }
            // `*p` needs `p` to be a variable.
            Expr::Unary('*', e) => e.count(v, false, u),
            Expr::Unary(_, e) | Expr::StringConstant(e) => e.count(v, true, u),
            Expr::AddressOf(e) | Expr::SetRef(e) | Expr::Ref(e) => e.count(v, false, u),
            Expr::ArrayAccess(b, i) => {
                b.count(v, false, u);
                i.count(v, true, u);
            }
            Expr::Call(_, args) => args.iter().for_each(|a| a.count(v, true, u)),
            Expr::PropPut(b, acc) | Expr::PropGet(b, acc) => {
                b.count(v, false, u);
                acc.iter().for_each(|a| a.count(v, true, u));
            }
        }
    }

    /// Replaces `v` in value positions with `with`.
    pub fn substitute(&mut self, v: VarId, with: &Expr) {
        match self {
            Expr::Var(x) if *x == v => *self = with.clone(),
            Expr::Number(_) | Expr::Str(_) | Expr::Var(_) | Expr::Function(_) => {}
            Expr::Binary(_, l, r) => {
                l.substitute(v, with);
                r.substitute(v, with);
            }
            Expr::Unary('*', _) => {}
            Expr::Unary(_, e) | Expr::StringConstant(e) => e.substitute(v, with),
            Expr::AddressOf(_) | Expr::SetRef(_) | Expr::Ref(_) => {}
            Expr::ArrayAccess(_, i) => i.substitute(v, with),
            Expr::Call(_, args) => args.iter_mut().for_each(|a| a.substitute(v, with)),
            Expr::PropPut(_, acc) | Expr::PropGet(_, acc) => {
                acc.iter_mut().for_each(|a| a.substitute(v, with))
            }
        }
    }

    /// Writes an operand of a binary or unary operator, in parentheses
    /// unless it is atomic or continues a left-associative chain of the same
    /// operator. InstallScript's precedence rules aren't documented in full,
    /// so nesting is always made explicit.
    fn write_operand(&self, out: &mut String, syms: &Symbols, parent: Option<BinOp>) {
        let bare = match self {
            Expr::Binary(op, ..) => Some(*op) == parent,
            Expr::Unary(..) | Expr::AddressOf(_) | Expr::SetRef(_) => false,
            _ => true,
        };
        if bare {
            self.write(out, syms);
        } else {
            out.push('(');
            self.write(out, syms);
            out.push(')');
        }
    }

    /// Whether the value is a truth value: a comparison, `&&`, `||` or `!`.
    fn is_condition(&self) -> bool {
        match self {
            Expr::Binary(op, ..) => matches!(
                op,
                BinOp::Lt
                    | BinOp::Gt
                    | BinOp::Lte
                    | BinOp::Gte
                    | BinOp::Eq
                    | BinOp::Neq
                    | BinOp::LogAnd
                    | BinOp::LogOr
            ),
            Expr::Unary('!', _) => true,
            _ => false,
        }
    }

    fn write(&self, out: &mut String, syms: &Symbols) {
        match self {
            // InstallShield 5 compiles `a < b` on strings to this.
            Expr::Binary(op, call, zero)
                if matches!(
                    op,
                    BinOp::Lt | BinOp::Gt | BinOp::Lte | BinOp::Gte | BinOp::Eq | BinOp::Neq
                ) && matches!(**zero, Expr::Number(0))
                    && matches!(&**call, Expr::Call(name, args) if name == "StrCompare" && args.len() == 2) =>
            {
                let Expr::Call(_, args) = &**call else {
                    unreachable!()
                };
                Expr::Binary(*op, Box::new(args[0].clone()), Box::new(args[1].clone()))
                    .write(out, syms);
            }
            // How the compiler spells `!(c)`.
            Expr::Binary(BinOp::Eq, c, zero)
                if c.is_condition() && matches!(**zero, Expr::Number(0)) =>
            {
                out.push('!');
                c.write_operand(out, syms, None);
            }
            Expr::Number(n) => out.push_str(&n.to_string()),
            // The compiler rejects literals over 255 characters.
            Expr::Str(s) if s.chars().count() > 255 => {
                let chars: Vec<char> = s.chars().collect();
                let parts: Vec<String> = chars
                    .chunks(200)
                    .map(|c| quote(&c.iter().collect::<String>()))
                    .collect();
                out.push('(');
                out.push_str(&parts.join(" + "));
                out.push(')');
            }
            Expr::Str(s) => out.push_str(&quote(s)),
            Expr::Var(v) => out.push_str(&syms.vars[*v].name),
            Expr::Binary(op, l, r) => {
                l.write_operand(out, syms, Some(*op));
                out.push(' ');
                out.push_str(op.symbol());
                out.push(' ');
                r.write_operand(out, syms, None);
            }
            Expr::Unary(op, e) => {
                out.push(*op);
                e.write_operand(out, syms, None);
            }
            Expr::AddressOf(e) => {
                out.push('&');
                e.write_operand(out, syms, None);
            }
            Expr::Ref(e) => e.write(out, syms),
            Expr::Function(name) => out.push_str(name),
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
            // The first accessor names the member, empty for an array element
            // or default member; the rest are its arguments.
            Expr::PropPut(var, accessors) | Expr::PropGet(var, accessors) => {
                var.write(out, syms);
                let args = match accessors.split_first() {
                    Some((Expr::Str(name), args)) => {
                        if !name.is_empty() {
                            out.push('.');
                            out.push_str(name);
                        }
                        args
                    }
                    _ => accessors,
                };
                if !args.is_empty() {
                    out.push('(');
                    for (i, a) in args.iter().enumerate() {
                        if i > 0 {
                            out.push_str(", ");
                        }
                        a.write(out, syms);
                    }
                    out.push(')');
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
    /// An `if` whose false branch jumps backwards, as at the end of a
    /// `repeat … until` loop: `if !cond then goto target; endif;`.
    GotoUnless {
        cond: Expr,
        target: usize,
    },
    Return(Option<Expr>),
    Abort,
    Exit,
    Nop,
}

impl Stmt {
    /// Mentions of `v`, not counting being assigned to.
    pub fn uses(&self, v: VarId) -> Uses {
        let mut u = Uses::default();
        match self {
            Stmt::Assign(lhs, rhs) => {
                if !matches!(lhs, Expr::Var(x) if *x == v) {
                    // Elements and members are written through their base.
                    u += match lhs {
                        Expr::Var(_) => Uses::default(),
                        _ => lhs.uses(v),
                    };
                }
                u += rhs.uses(v);
            }
            Stmt::If { cond, .. } | Stmt::GotoUnless { cond, .. } => u += cond.uses(v),
            Stmt::Return(Some(e)) => u += e.uses(v),
            _ => {}
        }
        u
    }

    pub fn substitute(&mut self, v: VarId, with: &Expr) {
        match self {
            Stmt::Assign(lhs, rhs) => {
                if !matches!(lhs, Expr::Var(_)) {
                    lhs.substitute(v, with);
                }
                rhs.substitute(v, with);
            }
            Stmt::If { cond, .. } | Stmt::GotoUnless { cond, .. } => cond.substitute(v, with),
            Stmt::Return(Some(e)) => e.substitute(v, with),
            _ => {}
        }
    }

    /// The variable this statement assigns as a whole, if any.
    pub fn assigned_var(&self) -> Option<VarId> {
        match self {
            Stmt::Assign(Expr::Var(v), _) => Some(*v),
            _ => None,
        }
    }
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
