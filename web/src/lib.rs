//! WebAssembly bindings for the browser front end in `www/`.
//!
//! Panics abort on wasm32-unknown-unknown and leave the instance unusable, so
//! the panic hook forwards the message to JS and the page throws the worker
//! away afterwards.

use decompiler::Mode;
use wasm_bindgen::prelude::*;

#[wasm_bindgen]
extern "C" {
    #[wasm_bindgen(js_namespace = globalThis, js_name = __isDecompilerPanic)]
    fn report_panic(msg: &str);
}

#[wasm_bindgen(start)]
pub fn start() {
    std::panic::set_hook(Box::new(|info| report_panic(&info.to_string())));
}

/// Run `data` (a compiled InstallScript file) through the pipeline.
///
/// `mode` is one of `"decompile"`, `"disassemble"`, `"cfg"`.
#[wasm_bindgen]
pub fn run(data: &[u8], mode: &str) -> Result<String, JsError> {
    let mode = match mode {
        "decompile" => Mode::Decompile,
        "disassemble" => Mode::Disassemble,
        "cfg" => Mode::Cfg,
        other => return Err(JsError::new(&format!("unknown mode {other:?}"))),
    };
    decompiler::run(data, mode).map_err(|e| JsError::new(&e))
}
