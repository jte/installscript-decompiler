//! Turns a function's block graph into structured statements and prints them.
//!
//! The block graph nests `if` bodies but leaves `else`, loops and many jumps
//! as `goto`s. The passes here rewrite the patterns the compiler produces for
//! `if … else`, `while`, `repeat … until` and `for` back into those
//! statements, each only where the result means the same thing.

use std::collections::HashMap;
use std::fmt::Write;

use crate::expr::{BinOp, Expr, Node, Stmt};
use crate::ir::{Cfg, IrStmt};
use crate::vars::{Symbols, VarId};

#[derive(Debug)]
enum Ast {
    /// The label of a statement some `goto` may target.
    Label(usize),
    /// A plain statement: assignment, `goto`, `return`, `exit`, `abort`, or a
    /// backwards conditional jump.
    Stmt(usize),
    If {
        node: usize,
        then: Vec<Ast>,
        else_: Vec<Ast>,
    },
    While {
        node: usize,
        body: Vec<Ast>,
    },
    /// `node` is the backwards jump taken while the condition is false.
    Repeat {
        body: Vec<Ast>,
        node: usize,
    },
    /// `for v = from to limit [step n]`: `init` assigns the start, `node` is
    /// the loop test, `step` the increment.
    For {
        init: usize,
        node: usize,
        step: usize,
        body: Vec<Ast>,
    },
}

fn has_label(item: &mut Ast) -> bool {
    matches!(item, Ast::Label(_))
        || children(item)
            .into_iter()
            .any(|c| c.iter_mut().any(has_label))
}

/// Children of an item, for passes that recurse.
fn children(item: &mut Ast) -> Vec<&mut Vec<Ast>> {
    match item {
        Ast::If { then, else_, .. } => vec![then, else_],
        Ast::While { body, .. } | Ast::Repeat { body, .. } | Ast::For { body, .. } => vec![body],
        Ast::Label(_) | Ast::Stmt(_) => vec![],
    }
}

fn from_block(cfg: &Cfg, bb: usize, nodes: &[Node], seen: &mut [bool]) -> Vec<Ast> {
    let mut out = Vec::new();
    if std::mem::replace(&mut seen[bb], true) {
        return out;
    }
    for s in &cfg.blocks[bb].stmts {
        let (IrStmt::Plain(i) | IrStmt::Branch { node: i, .. }) = *s;
        if !nodes[i].label.is_empty() {
            out.push(Ast::Label(i));
        }
        match *s {
            IrStmt::Plain(i) => {
                if !matches!(nodes[i].stmt, Stmt::Nop) {
                    out.push(Ast::Stmt(i));
                }
            }
            IrStmt::Branch {
                node,
                then_bb,
                cont_bb,
            } => {
                let then = from_block(cfg, then_bb, nodes, seen);
                out.push(Ast::If {
                    node,
                    then,
                    else_: Vec::new(),
                });
                out.extend(from_block(cfg, cont_bb, nodes, seen));
            }
        }
    }
    out
}

struct Structurer<'a> {
    nodes: &'a [Node],
    /// How many `goto`s in the tree target each label.
    refs: HashMap<&'a str, usize>,
}

