//! Folds the values the compiler parks in temporaries back into the
//! expressions that use them: `LAST_RESULT = F(); x = LAST_RESULT;` becomes
//! `x = F();`, and `t = a == b; if t then` becomes `if (a == b) then`.
//!
//! Statements are never removed, only turned into `Nop`, so the indices that
//! `if` and `goto` hold stay valid.

use std::collections::HashMap;

use crate::expr::{Expr, Node, Stmt, Uses};
use crate::vars::VarId;

/// Statements that something other than the previous statement can reach.
fn jump_targets(nodes: &[Node]) -> Vec<bool> {
    let mut targets: Vec<bool> = nodes.iter().map(|n| !n.label.is_empty()).collect();
    for n in nodes {
        let target = match n.stmt {
            Stmt::If { else_, .. } => else_,
            Stmt::Goto { target } => target,
            Stmt::GotoUnless { target, .. } => Some(target),
            _ => None,
        };
        if let Some(t) = target.filter(|&t| t < targets.len()) {
            targets[t] = true;
        }
    }
    targets
}

const ONE_VALUE: Uses = Uses {
    total: 1,
    values: 1,
};

/// Statements control can go to after statement `k`.
fn successors(nodes: &[Node], k: usize) -> Vec<usize> {
    let next = (k + 1 < nodes.len()).then_some(k + 1);
    let mut out: Vec<usize> = match nodes[k].stmt {
        Stmt::Goto { target } => target.into_iter().collect(),
        Stmt::Return(_) | Stmt::Exit | Stmt::Abort => Vec::new(),
        Stmt::If { else_, .. } => next.into_iter().chain(else_).collect(),
        Stmt::GotoUnless { target, .. } => next.into_iter().chain([target]).collect(),
        _ => next.into_iter().collect(),
    };
    out.retain(|&s| s < nodes.len());
    out
}

/// For each statement, which of the tracked variables may still be read
/// after it runs. `tracked` maps a variable to its bit.
fn live_after(nodes: &[Node], tracked: &HashMap<VarId, usize>) -> Vec<Vec<u64>> {
    let words = tracked.len().div_ceil(64);
    let bits = |vars: &mut dyn Iterator<Item = VarId>| {
        let mut set = vec![0u64; words];
        for v in vars {
            if let Some(&b) = tracked.get(&v) {
                set[b / 64] |= 1 << (b % 64);
            }
        }
        set
    };
    let uses: Vec<Vec<u64>> = nodes
        .iter()
        .map(|n| bits(&mut read_vars(&n.stmt).into_iter()))
        .collect();
    let defs: Vec<Vec<u64>> = nodes
        .iter()
        .map(|n| bits(&mut n.stmt.assigned_var().into_iter()))
        .collect();
    let succs: Vec<Vec<usize>> = (0..nodes.len()).map(|k| successors(nodes, k)).collect();
    let mut live_in = vec![vec![0u64; words]; nodes.len()];
    let mut live_out = vec![vec![0u64; words]; nodes.len()];
    let mut changed = true;
    while changed {
        changed = false;
        for k in (0..nodes.len()).rev() {
            let mut out = vec![0u64; words];
            for &s in &succs[k] {
                for (o, i) in out.iter_mut().zip(&live_in[s]) {
                    *o |= i;
                }
            }
            let inn: Vec<u64> = (0..words)
                .map(|w| uses[k][w] | (out[w] & !defs[k][w]))
                .collect();
            if inn != live_in[k] || out != live_out[k] {
                live_in[k] = inn;
                live_out[k] = out;
                changed = true;
            }
        }
    }
    live_out
}

