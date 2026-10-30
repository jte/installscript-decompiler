//! Scripts compiled by the real InstallShield compiler. The sources are next
//! to the binaries in `tests/fixtures`.

use decompiler::{Mode, run};

fn decompile(bytes: &[u8]) -> String {
    run(bytes, Mode::Decompile).unwrap()
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