impl<'a> Structurer<'a> {
    fn label(&self, n: usize) -> &'a str {
        &self.nodes[n].label
    }

    /// The label a trailing `goto` of `seq` jumps to.
    fn trailing_goto(&self, seq: &[Ast]) -> Option<&'a str> {
        match seq.last()? {
            Ast::Stmt(g) if matches!(self.nodes[*g].stmt, Stmt::Goto { .. }) => {
                Some(&self.nodes[*g].target_label)
            }
            _ => None,
        }
    }

    fn is_label(&self, item: &Ast, name: &str) -> bool {
        matches!(item, Ast::Label(n) if self.label(*n) == name)
    }

    /// `if c then T; goto L; endif; E; L:` → `if c then T else E endif; L:`.
    ///
    /// `cont` is the label control reaches when it runs off the end of
    /// `seq`, if that is known: then `L` may also be that label, with `E`
    /// running to the end of `seq`.
    fn recover_else(&self, seq: &mut Vec<Ast>, cont: Option<&str>) {
        let mut k = 0;
        while k < seq.len() {
            let target = match &seq[k] {
                Ast::If { then, else_, .. } if else_.is_empty() => self.trailing_goto(then),
                _ => None,
            };
            let end = target.and_then(|l| {
                (k + 1..seq.len())
                    .find(|&m| self.is_label(&seq[m], l))
                    .or((Some(l) == cont).then_some(seq.len()))
            });
            if let Some(m) = end {
                let moved: Vec<Ast> = seq.drain(k + 1..m).collect();
                if let Ast::If { then, else_, .. } = &mut seq[k] {
                    then.pop();
                    *else_ = moved;
                }
            }
            k += 1;
        }
        // What follows each item: the label after it, or this sequence's own
        // continuation for the last one.
        let conts: Vec<Option<&str>> = (0..seq.len())
            .map(|k| match seq.get(k + 1) {
                Some(Ast::Label(n)) => Some(self.label(*n)),
                Some(_) => None,
                None => cont,
            })
            .collect();
        for (item, after) in seq.iter_mut().zip(conts) {
            match item {
                Ast::If { then, else_, .. } => {
                    self.recover_else(then, after);
                    self.recover_else(else_, after);
                }
                _ => {
                    for child in children(item) {
                        self.recover_else(child, None);
                    }
                }
            }
        }
    }

    /// Drops a `goto` at the end of a branch that jumps to where the branch
    /// ends anyway.
    fn remove_redundant_gotos(&self, seq: &mut Vec<Ast>, cont: Option<&str>) {
        let conts: Vec<Option<&str>> = (0..seq.len())
            .map(|k| match seq.get(k + 1) {
                Some(Ast::Label(n)) => Some(self.label(*n)),
                Some(_) => None,
                None => cont,
            })
            .collect();
        for (item, after) in seq.iter_mut().zip(conts) {
            match item {
                Ast::If { then, else_, .. } => {
                    self.remove_redundant_gotos(then, after);
                    self.remove_redundant_gotos(else_, after);
                }
                _ => {
                    for child in children(item) {
                        self.remove_redundant_gotos(child, None);
                    }
                }
            }
        }
        if cont.is_some() && self.trailing_goto(seq) == cont {
            seq.pop();
        }
    }

    fn terminates(&self, item: &Ast) -> bool {
        matches!(item, Ast::Stmt(n) if matches!(
            self.nodes[*n].stmt,
            Stmt::Goto { .. } | Stmt::Return(_) | Stmt::Exit | Stmt::Abort
        ))
    }

    /// Drops statements after a jump or `return` that no label makes
    /// reachable.
    fn remove_dead(&self, seq: &mut Vec<Ast>) {
        for item in seq.iter_mut() {
            for child in children(item) {
                self.remove_dead(child);
            }
        }
        let mut dead = false;
        seq.retain_mut(|item| {
            // A `goto` from elsewhere may land on a label, even one nested
            // inside this item.
            if has_label(item) {
                dead = false;
            }
            let keep = !dead;
            if self.terminates(item) {
                dead = true;
            }
            keep
        });
    }

    fn count_refs(&mut self, seq: &[Ast]) {
        for item in seq {
            match item {
                Ast::Stmt(n) => {
                    if matches!(
                        self.nodes[*n].stmt,
                        Stmt::Goto { .. } | Stmt::GotoUnless { .. }
                    ) {
                        *self.refs.entry(&self.nodes[*n].target_label).or_default() += 1;
                    }
                }
                Ast::Label(_) => {}
                Ast::If { then, else_, .. } => {
                    self.count_refs(then);
                    self.count_refs(else_);
                }
                Ast::While { body, .. } | Ast::Repeat { body, .. } | Ast::For { body, .. } => {
                    self.count_refs(body)
                }
            }
        }
    }

    fn sole_ref(&self, label: &str) -> bool {
        self.refs.get(label) == Some(&1)
    }

    /// `L: if c then B; goto L; endif;` → `while (c) B endwhile;` and
    /// `L: B; if !c goto L;` → `repeat B until (c);`, when nothing else jumps
    /// to `L`.
    fn recover_loops(&mut self, seq: &mut Vec<Ast>) {
        for item in seq.iter_mut() {
            for child in children(item) {
                self.recover_loops(child);
            }
        }
        let mut k = 0;
        while k < seq.len() {
            let Ast::Label(n) = seq[k] else {
                k += 1;
                continue;
            };
            let label = self.label(n);
            if !self.sole_ref(label) {
                k += 1;
                continue;
            }
            let is_while = matches!(seq.get(k + 1), Some(Ast::If { then, .. })
                if self.trailing_goto(then) == Some(label));
            if is_while {
                let Ast::If {
                    node,
                    mut then,
                    else_,
                } = seq.remove(k + 1)
                else {
                    unreachable!()
                };
                then.pop();
                self.refs.insert(label, 0);
                seq.insert(k + 1, Ast::While { node, body: then });
                // The `else` runs once the condition fails: after the loop.
                for (i, item) in else_.into_iter().enumerate() {
                    seq.insert(k + 2 + i, item);
                }
            } else if let Some(m) = (k + 1..seq.len()).find(|&m| {
                matches!(seq[m], Ast::Stmt(g) if matches!(self.nodes[g].stmt, Stmt::GotoUnless { .. })
                    && self.nodes[g].target_label == label)
            }) {
                let Ast::Stmt(node) = seq.remove(m) else {
                    unreachable!()
                };
                let mut body: Vec<Ast> = seq.drain(k + 1..m).collect();
                self.recover_loops(&mut body);
                self.refs.insert(label, 0);
                seq.insert(k + 1, Ast::Repeat { body, node });
            }
            k += 1;
        }
    }

    fn remove_unused_labels(&self, seq: &mut Vec<Ast>) {
        seq.retain(|item| match item {
            Ast::Label(n) => self.refs.get(self.label(*n)).is_some_and(|&c| c > 0),
            _ => true,
        });
        for item in seq.iter_mut() {
            for child in children(item) {
                self.remove_unused_labels(child);
            }
        }
    }

    /// `i = a; while (i <= b) B; i = i + n; endwhile;` → `for i = a to b`.
    fn recover_for(&self, seq: &mut Vec<Ast>) {
        for item in seq.iter_mut() {
            for child in children(item) {
                self.recover_for(child);
            }
        }
        let mut k = 1;
        while k < seq.len() {
            if let (Ast::Stmt(init), Ast::While { node, body }) = (&seq[k - 1], &seq[k])
                && let Some(Ast::Stmt(step)) = body.last()
                && self.is_for(*init, *node, *step)
            {
                let (init, node, step) = (*init, *node, *step);
                let Ast::While { mut body, .. } = seq.remove(k) else {
                    unreachable!()
                };
                body.pop();
                seq[k - 1] = Ast::For {
                    init,
                    node,
                    step,
                    body,
                };
                continue;
            }
            k += 1;
        }
    }

    fn is_for(&self, init: usize, test: usize, step: usize) -> bool {
        let Stmt::Assign(Expr::Var(v), _) = &self.nodes[init].stmt else {
            return false;
        };
        let counter = |e: &Expr| matches!(e, Expr::Var(x) if x == v);
        let tests_counter = matches!(&self.nodes[test].stmt,
            Stmt::If { cond: Expr::Binary(BinOp::Lte, l, _), .. } if counter(l));
        let steps_counter = matches!(&self.nodes[step].stmt,
            Stmt::Assign(lhs, Expr::Binary(BinOp::Add, l, r))
                if counter(lhs) && counter(l) && matches!(**r, Expr::Number(n) if n > 0));
        tests_counter && steps_counter
    }
}

