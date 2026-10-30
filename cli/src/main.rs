use std::{fs, path::PathBuf};

use clap::{ArgGroup, Parser};
use decompiler::Mode;

#[derive(Parser)]
#[command(
    name = "installscript-decompiler",
    version,
    about = "Decompiler for InstallShield's InstallScript"
)]
#[command(group(ArgGroup::new("mode").args(["show_actions", "show_decompiled"])))]
struct Cli {
    /// Input file (.inx/.obs/.obl/.ins)
    input_file: PathBuf,

    /// Output file (.rul)
    output_file: PathBuf,

    /// Show actions (disassembly)
    #[arg(long)]
    show_actions: bool,

    /// Show decompilation (the default)
    #[arg(long)]
    show_decompiled: bool,

    /// Write the control flow graph of every function in .dot format to this file
    #[arg(long)]
    output_cfg_file: Option<PathBuf>,
}

fn main() {
    // The structurer recurses once per nested or consecutive `if`; give it room.
    let code = std::thread::Builder::new()
        .stack_size(64 * 1024 * 1024)
        .spawn(run)
        .expect("failed to spawn worker thread")
        .join()
        .unwrap_or(1);
    std::process::exit(code);
}

fn run() -> i32 {
    let args = Cli::parse();
    println!("Starting decompilation...");

    let data = match fs::read(&args.input_file) {
        Ok(d) => d,
        Err(e) => {
            eprintln!("Failed to open the input file: {e}");
            return 1;
        }
    };

    let (output, cfg) = if args.show_actions {
        (decompiler::run(&data, Mode::Disassemble), None)
    } else if args.output_cfg_file.is_some() {
        match decompiler::decompile_with_cfg(&data) {
            Ok(d) => (Ok(d.source), Some(d.dot)),
            Err(e) => (Err(e), None),
        }
    } else {
        (decompiler::run(&data, Mode::Decompile), None)
    };

    let output = match output {
        Ok(o) => o,
        Err(e) => {
            eprintln!("Error: {e}");
            return 1;
        }
    };
    if let Err(e) = fs::write(&args.output_file, output) {
        eprintln!("Error writing {}: {e}", args.output_file.display());
        return 1;
    }
    if let (Some(path), Some(dot)) = (&args.output_cfg_file, cfg)
        && let Err(e) = fs::write(path, dot)
    {
        eprintln!("Error writing {}: {e}", path.display());
        return 1;
    }
    println!("Successful decompilation.");
    0
}
