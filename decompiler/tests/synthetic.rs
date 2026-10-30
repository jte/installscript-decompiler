//! End-to-end tests on small hand-assembled scripts.

use decompiler::{Mode, run};

#[derive(Default)]
struct B(Vec<u8>);

impl B {
    fn u8(&mut self, v: u8) -> &mut Self {
        self.0.push(v);
        self
    }
    fn u16(&mut self, v: u16) -> &mut Self {
        self.0.extend(v.to_le_bytes());
        self
    }
    fn i16(&mut self, v: i16) -> &mut Self {
        self.0.extend(v.to_le_bytes());
        self
    }
    fn u32(&mut self, v: u32) -> &mut Self {
        self.0.extend(v.to_le_bytes());
        self
    }
    fn i32(&mut self, v: i32) -> &mut Self {
        self.0.extend(v.to_le_bytes());
        self
    }
    fn s(&mut self, v: &str) -> &mut Self {
        self.u16(v.len() as u16);
        self.0.extend(v.as_bytes());
        self
    }
    fn pos(&self) -> u32 {
        self.0.len() as u32
    }
    fn patch_u32(&mut self, at: usize, v: u32) {
        self.0[at..at + 4].copy_from_slice(&v.to_le_bytes());
    }
}

// .inx/.obs operand encodings
fn str_var(b: &mut B, a: i16) {
    b.u8(4).i16(a);
}
fn num_var(b: &mut B, a: i16) {
    b.u8(5).i16(a);
}
fn str_const(b: &mut B, s: &str) {
    b.u8(6).s(s);
}
fn num_const(b: &mut B, n: i32) {
    b.u8(7).i32(n);
}
fn obj_var(b: &mut B, a: i16) {
    b.u8(8).i16(a);
}

fn encrypt(buf: &mut [u8]) {
    for (i, b) in buf.iter_mut().enumerate() {
        *b = b.wrapping_add((i % 0x47) as u8).rotate_left(2) ^ 0xf1;
    }
}

/// A one-function script: `Main(STRING)` plus a DLL prototype, three blocks.
fn new_format(obs: bool) -> Vec<u8> {
    let header_size = if obs { 0x100 } else { 124 };
    let mut b = B(vec![0; header_size]);
    if obs {
        b.0[..4].copy_from_slice(&[0x48, 0x4f, 0xf3, 0xc9]);
    } else {
        b.0[..4].copy_from_slice(b"aLuZ");
    }

    let typedefs = b.pos();
    b.u16(1);
    if obs {
        b.s("POINT");
    }
    b.u16(2).u8(4).u16(1).s("a").u8(0).u16(10).s("b");

    let (mut variables, mut strings, mut variants, mut externs) = (0, 0, 0, 0);
    if obs {
        strings = b.pos();
        b.u16(1).u16(0).u16(0).u16(50);
        variants = b.pos();
        b.u16(3)
            .u16(0)
            .u16(0xffff)
            .u16(0)
            .u16(0xffff)
            .u16(0x2000)
            .u16(4);
        externs = b.pos();
        b.u16(1).u8(3).i16(1).s("MyNum");
    } else {
        variables = b.pos();
        b.u16(2);
        b.u16(3)
            .u16(0)
            .u16(0xffff)
            .u16(0)
            .u16(0xffff)
            .u16(0x2000)
            .u16(4);
        b.u16(1).u16(1).u16(0).u16(50);
    }

    let prototypes = b.pos();
    b.u16(2);
    b.u8(2 | 8).u8(3).u16(0).s("Main").u16(0).u16(1).u8(0).u8(5);
    b.u8(1)
        .u8(4)
        .s("KERNEL32")
        .s("GetTickCount")
        .u16(0xffff)
        .u16(0);

    let bb0 = b.pos();
    b.u16(6);
    b.u16(34).u16(0).u8(7);
    let prolog_offset = b.pos();
    b.u32(0);
    b.u16(32).u16(1).u16(0);
    b.u16(6).u16(2);
    num_var(&mut b, -101);
    obj_var(&mut b, 0);
    b.u16(13).u16(3);
    num_var(&mut b, -101);
    num_var(&mut b, -101);
    num_const(&mut b, 0);
    b.u16(4).u16(2);
    num_const(&mut b, 1);
    num_var(&mut b, -101);
    b.u16(6).u16(2);
    str_var(&mut b, -102);
    str_const(&mut b, "a\"b\\c\n");

    let bb1 = b.pos();
    b.u16(1).u16(5).u16(1);
    num_const(&mut b, -1);

    let bb2 = b.pos();
    b.u16(2).u16(35).u16(1);
    num_var(&mut b, -101);
    b.u16(38).u16(0).u16(0).u16(0).u16(0).u16(0);

    let bbs = b.pos();
    if !obs {
        b.u16(3);
    }
    b.u32(bb0).u32(bb1).u32(bb2);

    let decls = b.pos();
    b.u16(1).u16(0).u16(2).u16(0);
    b.patch_u32(prolog_offset as usize, decls - prolog_offset);

    if obs {
        b.0[0x62..0x64].copy_from_slice(&3u16.to_le_bytes());
        b.0[0x74..0x76].copy_from_slice(&2u16.to_le_bytes());
        b.patch_u32(0x7c, strings);
        b.patch_u32(0x80, variants);
        b.patch_u32(0x84, externs);
        b.patch_u32(0x88, prototypes);
        b.patch_u32(0x8c, typedefs);
        b.patch_u32(0xd8, bbs);
    } else {
        b.patch_u32(104, variables);
        b.patch_u32(108, prototypes);
        b.patch_u32(112, typedefs);
        b.patch_u32(116, bbs);
        encrypt(&mut b.0);
    }
    b.0
}

