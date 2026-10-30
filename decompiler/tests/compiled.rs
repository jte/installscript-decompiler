//! Scripts compiled by the real InstallShield compiler. The sources are next
//! to the binaries in `tests/fixtures`.

use decompiler::{Mode, run, run_with_symbols};

fn decompile(bytes: &[u8]) -> String {
    run(bytes, Mode::Decompile).unwrap()
}

const DEBUG_INX: &[u8] = include_bytes!("fixtures/debug.inx");
const DEBUG_DBG: &[u8] = include_bytes!("fixtures/debug.dbg");

/// `debug.rul` decompiled with its debug symbols: the source, but for
/// comments and the struct's name.
const WITH_SYMBOLS: &str =
    "// Names restored from debug symbols. Source files: debug_inc.h, debug.rul

typedef STRUCT_1
	begin
		LONG x;
		LONG y;
	end;

INT gFromHeader;
INT gCount;
STRING gName[32];

prototype LONG Helper(LONG);
prototype LONG First(LONG, STRING);
prototype LONG Locals(STRING, VARIANT);

// debug_inc.h, lines 9-10
function LONG Helper(nArg)
begin
	gFromHeader = nArg;
	return;
end;

// debug.rul, lines 24-28
function LONG First(nParam, szParam)
INT nLocal;
STRING szLocal;
begin
	nLocal = nParam + 1;
	szLocal = szParam;
	Helper(nLocal);
	return nLocal;
end;

// debug.rul, lines 35-39
function LONG Locals(szA, vA)
STRING szSized[20];
STRUCT_1 pt;
NUMBER nArr(3);
begin
	szSized = szA;
	pt.x = 1;
	nArr(0) = pt.x;
	vA = nArr(0);
	return;
end;

