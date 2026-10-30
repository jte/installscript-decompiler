//! Decompiler for compiled InstallShield InstallScript.
//!
//! Supported inputs: `setup.inx` (InstallShield 6+, encrypted `aLuZ`), `.obs`
//! script objects, `.obl` script libraries and InstallShield 5 `setup.ins`.

pub mod debug;
pub mod decompile;
pub mod expr;
pub mod format;
pub mod ir;
pub mod lower;
pub mod newis;
pub mod oldis;
pub mod script;
pub mod stream;
pub mod types;
pub mod vars;

use format::HeaderKind;
use script::Script;

pub type Result<T> = std::result::Result<T, String>;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Mode {
    /// Reconstructed InstallScript source (`.rul`).
    Decompile,
    /// Action listing per function.
    Disassemble,
    /// Graphviz DOT of every function's block graph.
    Cfg,
}

/// Parses a single (already decrypted) script.
pub fn parse_script(data: &[u8], kind: HeaderKind) -> Result<Script> {
    match kind {
        HeaderKind::Ins => oldis::parse(data),
        HeaderKind::Obs | HeaderKind::ALuZ => newis::parse(data, kind),
        HeaderKind::Dbg => Err("This is an InstallScript debug symbols file (.dbg), not a \
script. Decompile the setup.inx that was compiled with it instead."
            .into()),
        HeaderKind::Obl => Err("Script libraries can't be nested.".into()),
        HeaderKind::Unrecognized => Err(UNRECOGNIZED.into()),
    }
}

const UNRECOGNIZED: &str = "The input file is unrecognized. Expected a compiled InstallScript file: \
setup.inx, an .obs script object, an .obl script library or an InstallShield 5 setup.ins.";

/// Parses a script and attaches separately supplied debug symbols, which
/// take precedence over any embedded in the script.
fn parse_with_symbols(
    data: &[u8],
    kind: HeaderKind,
    symbols: Option<&debug::DebugInfo>,
) -> Result<Script> {
    let mut script = parse_script(data, kind)?;
    if let Some(info) = symbols
        && !debug::attach(&mut script, info.clone())
    {
        script.debug_note =
            Some("The supplied debug symbols belong to a different build and were ignored.".into());
    }
    Ok(script)
}

fn process(
    data: &[u8],
    kind: HeaderKind,
    symbols: Option<&debug::DebugInfo>,
    mode: Mode,
) -> Result<String> {
    let script = parse_with_symbols(data, kind, symbols)?;
    Ok(match mode {
        Mode::Disassemble => script.listing(),
        Mode::Decompile => decompile::decompile(&script)?.source,
        Mode::Cfg => decompile::decompile(&script)?.dot,
    })
}

fn parse_symbols(symbols: Option<&[u8]>) -> Result<Option<debug::DebugInfo>> {
    symbols.map(debug::parse_file).transpose()
}

/// Runs the whole pipeline on the contents of an input file.
pub fn run(data: &[u8], mode: Mode) -> Result<String> {
    run_with_symbols(data, None, mode)
}

/// Like [`run`], with the contents of the `.dbg` file the script was
/// compiled with, which restores the original names.
pub fn run_with_symbols(data: &[u8], symbols: Option<&[u8]>, mode: Mode) -> Result<String> {
    let symbols = parse_symbols(symbols)?;
    let mut data = data.to_vec();
    let kind = format::header_kind(&mut data);
    match kind {
        HeaderKind::Unrecognized => Err(UNRECOGNIZED.into()),
        HeaderKind::Obl => {
            let mut out = String::new();
            for mut member in format::parse_library(&data)? {
                let kind = format::header_kind(&mut member.data);
                out.push_str(&format!("// ===== {} =====\n", member.name));
                match process(&member.data, kind, None, mode) {
                    Ok(text) => out.push_str(&text),
                    Err(e) => out.push_str(&format!("// could not process {}: {e}\n", member.name)),
                }
                out.push('\n');
            }
            Ok(out)
        }
        _ => process(&data, kind, symbols.as_ref(), mode),
    }
}

/// Both the source and the DOT graph from one decompilation.
pub fn decompile_with_cfg(data: &[u8], symbols: Option<&[u8]>) -> Result<decompile::Decompiled> {
    let mut data = data.to_vec();
    let kind = format::header_kind(&mut data);
    if kind == HeaderKind::Obl {
        return Ok(decompile::Decompiled {
            source: run(&data, Mode::Decompile)?,
            dot: run(&data, Mode::Cfg)?,
        });
    }
    let symbols = parse_symbols(symbols)?;
    decompile::decompile(&parse_with_symbols(&data, kind, symbols.as_ref())?)
}

/// The path of the `.dbg` file a compiled setup script was built with, as
/// recorded in the script (a path on the build machine).
pub fn debug_symbols_path(data: &[u8]) -> Option<String> {
    let mut data = data.to_vec();
    let kind = format::header_kind(&mut data);
    if kind != HeaderKind::ALuZ {
        return None;
    }
    match parse_script(&data, kind).ok()?.debug_section {
        debug::Section::External(path) => Some(path),
        _ => None,
    }
}