/// Folds temporaries into their single use.
///
/// `v = E` moves into the next statement when that statement can only be
/// reached from here (nothing jumps to it), mentions `v` exactly once where
/// a value goes, and after it `v` is either dead or assigned anew by it. The
/// value of `E` then has no other reader. Only variables `candidate` allows
/// are moved. `last_result` gets the same treatment without the liveness
/// test: recompiling `x = F()` stores the call's result in it all the same.
pub fn fold_temporaries(
    nodes: &mut [Node],
    candidate: impl Fn(VarId) -> bool,
    last_result: Option<VarId>,
) {
    let targets = jump_targets(nodes);
    let skippable = |nodes: &[Node], k: usize| matches!(nodes[k].stmt, Stmt::Nop) && !targets[k];
    let next = |nodes: &[Node], i: usize| (i + 1..nodes.len()).find(|&k| !skippable(nodes, k));

    let mut tracked: HashMap<VarId, usize> = HashMap::new();
    for v in mentioned_vars(nodes) {
        if Some(v) != last_result && candidate(v) {
            let bit = tracked.len();
            tracked.insert(v, bit);
        }
    }

    loop {
        // Recomputed every round, as folding changes what is read where.
        let live = live_after(nodes, &tracked);
        let is_live = |k: usize, v: VarId| {
            tracked
                .get(&v)
                .is_some_and(|&b| live[k][b / 64] & (1 << (b % 64)) != 0)
        };

        // A fold changes what `live` says about the statements it touched;
        // anything overlapping waits for the next round.
        let mut touched = vec![false; nodes.len()];
        let mut changed = false;
        for i in 0..nodes.len() {
            if touched[i] {
                continue;
            }
            let Stmt::Assign(Expr::Var(v), value) = &nodes[i].stmt else {
                continue;
            };
            let v = *v;
            let lr = Some(v) == last_result;
            if lr && matches!(value, Expr::Var(x) if *x == v) || !lr && !tracked.contains_key(&v) {
                continue;
            }
            let Some(j) = next(nodes, i) else { continue };
            if touched[j] || targets[j] || nodes[j].stmt.uses(v) != ONE_VALUE {
                continue;
            }
            if !lr && is_live(j, v) && nodes[j].stmt.assigned_var() != Some(v) {
                continue;
            }
            touched[i] = true;
            touched[j] = true;
            let Stmt::Assign(_, value) = std::mem::replace(&mut nodes[i].stmt, Stmt::Nop) else {
                unreachable!()
            };
            nodes[j].stmt.substitute(v, &value);
            changed = true;
        }
        if !changed {
            break;
        }
    }
}

/// Every variable the statements mention.
pub fn mentioned_vars(nodes: &[Node]) -> Vec<VarId> {
    let mut out: Vec<VarId> = nodes
        .iter()
        .flat_map(|n| read_vars(&n.stmt).into_iter().chain(n.stmt.assigned_var()))
        .collect();
    out.sort_unstable();
    out.dedup();
    out
}

/// Variables a statement mentions other than as the variable it assigns.
fn read_vars(stmt: &Stmt) -> Vec<VarId> {
    fn collect(e: &Expr, out: &mut Vec<VarId>) {
        match e {
            Expr::Var(v) => out.push(*v),
            Expr::Number(_) | Expr::Str(_) | Expr::Function(_) => {}
            Expr::Binary(_, l, r) | Expr::ArrayAccess(l, r) => {
                collect(l, out);
                collect(r, out);
            }
            Expr::Unary(_, e)
            | Expr::AddressOf(e)
            | Expr::SetRef(e)
            | Expr::Ref(e)
            | Expr::StringConstant(e) => collect(e, out),
            Expr::Call(_, args) => args.iter().for_each(|a| collect(a, out)),
            Expr::PropPut(b, acc) | Expr::PropGet(b, acc) => {
                collect(b, out);
                acc.iter().for_each(|a| collect(a, out));
            }
        }
    }
    let mut out = Vec::new();
    match stmt {
        Stmt::Assign(lhs, rhs) => {
            if !matches!(lhs, Expr::Var(_)) {
                collect(lhs, &mut out);
            }
            collect(rhs, &mut out);
        }
        Stmt::If { cond, .. } | Stmt::GotoUnless { cond, .. } => collect(cond, &mut out),
        Stmt::Return(Some(e)) => collect(e, &mut out),
        _ => {}
    }
    out.sort_unstable();
    out.dedup();
    out
}