// debug.rul, lines 42-44
program
	gCount = First(1, gName);
	Locals(\"a\", 1);
	exit;
endprogram

";

#[test]
fn separate_debug_symbols_restore_names() {
    let out = run_with_symbols(DEBUG_INX, Some(DEBUG_DBG), Mode::Decompile).unwrap();
    assert_eq!(out, WITH_SYMBOLS);
}

#[test]
fn embedded_debug_symbols_restore_names() {
    assert_eq!(
        decompile(include_bytes!("fixtures/debug_embedded.inx")),
        WITH_SYMBOLS
    );
}

#[test]
fn missing_debug_symbols_are_pointed_out() {
    let out = decompile(DEBUG_INX);
    assert!(out.starts_with(
        "// Compiled with debug symbols in C:\\Projects\\installscript-decompiler\\target\\dbgfix\\debug.dbg\n\
         // Supply that .dbg file to restore the original names.\n\n"
    ));
    assert!(out.contains("\nfunction LONG Func_1(LclVarNum1, LclVarStr1)\n"));
    // Local declarations come out right without symbols too.
    assert!(
        out.contains("\nSTRING LclVarStr2[20];\nSTRUCT_1 LclVarObj2;\nNUMBER LclVarObj3(3);\n")
    );
}

#[test]
fn debug_symbols_of_another_build_are_ignored() {
    let out = run_with_symbols(
        include_bytes!("fixtures/control_flow.inx"),
        Some(DEBUG_DBG),
        Mode::Decompile,
    )
    .unwrap();
    assert!(out.starts_with(
        "// The supplied debug symbols belong to a different build and were ignored.\n\n"
    ));
    assert_eq!(
        out.split_once("\n\n").unwrap().1,
        decompile(include_bytes!("fixtures/control_flow.inx"))
    );
}

#[test]
fn a_script_is_not_debug_symbols() {
    let err = run_with_symbols(DEBUG_INX, Some(DEBUG_INX), Mode::Decompile).unwrap_err();
    assert!(err.contains("Not an InstallScript debug symbols file"));
}

#[test]
fn externals_keep_their_names_and_types() {
    let out = decompile(include_bytes!("fixtures/externs.obs"));
    assert_eq!(
        out,
        "\
external INT extNumber;
external STRING extString;
external VARIANT extVariant;

export prototype LONG UseExterns();

function LONG UseExterns()
begin
\textNumber = 7;
\textString = \"text\";
\textVariant = extNumber;
\treturn;
end;

"
    );
}

const PROGRAM: &str = "\
program
\tGblVarStr0 = Func_1(\"World\");
\tGblVarNum0 = Func_0(2, 3);
\tFunc_2(GblVarNum0);
\tFunc_6(GblVarNum0, GblVarStr1);
\tGblVarObj{pt}.x = 10;
\tGblVarNum1 = GblVarObj{pt}.x;
\tGblVarNum1 = GblVarNum1 - 3;
\tGblVarObj{pt}.y = GblVarNum1;
\tGblVarNum0 = Kernel32.GetTickCount();
\texit;
endprogram
";

/// `control_flow.rul` as decompiled: its loops and `if`/`elseif` chains come
/// back as such, the `&&` and the `case 2, 3:` included.
const FUNCTIONS: &str = "\
prototype LONG Func_0(LONG, LONG);
prototype STRING Func_1(STRING);
prototype LONG Func_2(LONG);
prototype LONG Func_6(LONG, BYREF STRING);
prototype LONG Kernel32.GetTickCount();
";

const BODIES: &str = "\
function LONG Func_0(LclVarNum1, LclVarNum2)
begin
\treturn LclVarNum1 + LclVarNum2;
end;

function STRING Func_1(LclVarStr1)
begin
\treturn \"Hello, \" + LclVarStr1 + \"!\\n\";
end;

function LONG Func_2(LclVarNum1)
INT LclVarNum2;
INT LclVarNum3;
begin
\tLclVarNum3 = 0;
\tfor LclVarNum2 = 1 to LclVarNum1
\t\tLclVarNum3 = LclVarNum3 + (LclVarNum2 * 2);
\tendfor;
\twhile (LclVarNum3 > 10)
\t\tLclVarNum3 = LclVarNum3 / 2;
\tendwhile;
\tGblVarNum0 = LclVarNum3;
\treturn;
end;

function LONG Func_6(LclVarNum1, LclVarStr1)
begin
\tif (LclVarNum1 == 0) then
\t\tLclVarStr1 = \"zero\";
\t\treturn 0;
\telseif (LclVarNum1 < 0) then
\t\tLclVarStr1 = \"negative\";
\telseif ((LclVarNum1 > 100) && (LclVarNum1 != 200)) then
\t\tLclVarStr1 = \"big\";
\telse
\t\tLclVarStr1 = \"small\";
\tendif;
\tif (LclVarNum1 == 1) then
\t\tLclVarStr1 = LclVarStr1 + \"one\";
\telseif ((LclVarNum1 == 2) || (LclVarNum1 == 3)) then
\t\tLclVarStr1 = LclVarStr1 + \"two-three\";
\telse
\t\tLclVarStr1 = LclVarStr1 + \"?\";
\tendif;
\treturn -1;
end;
";

fn check_control_flow(out: &str, pt: &str) {
    assert!(out.contains(FUNCTIONS), "{out}");
    assert!(out.contains(BODIES), "{out}");
    assert!(out.contains(&PROGRAM.replace("{pt}", pt)), "{out}");
}

#[test]
fn repeat_until_loop() {
    let out = decompile(include_bytes!("fixtures/repeat.inx"));
    assert!(out.contains(
        "\
begin
\tLclVarNum2 = 0;
\trepeat
\t\tLclVarNum1 = LclVarNum1 - 1;
\t\tLclVarNum2 = LclVarNum2 + 1;
\tuntil ((LclVarNum1 <= 0) || (LclVarNum2 == 10));
\treturn LclVarNum2;
end;
"
    ));
}

#[test]
fn linked_setup_script() {
    let out = decompile(include_bytes!("fixtures/control_flow.inx"));
    assert!(out.starts_with("typedef STRUCT_1\n"));
    check_control_flow(&out, "3");
}

#[test]
fn object_file() {
    let out = decompile(include_bytes!("fixtures/control_flow.obs"));
    assert!(out.starts_with("typedef POINT\n"));
    assert!(out.contains("\nPOINT GblVarObj4;\n"));
    check_control_flow(&out, "4");
}

#[test]
fn damaged_debug_symbols_do_not_panic() {
    for len in 0..DEBUG_DBG.len() {
        let _ = run_with_symbols(DEBUG_INX, Some(&DEBUG_DBG[..len]), Mode::Decompile);
    }
    let embedded = include_bytes!("fixtures/debug_embedded.inx");
    for i in 0..embedded.len() {
        for v in [0x00, 0xff, embedded[i] ^ 0x5a] {
            let mut data = embedded.to_vec();
            data[i] = v;
            let _ = run(&data, Mode::Decompile);
        }
    }
}

#[test]
fn features() {
    let out = decompile(include_bytes!("fixtures/features.inx"));
    // A DLL's STRING parameter is declared plainly.
    assert!(
        out.contains("prototype LONG Kernel32.lstrlenA(STRING);\n"),
        "{out}"
    );
    // The empty last case keeps the statement every case jumps to.
    assert!(
        out.contains(
            "\
\telseif (LclVarNum1 == 3) then
\tendif;
\treturn 0;
end;"
        ),
        "{out}"
    );
    // The NUL is escaped, in octal as the compiler wants.
    assert!(out.contains("\tLclVarStr1 = \"a\\000b\";\n"), "{out}");
    // BYREF arguments stay variables.
    assert!(
        out.contains(
            "\
\tLclVarNum1 = 0;
\tLclVarStr1 = \"\";
\tFunc_5(LclVarNum1, LclVarStr1);"
        ),
        "{out}"
    );
    // A member called with an argument.
    assert!(
        out.contains("\tGblVarStr0 = GblVarObj3.Item(\"key\");\n"),
        "{out}"
    );
}
