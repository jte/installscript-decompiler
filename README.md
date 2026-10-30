# InstallScript Decompiler

A decompiler for InstallShield's InstallScript, written in Rust. The earlier C++
version is kept at the [`cpp-final`](https://github.com/jte/installscript-decompiler/tree/cpp-final)
tag.

It runs as a static website at <https://installscript-decompiler.com> (the
decompiler compiled to WebAssembly, so files never leave the browser) and as a
command-line tool.

## Supported input

- `setup.inx` — compiled setup scripts from InstallShield 6 and later
  (encrypted `aLuZ`)
- `.obs` — compiled script objects
- `.obl` — script libraries; every member script is decompiled in turn
- `setup.ins` — InstallShield 5 and earlier

### Debug symbols

Scripts compiled with debug information carry the original names of
functions, parameters, locals and globals, and which source file and lines
each function came from. The decompiler uses them when it can find them:

- `Compile.exe /GI` embeds them in `setup.inx`; they are used automatically.
- `Compile.exe /G` writes them to a separate `.dbg` file (magic `kUtZ`) and
  records its path in `setup.inx`. Supply that file to use it; without it, the
  output names the path the script was built with.

Functions linked in from `.obl` libraries have no symbols and keep generated
names. Symbols from a different build are detected and ignored.

Programs installed with InstallShield keep their script at
`C:\Program Files (x86)\InstallShield Installation Information\{GUID}\setup.inx`.

## Command line

Requires [Rust](https://rustup.rs).

```sh
cargo build --release -p cli

# decompile to InstallScript source (the default)
target/release/installscript-decompiler setup.inx setup.rul --show-decompiled
# action listing (disassembly)
target/release/installscript-decompiler setup.inx setup.txt --show-actions
# also write every function's control flow graph as Graphviz DOT
target/release/installscript-decompiler setup.inx setup.rul --output-cfg-file setup.dot
# with the debug symbols the script was compiled with
target/release/installscript-decompiler setup.inx setup.rul --debug-symbols setup.dbg
```

Without `--debug-symbols`, a `.dbg` next to the input is used if there is one:
the file name recorded in the script, or the input's name with `.dbg`.

## Project layout

| Crate | Purpose |
|---|---|
| `decompiler` | File detection and decryption, the `.inx`/`.obs` and `.ins` frontends, lowering, structuring and output |
| `cli` | Command-line tool |
| `web` | WebAssembly bindings and the static site — see [web/README.md](web/README.md) |

Inside `decompiler`:

| Module | Purpose |
|---|---|
| `format` | Header detection, `setup.inx` decryption, `.obl` libraries |
| `debug` | Debug symbols (`.dbg` files and the embedded form) |
| `newis` | InstallShield 6+ frontend (`.inx`, `.obs`) |
| `oldis` | InstallShield 5 frontend (`.ins`) |
| `script` | Data model and the action listing |
| `lower` | Lowers actions to statements |
| `ir` | Control flow structuring, printing and Graphviz output |
| `decompile` | Assembles the `.rul` output |
| `vars`, `expr` | Variables, symbol tables and expressions |

Tests in `decompiler/tests/compiled.rs` use scripts compiled by the InstallShield
2026 compiler; their sources are in `decompiler/tests/fixtures`.

## Contributing

Pull requests are welcome. Test samples that don't decompile correctly are
especially useful.

## License

MIT — see [`LICENSE`](LICENSE).