const MAIN_BODY: &str = "\
function INT Main(STRING LclVarStr1)
INT LclVarNum1;
STRING LclVarStr2;
begin
\tlabel_1:
\tLAST_RESULT = KERNEL32.GetTickCount();
\tLclVarNum1 = LAST_RESULT;
\tLclVarNum1 = LclVarNum1 == 0;
\tif LclVarNum1 then
\t\tLclVarStr2 = \"a\\\"b\\\\c\\n\";
\tendif;
\tgoto label_1;
\treturn LclVarNum1;
end;

";

#[test]
fn inx_decompiles() {
    let out = run(&new_format(false), Mode::Decompile).unwrap();
    let expected = format!(
        "typedef STRUCT_1\n\tbegin\n\t\tLONG a;\n\t\tSTRING b[10];\n\tend;\n\n\n\
         INT GblVarNum0\nINT GblVarNum1\nSTRING GblVarStr0[50]\n\
         VARIANT LAST_RESULT\nVARIANT NOTHING\nVARIANT Err(4)\n\n{MAIN_BODY}"
    );
    assert_eq!(out, expected);
}

#[test]
fn obs_decompiles_with_externs() {
    let out = run(&new_format(true), Mode::Decompile).unwrap();
    let expected = format!(
        "typedef POINT\n\tbegin\n\t\tLONG a;\n\t\tSTRING b[10];\n\tend;\n\n\n\
         INT GblVarNum0\nINT MyNum\nSTRING GblVarStr0[50]\n\
         VARIANT LAST_RESULT\nVARIANT NOTHING\nVARIANT Err(4)\n\n{MAIN_BODY}"
    );
    assert_eq!(out, expected);
}

#[test]
fn inx_disassembles() {
    let out = run(&new_format(false), Mode::Disassemble).unwrap();
    assert!(
        out.starts_with("export prototype INT Main(STRING);\nnumNumbers: 1\nnumStrings: 2\n\n"),
        "{out}"
    );
    assert!(out.contains(": KERNEL32.GetTickCount()\n"), "{out}");
    assert!(out.contains(": If (else:1) v {CNumArg} -101\n"), "{out}");
    assert!(out.contains(": goto -1;\n"), "{out}");
    assert!(out.contains(": EndFuncAction\n"), "{out}");
}

#[test]
fn inx_cfg() {
    let out = run(&new_format(false), Mode::Cfg).unwrap();
    assert!(out.starts_with("digraph \"Main\" {\n"), "{out}");
    assert!(
        out.contains("\t\"BB_0\" -> \"BB_1\"\n\t\"BB_0\" -> \"BB_2\"\n"),
        "{out}"
    );
    assert!(out.contains("\t\"BB_1\" -> \"BB_2\"\n"), "{out}");
}

