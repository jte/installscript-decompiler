//! Builds a block graph from the statement list, nesting `if` bodies, and
//! prints it back as structured InstallScript.

use std::fmt::Write;

use crate::expr::{Node, Stmt};
use crate::vars::Symbols;

#[derive(Clone, Copy, Debug)]
pub enum IrStmt {
    /// A plain statement (or a bare label), by node index.
    Plain(usize),
    /// The `if` at `node`: run `then_bb` when the condition holds, then `cont_bb`.
    Branch {
        node: usize,
        then_bb: usize,
        cont_bb: usize,
    },
}

#[derive(Default, Debug)]
pub struct Block {
    pub stmts: Vec<IrStmt>,
    pub succs: Vec<usize>,
}

pub struct Cfg {
    pub blocks: Vec<Block>,
}

enum Visit {
    Stopped,
    AlreadyVisited,
    Done,
}

struct Gen<'a> {
    nodes: &'a [Node],
    blocks: Vec<Block>,
    current: Vec<usize>,
    stop: Vec<Option<usize>>,
    visited: Vec<bool>,
    active: Vec<bool>,
}

impl Gen<'_> {
    fn new_block(&mut self) -> usize {
        self.blocks.push(Block::default());
        self.blocks.len() - 1
    }

    fn link(&mut self, from: usize, to: usize) {
        if !self.blocks[from].succs.contains(&to) {
            self.blocks[from].succs.push(to);
        }
    }

    fn cur(&self) -> usize {
        *self.current.last().unwrap()
    }

    fn next(&self, i: usize) -> Option<usize> {
        (i + 1 < self.nodes.len()).then_some(i + 1)
    }

    fn generate(&mut self, i: usize) -> Visit {
        if self.stop.contains(&Some(i)) {
            return Visit::Stopped;
        }
        // `active` guards against re-entering an `if` from its own body,
        // which a backwards `.ins` branch target can cause.
        if self.visited[i] || self.active[i] {
            return Visit::AlreadyVisited;
        }
        self.active[i] = true;
        self.accept(i);
        self.active[i] = false;
        self.visited[i] = true;
        Visit::Done
    }

    fn accept(&mut self, i: usize) {
        let cur = self.cur();
        match &self.nodes[i].stmt {
            Stmt::If { then, else_, .. } => self.visit_if(i, *then, *else_),
            Stmt::Nop if self.nodes[i].label.is_empty() => {}
            _ => self.blocks[cur].stmts.push(IrStmt::Plain(i)),
        }
    }

    fn visit_if(&mut self, node: usize, then: Option<usize>, else_: Option<usize>) {
        let then_bb = self.new_block();
        let cont_bb = self.new_block();
        let cur = self.cur();
        self.link(cur, then_bb);
        self.link(cur, cont_bb);
        self.blocks[cur].stmts.push(IrStmt::Branch {
            node,
            then_bb,
            cont_bb,
        });

        self.current.push(then_bb);
        self.stop.push(else_);
        let mut t = then;
        while let Some(i) = t {
            if Some(i) == else_ {
                self.stop.pop();
                break;
            }
            if let Visit::Stopped = self.generate(i) {
                break;
            }
            t = self.next(i);
        }
        let cur = self.cur();
        self.link(cur, cont_bb);
        self.current.pop();
        self.current.push(cont_bb);

        let mut n = else_;
        while let Some(i) = n {
            if let Visit::Stopped = self.generate(i) {
                break;
            }
            n = self.next(i);
        }
    }
}

pub fn build(nodes: &[Node]) -> Cfg {
    let mut g = Gen {
        nodes,
        blocks: vec![Block::default()],
        current: vec![0],
        stop: Vec::new(),
        visited: vec![false; nodes.len()],
        active: vec![false; nodes.len()],
    };
    for i in 0..nodes.len() {
        g.generate(i);
    }
    Cfg { blocks: g.blocks }
}

impl Cfg {
    /// Prints the function body, one tab of indentation per nesting level.
    pub fn write_body(&self, out: &mut String, nodes: &[Node], syms: &Symbols) {
        let mut visited = vec![false; self.blocks.len()];
        for bb in 0..self.blocks.len() {
            self.write_block(out, bb, &mut visited, 1, nodes, syms);
        }
    }

    fn write_block(
        &self,
        out: &mut String,
        bb: usize,
        visited: &mut Vec<bool>,
        indent: usize,
        nodes: &[Node],
        syms: &Symbols,
    ) {
        if visited[bb] {
            return;
        }
        let tabs = "\t".repeat(indent);
        for s in &self.blocks[bb].stmts {
            let (IrStmt::Plain(i) | IrStmt::Branch { node: i, .. }) = *s;
            let node = &nodes[i];
            if !node.label.is_empty() {
                let _ = writeln!(out, "{tabs}{}:", node.label);
            }
            match *s {
                IrStmt::Branch {
                    then_bb, cont_bb, ..
                } => {
                    let Stmt::If { cond, .. } = &node.stmt else {
                        unreachable!()
                    };
                    let _ = writeln!(out, "{tabs}if {} then", cond.render(syms));
                    self.write_block(out, then_bb, visited, indent + 1, nodes, syms);
                    visited[then_bb] = true;
                    let _ = writeln!(out, "{tabs}endif;");
                    self.write_block(out, cont_bb, visited, indent, nodes, syms);
                }
                IrStmt::Plain(_) => {
                    if let Stmt::GotoUnless { cond, .. } = &node.stmt {
                        let _ = writeln!(out, "{tabs}if !{} then", cond.render(syms));
                        let _ = writeln!(out, "{tabs}\tgoto {};", node.target_label);
                        let _ = writeln!(out, "{tabs}endif;");
                        continue;
                    }
                    let text = match &node.stmt {
                        Stmt::Assign(l, r) => format!("{} = {}", l.render(syms), r.render(syms)),
                        Stmt::Goto { .. } => format!("goto {}", node.target_label),
                        Stmt::Return(Some(e)) => format!("return {}", e.render(syms)),
                        Stmt::Return(None) => "return".into(),
                        Stmt::Exit => "exit".into(),
                        Stmt::Abort => "abort".into(),
                        Stmt::Nop | Stmt::If { .. } | Stmt::GotoUnless { .. } => continue,
                    };
                    let _ = writeln!(out, "{tabs}{text};");
                }
            }
        }
        visited[bb] = true;
    }

    /// Graphviz DOT of the block graph.
    pub fn write_dot(&self, out: &mut String, name: &str) {
        let _ = writeln!(out, "digraph \"{}\" {{", name.replace('"', "\\\""));
        for (i, bb) in self.blocks.iter().enumerate() {
            let _ = writeln!(
                out,
                "\tBB_{i}[shape = record, color = \"#b70d28ff\", style = filled, fillcolor = \"#b70d2870\", label = \"{{|{{<s0>T|<s1>F}}}}\"]; "
            );
            for s in &bb.succs {
                let _ = writeln!(out, "\t\"BB_{i}\" -> \"BB_{s}\"");
            }
        }
        out.push_str("}\n");
    }
}
