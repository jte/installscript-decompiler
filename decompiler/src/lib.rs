//! Decompiler for compiled InstallShield InstallScript.
//!
//! Supported inputs: `setup.inx` (InstallShield 6+, encrypted `aLuZ`), `.obs`
//! script objects, `.obl` script libraries and InstallShield 5 `setup.ins`.

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
        HeaderKind::Obs | HeaderKind::ALuZ | HeaderKind::KUtZ => newis::parse(data, kind),
        HeaderKind::Obl => Err("Script libraries can't be nested.".into()),
        HeaderKind::Unrecognized => Err(UNRECOGNIZED.into()),
    }
}

const UNRECOGNIZED: &str = "The input file is unrecognized. Expected a compiled InstallScript file: \
setup.inx, an .obs script object, an .obl script library or an InstallShield 5 setup.ins.";

fn process(data: &[u8], kind: HeaderKind, mode: Mode) -> Result<String> {
    let script = parse_script(data, kind)?;
    Ok(match mode {
        Mode::Disassemble => script.listing(),
        Mode::Decompile => decompile::decompile(&script)?.source,
        Mode::Cfg => decompile::decompile(&script)?.dot,
    })
}

/// Runs the whole pipeline on the contents of an input file.
pub fn run(data: &[u8], mode: Mode) -> Result<String> {
    let mut data = data.to_vec();
    let kind = format::header_kind(&mut data);
    match kind {
        HeaderKind::Unrecognized => Err(UNRECOGNIZED.into()),
        HeaderKind::Obl => {
            let mut out = String::new();
            for mut member in format::parse_library(&data)? {
                let kind = format::header_kind(&mut member.data);
                out.push_str(&format!("// ===== {} =====\n", member.name));
                match process(&member.data, kind, mode) {
                    Ok(text) => out.push_str(&text),
                    Err(e) => out.push_str(&format!("// could not process {}: {e}\n", member.name)),
                }
                out.push('\n');
            }
            Ok(out)
        }
        _ => process(&data, kind, mode),
    }
}

/// Both the source and the DOT graph from one decompilation.
pub fn decompile_with_cfg(data: &[u8]) -> Result<decompile::Decompiled> {
    let mut data = data.to_vec();
    let kind = format::header_kind(&mut data);
    if kind == HeaderKind::Obl {
        return Ok(decompile::Decompiled {
            source: run(&data, Mode::Decompile)?,
            dot: run(&data, Mode::Cfg)?,
        });
    }
    decompile::decompile(&parse_script(&data, kind)?)
}