/// Renders statements as InstallScript.
struct Printer<'a> {
    nodes: &'a [Node],
    syms: &'a Symbols,
    /// The variable that receives call results; a call stored there alone
    /// is printed as a call statement.
    last_result: Option<VarId>,
    /// New names of the labels that are left.
    labels: HashMap<&'a str, String>,
}

impl<'a> Printer<'a> {
    fn expr(&self, e: &Expr) -> String {
        e.render(self.syms)
    }

    fn cond(&self, n: usize) -> String {
        match &self.nodes[n].stmt {
            Stmt::If { cond, .. } => self.expr(cond),
            Stmt::GotoUnless { cond, .. } => self.expr(cond),
            _ => unreachable!(),
        }
    }

    fn label<'b>(&'b self, name: &'b str) -> &'b str {
        self.labels.get(name).map_or(name, |s| s)
    }

    fn stmt(&self, n: usize) -> String {
        let node = &self.nodes[n];
        match &node.stmt {
            Stmt::Assign(Expr::Var(v), call @ Expr::Call(..)) if Some(*v) == self.last_result => {
                format!("{};", self.expr(call))
            }
            Stmt::Assign(l, r) => format!("{} = {};", self.expr(l), self.expr(r)),
            Stmt::Goto { .. } => format!("goto {};", self.label(&node.target_label)),
            Stmt::GotoUnless { cond, .. } => format!(
                "if ({}) goto {};",
                negate(cond).render(self.syms),
                self.label(&node.target_label)
            ),
            Stmt::Return(Some(e)) => format!("return {};", self.expr(e)),
            Stmt::Return(None) => "return;".into(),
            Stmt::Exit => "exit;".into(),
            Stmt::Abort => "abort;".into(),
            Stmt::Nop | Stmt::If { .. } => String::new(),
        }
    }

