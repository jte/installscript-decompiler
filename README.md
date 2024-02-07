# MoneyPrinter

InstallScript Decompiler is a decompiler for InstallShield's InstallScript programming language.

## Installation

```bash
git clone https://github.com/jte/installscript-decompiler.git
cd installscript-decompiler

mkdir build
cd build

cmake ..
```

## Usage

```bash
Usage: InstallScriptDecompiler [--help] [--version] [--show-actions] [--show-decompiled] input_file output_file

Positional arguments:
  input_file         input file (.inx/.obs/.obl)
  output_file        output file (.rul)

Optional arguments:
  -h, --help         shows help message and exits
  -v, --version      prints version information and exits
  --show-actions     show actions (disassembly)
  --show-decompiled  show decompilation
```

## Contributing

Pull requests are welcome. I am looking for test samples which don't work.

## License

See [`LICENSE`](LICENSE) file for more information.
