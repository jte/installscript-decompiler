//! Scripts compiled by the real InstallShield compiler. The sources are next
//! to the binaries in `tests/fixtures`.

use decompiler::{Mode, run, run_with_symbols};

fn decompile(bytes: &[u8]) -> String {
    run(bytes, Mode::Decompile).unwrap()
}

const DEBUG_INX: &[u8] = include_bytes!("fixtures/debug.inx");
const DEBUG_DBG: &[u8] = include_bytes!("fixtures/debug.dbg");

/// `debug.rul` decompiled with its debug symbols.
const WITH_SYMBOLS: &str = "\
// Names restored from debug symbols. Source files: debug_inc.h, debug.rul

typedef STRUCT_1
\tbegin
\t\tLONG x;
\t\tLONG y;
\tend;


INT gFromHeader
INT gCount
STRING gName[32]
VARIANT LAST_RESULT
VARIANT NOTHING
VARIANT Err

// debug_inc.h, lines 9-10
function LONG Helper(INT nArg)
begin
\tgFromHeader = nArg;
\treturn;
end;

// debug.rul, lines 24-28
function LONG First(INT nParam,STRING szParam)
INT nLocal;
STRING szLocal;
begin
\tnLocal = nParam + 1;
\tszLocal = szParam;
\tLAST_RESULT = Helper(nLocal);
\tLAST_RESULT = LAST_RESULT;
\treturn nLocal;
end;

// debug.rul, lines 35-39
function LONG Locals(STRING szA,VARIANT vA)
INT LclVarNum1;
STRING szSized[20];
STRUCT_1 pt;
VARIANT nArr(3);
begin
\tszSized = szA;
\tpt.x = 1;
\tLAST_RESULT = pt.x;
\tLclVarNum1 = LAST_RESULT;
\tnArr(0) = LclVarNum1;
\tLAST_RESULT = nArr(0);
\tvA = LAST_RESULT;
\treturn;
end;

// debug.rul, lines 42-44
function VOID program()
begin
\tLAST_RESULT = First(1, gName);
\tgCount = LAST_RESULT;
\tLAST_RESULT = Locals(\"a\", 1);
\texit;
end;

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
    assert!(out.contains("\nfunction LONG Func_1(INT LclVarNum1,STRING LclVarStr1)\n"));
    // Local declarations come out right without symbols too.
    assert!(
        out.contains("\nSTRING LclVarStr2[20];\nSTRUCT_1 LclVarObj2;\nVARIANT LclVarObj3(3);\n")
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
        "
INT extNumber
STRING extString
VARIANT LAST_RESULT
VARIANT NOTHING
VARIANT Err
VARIANT extVariant

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
function VOID program()
begin
\tLAST_RESULT = Func_1(\"World\");
\tGblVarStr0 = LAST_RESULT;
\tLAST_RESULT = Func_0(2, 3);
\tGblVarNum0 = LAST_RESULT;
\tLAST_RESULT = Func_2(GblVarNum0);
\tLAST_RESULT = Func_6(GblVarNum0, GblVarStr1);
\tGblVarObj{pt}.x = 10;
\tLAST_RESULT = GblVarObj{pt}.x;
\tGblVarNum1 = LAST_RESULT;
\tGblVarNum1 = GblVarNum1 - 3;
\tGblVarObj{pt}.y = GblVarNum1;
\tLAST_RESULT = Kernel32.GetTickCount();
\tGblVarNum0 = LAST_RESULT;
\texit;
end;
";

fn check_control_flow(out: &str, pt: &str) {
    // `n > 100 && n != 200`
    assert!(out.contains(
        "\tLclVarNum2 = LclVarNum2 && LclVarNum3;\n\tif LclVarNum2 then\n\t\tLclVarStr1 = \"big\";"
    ));
    // `case 2, 3:`
    assert!(out.contains("\tLclVarNum2 = LclVarNum2 || LclVarNum3;\n\tif LclVarNum2 then\n\t\tLclVarStr1 = LclVarStr1 + \"two-three\";"));
    // The program block is its own function rather than the tail of the last one.
    assert!(out.contains("\treturn -1;\nend;\n"));
    assert!(out.contains(&format!("{}\n", PROGRAM.replace("{pt}", pt))));
}

#[test]
fn repeat_until_loop() {
    let out = decompile(include_bytes!("fixtures/repeat.inx"));
    assert!(out.contains(
        "\
begin
\tLclVarNum2 = 0;
\tlabel_1:
\tLclVarNum1 = LclVarNum1 - 1;
\tLclVarNum2 = LclVarNum2 + 1;
\tLclVarNum3 = LclVarNum1 <= 0;
\tLclVarNum4 = LclVarNum2 == 10;
\tLclVarNum3 = LclVarNum3 || LclVarNum4;
\tif !LclVarNum3 then
\t\tgoto label_1;
\tendif;
\tLAST_RESULT = LAST_RESULT;
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
    assert!(out.contains("\nPOINT GblVarObj4\n"));
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
