use std::fmt;

/// Declared type of a prototype argument, return value or struct member.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct ScriptType(pub u8);

impl ScriptType {
    pub const STRING: ScriptType = ScriptType(0);
    pub const STRUCT: ScriptType = ScriptType(9);
    pub const WSTRING: ScriptType = ScriptType(12);
}

impl fmt::Display for ScriptType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let name = match self.0 {
            0 => "STRING",
            1 => "CHAR",
            2 => "SHORT",
            3 => "INT",
            4 => "LONG",
            5 => "POINTER",
            6 => "BOOL",
            7 => "HWND",
            8 => "VOID",
            9 => "STRUCT",
            10 => "OBJECT",
            11 => "BINARY",
            12 => "WSTRING",
            13 => "WPOINTER",
            n => return write!(f, "UNKNOWN_TYPE_{n}"),
        };
        f.write_str(name)
    }
}

/// How an argument is passed internally (by value or by reference, and which
/// variable bank it lives in).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum ConcreteType {
    StringPointer,
    NumberPointer,
    ObjectPointer,
    String,
    Number,
    Object,
    Invalid(u8),
}

impl ConcreteType {
    pub fn from_u8(v: u8) -> Self {
        match v {
            2 => ConcreteType::StringPointer,
            3 => ConcreteType::NumberPointer,
            4 => ConcreteType::ObjectPointer,
            5 => ConcreteType::String,
            6 => ConcreteType::Number,
            7 => ConcreteType::Object,
            n => ConcreteType::Invalid(n),
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub struct ArgumentTypeInfo {
    pub script_type: ScriptType,
    pub internal_type: ConcreteType,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum VarType {
    Number,
    String,
    Variant,
}
