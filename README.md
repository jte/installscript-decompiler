# InstallScript Decompiler

A decompiler for InstallShield's InstallScript, written in Rust. The earlier C++
version is kept at the [`cpp-final`](https://github.com/jte/installscript-decompiler/tree/cpp-final)
tag.

It runs as a static website (the decompiler compiled to WebAssembly, so files
never leave the browser) and as a command-line tool.

## Supported input

- `setup.inx` — compiled setup scripts from InstallShield 6 and later
  (encrypted `aLuZ`)
- `.obs` — compiled script objects
- `.obl` — script libraries; every member script is decompiled in turn
- `setup.ins` — InstallShield 5 and earlier

Scripts compiled with debug information (`kUtZ`) are not supported yet.

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
```

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