    fn write(&self, out: &mut String, seq: &[Ast], indent: usize) {
        let tabs = "\t".repeat(indent);
        for item in seq {
            match item {
                Ast::Label(n) => {
                    let _ = writeln!(out, "{tabs}{}:", self.label(&self.nodes[*n].label));
                }
                Ast::Stmt(n) => {
                    let _ = writeln!(out, "{tabs}{}", wrap(&self.stmt(*n), &tabs));
                }
                Ast::If { node, then, else_ } => self.write_if(out, *node, then, else_, indent),
                Ast::While { node, body } => {
                    let _ = writeln!(
                        out,
                        "{tabs}{}",
                        wrap(&format!("while ({})", self.cond(*node)), &tabs)
                    );
                    self.write(out, body, indent + 1);
                    let _ = writeln!(out, "{tabs}endwhile;");
                }
                Ast::Repeat { body, node } => {
                    let _ = writeln!(out, "{tabs}repeat");
                    self.write(out, body, indent + 1);
                    let _ = writeln!(
                        out,
                        "{tabs}{}",
                        wrap(&format!("until ({});", self.cond(*node)), &tabs)
                    );
                }
                Ast::For {
                    init,
                    node,
                    step,
                    body,
                } => {
                    let (
                        Stmt::Assign(var, from),
                        Stmt::If {
                            cond: Expr::Binary(_, _, to),
                            ..
                        },
                        Stmt::Assign(_, Expr::Binary(_, _, by)),
                    ) = (
                        &self.nodes[*init].stmt,
                        &self.nodes[*node].stmt,
                        &self.nodes[*step].stmt,
                    )
                    else {
                        unreachable!()
                    };
                    let step = match **by {
                        Expr::Number(1) => String::new(),
                        ref n => format!(" step {}", self.expr(n)),
                    };
                    let _ = writeln!(
                        out,
                        "{tabs}for {} = {} to {}{step}",
                        self.expr(var),
                        self.expr(from),
                        self.expr(to)
                    );
                    self.write(out, body, indent + 1);
                    let _ = writeln!(out, "{tabs}endfor;");
                }
            }
        }
    }

    fn write_if(&self, out: &mut String, node: usize, then: &[Ast], else_: &[Ast], indent: usize) {
        let tabs = "\t".repeat(indent);
        // `if (c) then else B endif;` reads better as `if (!c) then B endif;`.
        if then.is_empty() && !else_.is_empty() {
            let Stmt::If { cond, .. } = &self.nodes[node].stmt else {
                unreachable!()
            };
            let cond = negate(cond).render(self.syms);
            let _ = writeln!(out, "{tabs}{}", wrap(&format!("if ({cond}) then"), &tabs));
            self.write(out, else_, indent + 1);
            let _ = writeln!(out, "{tabs}endif;");
            return;
        }
        // `if (c) goto L;`, the short form scripts use for dialog sequences.
        if let ([Ast::Stmt(g)], []) = (then, else_)
            && matches!(self.nodes[*g].stmt, Stmt::Goto { .. })
        {
            let _ = writeln!(
                out,
                "{tabs}{}",
                wrap(
                    &format!("if ({}) {}", self.cond(node), self.stmt(*g)),
                    &tabs
                )
            );
            return;
        }
        let _ = writeln!(
            out,
            "{tabs}{}",
            wrap(&format!("if ({}) then", self.cond(node)), &tabs)
        );
        self.write(out, then, indent + 1);
        let mut else_ = else_;
        // An `else` holding nothing but another `if` is an `elseif`.
        while let [
            Ast::If {
                node,
                then,
                else_: rest,
            },
        ] = else_
        {
            let _ = writeln!(
                out,
                "{tabs}{}",
                wrap(&format!("elseif ({}) then", self.cond(*node)), &tabs)
            );
            self.write(out, then, indent + 1);
            else_ = rest;
        }
        if !else_.is_empty() {
            let _ = writeln!(out, "{tabs}else");
            self.write(out, else_, indent + 1);
        }
        let _ = writeln!(out, "{tabs}endif;");
    }
}

