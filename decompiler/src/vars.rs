//! Variables and the scoped symbol tables used to resolve operand addresses.
//!
//! Globals have addresses >= 0; locals and parameters count down from -101.

use std::fmt;

use crate::Result;
use crate::types::VarType;

pub type VarId = usize;
pub type TableId = usize;

#[derive(Clone, Debug)]
pub enum Decl {
    Number,
    String {
        size: Option<u16>,
    },
    Variant,
    Object,
    /// `count` 0 is an array sized at run time.
    Array {
        element: &'static str,
        count: u16,
    },
    Struct {
        type_name: String,
    },
}

#[derive(Clone, Debug)]
pub struct Variable {
    pub address: i32,
    pub name: String,
    pub ty: VarType,
    pub decl: Decl,
    /// Named after debug symbols, so declared in the source.
    pub named: bool,
    /// Declared `external`: defined by another script object.
    pub external: bool,
}

pub fn is_global_address(address: i32) -> bool {
    address > -100
}

impl Variable {
    pub fn new(address: i32, decl: Decl) -> Self {
        let ty = match decl {
            Decl::Number => VarType::Number,
            Decl::String { .. } => VarType::String,
            _ => VarType::Variant,
        };
        let scope = if is_global_address(address) {
            "Gbl"
        } else {
            "Lcl"
        };
        let kind = match ty {
            VarType::String => "Str",
            VarType::Number => "Num",
            VarType::Variant => "Obj",
        };
        let index = if is_global_address(address) {
            address
        } else {
            -(address + 100)
        };
        Variable {
            address,
            name: format!("{scope}Var{kind}{index}"),
            ty,
            decl,
            named: false,
            external: false,
        }
    }

    pub fn is_global(&self) -> bool {
        is_global_address(self.address)
    }
}

impl fmt::Display for Variable {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match &self.decl {
            Decl::Number => write!(f, "INT {}", self.name),
            Decl::String { size: Some(n) } => write!(f, "STRING {}[{n}]", self.name),
            Decl::String { size: None } => write!(f, "STRING {}", self.name),
            Decl::Variant => write!(f, "VARIANT {}", self.name),
            Decl::Object => write!(f, "OBJECT {}", self.name),
            Decl::Array { element, count: 0 } => write!(f, "{element} {}()", self.name),
            Decl::Array { element, count } => write!(f, "{element} {}({count})", self.name),
            Decl::Struct { type_name } => write!(f, "{type_name} {}", self.name),
        }
    }
}

struct Table {
    vars: Vec<VarId>,
    parent: Option<TableId>,
}

/// All variables of a script plus a tree of scopes: each function's locals
/// chain to its parameters, which chain to the globals.
#[derive(Default)]
pub struct Symbols {
    pub vars: Vec<Variable>,
    tables: Vec<Table>,
}

impl Symbols {
    pub fn new_table(&mut self, parent: Option<TableId>) -> TableId {
        self.tables.push(Table {
            vars: Vec::new(),
            parent,
        });
        self.tables.len() - 1
    }

    pub fn add(&mut self, table: TableId, var: Variable) -> VarId {
        self.vars.push(var);
        let id = self.vars.len() - 1;
        self.tables[table].vars.push(id);
        id
    }

    pub fn table_vars(&self, table: TableId) -> &[VarId] {
        &self.tables[table].vars
    }

    fn find(&self, table: TableId, pred: impl Fn(&Variable) -> bool) -> Option<VarId> {
        let mut t = Some(table);
        while let Some(id) = t {
            let table = &self.tables[id];
            if let Some(&v) = table.vars.iter().find(|&&v| pred(&self.vars[v])) {
                return Some(v);
            }
            t = table.parent;
        }
        None
    }

    pub fn by_address(
        &self,
        table: TableId,
        address: i32,
        ty: VarType,
        global: bool,
    ) -> Result<VarId> {
        self.find(table, |v| {
            v.address == address && v.is_global() == global && v.ty == ty
        })
        .ok_or_else(|| format!("Couldn't find a variable with address {address}"))
    }

    pub fn by_name(&self, table: TableId, name: &str, ty: VarType, global: bool) -> Result<VarId> {
        self.find(table, |v| {
            v.name == name && v.is_global() == global && v.ty == ty
        })
        .ok_or_else(|| format!("Couldn't find a variable named {name}"))
    }
}