#[test]
fn obl_library() {
    let obs = new_format(true);
    let mut b = B::default();
    b.0.extend(b"pOdA");
    b.u32(1).u32(2);
    let first = b.pos() as usize;
    b.s("one.obs").u32(0).u32(obs.len() as u32);
    b.s("broken.obs").u32(0).u32(4);
    let data_at = b.pos();
    b.patch_u32(first + 2 + 7, data_at);
    b.0.extend(&obs);
    let out = run(&b.0, Mode::Decompile).unwrap();
    assert!(
        out.starts_with("// ===== one.obs =====\ntypedef POINT\n"),
        "{out}"
    );
    assert!(out.contains(MAIN_BODY), "{out}");
    assert!(
        out.contains("// ===== broken.obs =====\n// could not process broken.obs: "),
        "{out}"
    );
}

// .ins operand encodings
fn o_num_var(b: &mut B, a: i16) {
    b.u8(0x30).i16(a);
}
fn o_num_const(b: &mut B, n: i32) {
    b.u8(0x41).i32(n);
}
fn o_str_var(b: &mut B, a: i16) {
    b.u8(0x50).i16(a);
}
fn o_str_const(b: &mut B, s: &str) {
    b.u8(0x61).s(s);
}

fn ins_script() -> Vec<u8> {
    let mut b = B(vec![0xb8, 0xc9, 0x0c, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
    b.s("Copyright").u16(3);
    b.u16(1).u16(0);
    b.u16(1).u16(1).s("SRCDIR");
    b.u16(2);
    b.u16(0);
    b.u16(0);
    b.u16(2);
    b.u8(0).u8(3).u16(0);
    b.u8(2).u8(3).s("").s("MyFunc").u16(1).u16(1).u16(3).u16(6);

    b.u16(0).u16(3);
    b.u16(0x13);
    o_num_var(&mut b, 1);
    o_num_const(&mut b, 7);
    b.u16(0xb5).u8(0).u16(1).u8(0).u16(1);
    o_num_const(&mut b, 5);
    b.u16(0x2b);

    b.u16(0).u16(3);
    b.u16(0xb6).u16(0).u16(1).u16(0).u16(0).u16(2).u16(0);
    b.u16(0x22).u8(0).u16(2).u8(0x95);
    o_num_var(&mut b, -101);
    b.0.extend(b"A\0\0\0\0");
    b.u16(0x112);
    o_num_const(&mut b, 0);
    o_str_const(&mut b, "ID_TITLE");
    o_str_var(&mut b, -101);

    b.u16(0).u16(2);
    b.u16(0x2c).u8(0x70).u16(1);
    b.u16(0xb7);
    o_num_var(&mut b, -102);
    b.0
}

#[test]
fn ins_decompiles() {
    let out = run(&ins_script(), Mode::Decompile).unwrap();
    let expected = "\n\
INT GblVarNum0
INT GblVarNum1
STRING GblVarStr0
STRING SRCDIR

function INT Func_0()
begin
\tGblVarNum1 = 7;
\tGblVarNum0 = MyFunc(5);
\tGblVarNum0 = GblVarNum0;
end;

function INT MyFunc(INT LclVarNum1)
INT LclVarNum1;
INT LclVarNum2;
STRING LclVarStr1;
begin
\tlabel_1:
\tif LclVarNum1 then
\t\tLclVarStr1 = @ID_TITLE;
\tendif;
\tgoto label_1;
\treturn LclVarNum2;
end;

";
    assert_eq!(out, expected);
}

#[test]
fn ins_disassembles() {
    let out = run(&ins_script(), Mode::Disassemble).unwrap();
    assert!(out.contains("0: MyFunc(5)\n"), "{out}");
    assert!(
        out.contains("1: IF (label: 0, realAddress: 2) v {CNumArg} -101\n"),
        "{out}"
    );
    assert!(out.contains("2: GOTO 1\n"), "{out}");
}

#[test]
fn garbage_is_an_error_not_a_panic() {
    assert!(run(b"hello", Mode::Decompile).is_err());
    assert!(run(&[], Mode::Decompile).is_err());
    let mut inx = new_format(false);
    inx.truncate(300);
    assert!(run(&inx, Mode::Decompile).is_err());
    for len in (0..ins_script().len()).step_by(7) {
        let _ = run(&ins_script()[..len], Mode::Decompile);
    }
}

#[test]
fn single_byte_corruption_never_panics() {
    for input in [new_format(true), ins_script()] {
        for i in 0..input.len() {
            for v in [0x00, 0x01, 0x7f, 0x80, 0xff] {
                let mut data = input.clone();
                data[i] = v;
                for mode in [Mode::Decompile, Mode::Disassemble] {
                    let _ = run(&data, mode);
                }
            }
        }
    }
}