/// Longest line printed before breaking it. The compiler rejects long lines
/// (somewhere above 1000 characters); folded string concatenations get there.
const MAX_LINE: usize = 200;

/// Breaks `text` after `, ` or before ` + ` outside string literals so no
/// line gets much longer than [`MAX_LINE`]; continuation lines are indented
/// one level deeper than `tabs`.
fn wrap(text: &str, tabs: &str) -> String {
    if text.len() <= MAX_LINE {
        return text.to_string();
    }
    let mut out = String::with_capacity(text.len() + 64);
    let mut line_len = tabs.len();
    let mut in_string = false;
    let mut escaped = false;
    let bytes = text.as_bytes();
    let mut i = 0;
    while i < bytes.len() {
        let c = bytes[i];
        if in_string {
            if escaped {
                escaped = false;
            } else if c == b'\\' {
                escaped = true;
            } else if c == b'"' {
                in_string = false;
            }
        } else if c == b'"' {
            in_string = true;
        } else if line_len > MAX_LINE {
            if text[i..].starts_with(", ") {
                out.push_str(",\n");
                out.push_str(tabs);
                out.push('\t');
                line_len = tabs.len() + 1;
                i += 2;
                continue;
            }
            if text[i..].starts_with(" + ") {
                out.push('\n');
                out.push_str(tabs);
                out.push_str("\t+ ");
                line_len = tabs.len() + 3;
                i += 3;
                continue;
            }
        }
        // Copy one whole character.
        let len = text[i..].chars().next().map_or(1, char::len_utf8);
        out.push_str(&text[i..i + len]);
        line_len += 1;
        i += len;
    }
    out
}

/// The condition under which a test of `e` fails.
fn negate(e: &Expr) -> Expr {
    let inverse = |op| match op {
        BinOp::Eq => Some(BinOp::Neq),
        BinOp::Neq => Some(BinOp::Eq),
        BinOp::Lt => Some(BinOp::Gte),
        BinOp::Gte => Some(BinOp::Lt),
        BinOp::Gt => Some(BinOp::Lte),
        BinOp::Lte => Some(BinOp::Gt),
        _ => None,
    };
    match e {
        Expr::Binary(op, l, r) => match inverse(*op) {
            Some(inv) => Expr::Binary(inv, l.clone(), r.clone()),
            None => Expr::Unary('!', Box::new(e.clone())),
        },
        Expr::Unary('!', inner) => (**inner).clone(),
        _ => Expr::Unary('!', Box::new(e.clone())),
    }
}

/// Structures a function body and prints it, one tab per nesting level.
pub fn write_body(
    out: &mut String,
    cfg: &Cfg,
    nodes: &[Node],
    syms: &Symbols,
    last_result: Option<VarId>,
) {
    let mut seen = vec![false; cfg.blocks.len()];
    let mut seq = Vec::new();
    for bb in 0..cfg.blocks.len() {
        seq.extend(from_block(cfg, bb, nodes, &mut seen));
    }

    let mut s = Structurer {
        nodes,
        refs: HashMap::new(),
    };
    s.recover_else(&mut seq, None);
    s.remove_dead(&mut seq);
    s.remove_redundant_gotos(&mut seq, None);
    s.count_refs(&seq);
    s.recover_loops(&mut seq);
    s.remove_unused_labels(&mut seq);
    s.recover_for(&mut seq);

    // Number the labels that are left in order of appearance.
    let mut labels = HashMap::new();
    let mut order = Vec::new();
    collect_labels(&seq, nodes, &mut order);
    for (i, name) in order.into_iter().enumerate() {
        labels.insert(name, format!("label_{}", i + 1));
    }

    let p = Printer {
        nodes,
        syms,
        last_result,
        labels,
    };
    p.write(out, &seq, 1);
}

fn collect_labels<'a>(seq: &[Ast], nodes: &'a [Node], out: &mut Vec<&'a str>) {
    for item in seq {
        match item {
            Ast::Label(n) => out.push(&nodes[*n].label),
            Ast::If { then, else_, .. } => {
                collect_labels(then, nodes, out);
                collect_labels(else_, nodes, out);
            }
            Ast::While { body, .. } | Ast::Repeat { body, .. } | Ast::For { body, .. } => {
                collect_labels(body, nodes, out)
            }
            Ast::Stmt(_) => {}
        }
    }
}
