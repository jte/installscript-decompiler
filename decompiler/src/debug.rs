//! Debug symbols written by the InstallShield compiler: a separate `.dbg`
//! file (`Compile.exe /G`) or the same data embedded in `setup.inx` (`/GI`).
//!
//! They hold the source file names, the names of globals, functions,
//! parameters and locals, and a line table mapping actions to source lines.
//! Functions linked in from `.obl` libraries have no entries.

use crate::Result;
use crate::script::{Function, Script};
use crate::stream::Stream;
use crate::types::VarType;

/// Size of the `.dbg` file header: the `kUtZ` magic and a copyright notice.
const DBG_HEADER_SIZE: usize = 0x74;

/// Variable names by type, in address order.
#[derive(Clone, Debug, Default)]
pub struct Names {
    pub numbers: Vec<String>,
    pub strings: Vec<String>,
    pub variants: Vec<String>,
}

impl Names {
    fn parse(s: &mut Stream) -> Result<Self> {
        let mut list =
            || -> Result<Vec<String>> { (0..s.u16()?).map(|_| s.ins_string()).collect() };
        Ok(Names {
            numbers: list()?,
            strings: list()?,
            variants: list()?,
        })
    }

    pub fn of(&self, ty: VarType) -> &[String] {
        match ty {
            VarType::Number => &self.numbers,
            VarType::String => &self.strings,
            VarType::Variant => &self.variants,
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub struct Line {
    /// Index of the action among all actions of the script, counted across
    /// basic blocks in file order (function prologs included).
    pub action: u32,
    /// Index into [`DebugInfo::files`].
    pub file: u16,
    pub line: u16,
}

#[derive(Clone, Debug)]
pub struct DebugFunction {
    pub name: String,
    /// Parameters first, then locals, as numbered by the compiler.
    pub vars: Names,
    pub lines: Vec<Line>,
}

impl DebugFunction {
    /// The source file of the function and the range of lines it covers.
    pub fn location<'a>(&self, info: &'a DebugInfo) -> Option<(&'a str, u16, u16)> {
        let file = self.lines.first()?.file;
        let lines = self.lines.iter().filter(|l| l.file == file).map(|l| l.line);
        let (min, max) = (lines.clone().min()?, lines.max()?);
        Some((info.file_name(file)?, min, max))
    }
}

#[derive(Clone, Debug)]
pub struct DebugInfo {
    /// Full paths of the source files on the build machine.
    pub files: Vec<String>,
    pub globals: Names,
    pub functions: Vec<DebugFunction>,
}

impl DebugInfo {
    /// File name without its directory.
    pub fn file_name(&self, file: u16) -> Option<&str> {
        let path = self.files.get(file as usize)?;
        Some(path.rsplit(['\\', '/']).next().unwrap_or(path))
    }
}

/// Parses a `.dbg` file.
pub fn parse_file(data: &[u8]) -> Result<DebugInfo> {
    if !data.starts_with(b"kUtZ") {
        return Err("Not an InstallScript debug symbols file (.dbg).".into());
    }
    let body = data
        .get(DBG_HEADER_SIZE..)
        .ok_or("Debug symbols file is truncated.")?;
    parse_body(body)
}

/// Parses the part shared by `.dbg` files and embedded debug sections.
fn parse_body(data: &[u8]) -> Result<DebugInfo> {
    let mut s = Stream::new(data);
    let version = s.u16()?;
    if version != 1 {
        return Err(format!("Unsupported debug symbols version {version}."));
    }
    let files = (0..s.u16()?)
        .map(|_| s.ins_string())
        .collect::<Result<_>>()?;
    let globals = Names::parse(&mut s)?;
    let count = s.u16()?;
    // Zero in every file seen so far; anything else means a layout this
    // parser doesn't know, so stop rather than misread the names.
    let unknown = s.u16()?;
    if unknown != 0 {
        return Err(format!("Unsupported debug symbols layout ({unknown:#x})."));
    }
    let functions = (0..count)
        .map(|_| {
            let name = s.ins_string()?;
            let vars = Names::parse(&mut s)?;
            let lines = (0..s.u16()?)
                .map(|_| {
                    Ok(Line {
                        action: s.u32()?,
                        file: s.u16()?,
                        line: s.u16()?,
                    })
                })
                .collect::<Result<_>>()?;
            Ok(DebugFunction { name, vars, lines })
        })
        .collect::<Result<_>>()?;
    Ok(DebugInfo {
        files,
        globals,
        functions,
    })
}

/// What a compiled setup script says about its debug symbols.
#[derive(Clone, Debug, Default)]
pub enum Section {
    #[default]
    None,
    /// Built with `/G`: the path of the `.dbg` file on the build machine.
    External(String),
    /// Built with `/GI`.
    Embedded(DebugInfo),
    /// Built with `/GI`, but the symbols could not be read.
    Unreadable(String),
}

/// Reads the debug section that the `setup.inx` header points to.
pub fn read_section(data: &[u8], offset: usize) -> Section {
    let mut s = Stream::at(data, offset);
    match s.u8() {
        Ok(0) => match s.ins_string() {
            Ok(path) if !path.is_empty() => Section::External(path),
            _ => Section::None,
        },
        Ok(1) => {
            let body = s.u32().and_then(|len| s.bytes(len as usize));
            match body.and_then(parse_body) {
                Ok(info) => Section::Embedded(info),
                Err(e) => Section::Unreadable(e),
            }
        }
        _ => Section::None,
    }
}

/// Whether `df` describes `f`: its first line entry is the function's first
/// statement (right after the prolog, if any), its last is the closing
/// `return`/`end` (one or two actions before the end), every entry lies
/// inside the function, there are no more names than variables, and an
/// exported function, whose name the script keeps, has the same name.
fn matches(df: &DebugFunction, f: &Function) -> bool {
    let (Some(first), Some(last)) = (df.lines.first(), df.lines.last()) else {
        return false;
    };
    let range = &f.actions;
    (range.start..=range.start + 1).contains(&first.action)
        && (range.end.saturating_sub(2)..range.end).contains(&last.action)
        && df.lines.iter().all(|l| range.contains(&l.action))
        && df.vars.numbers.len() <= f.decls.num_numbers as usize
        && df.vars.strings.len() <= f.decls.num_strings as usize
        && df.vars.variants.len() <= f.decls.objects.len()
        && (!f.proto.is_exported() || f.proto.name.eq_ignore_ascii_case(&df.name))
}

/// Matches the symbols' functions to the script's and names them. The
/// symbols only list the functions the linker kept, so every one must match;
/// otherwise they belong to a different build and the script is left as it
/// was (returns false).
pub fn attach(script: &mut Script, info: DebugInfo) -> bool {
    let mut pairs = Vec::new();
    for (i, df) in info.functions.iter().enumerate() {
        let taken = |f: usize| pairs.iter().any(|&(g, _)| g == f);
        match (0..script.fns.len()).find(|&f| !taken(f) && matches(df, &script.fns[f])) {
            Some(f) => pairs.push((f, i)),
            None => return false,
        }
    }
    for f in &mut script.fns {
        f.debug = None;
    }
    for (f, i) in pairs {
        script.fns[f].debug = Some(i);
        script.fns[f].proto.name = info.functions[i].name.clone();
    }
    script.debug = Some(info);
    true
}
