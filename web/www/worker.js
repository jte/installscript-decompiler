// Runs the decompiler off the main thread. If the wasm module panics or traps
// it is no longer safe to reuse, so we report `fatal` and the page replaces us.
import init, { run } from "./pkg/installscript_decompiler_web.js";

let lastPanic = null;
globalThis.__isDecompilerPanic = (msg) => { lastPanic = msg; };

const ready = init();

self.onmessage = async ({ data: { id, bytes, mode } }) => {
  try {
    await ready;
  } catch (e) {
    postMessage({ id, ok: false, fatal: true, error: `Failed to load the WebAssembly module: ${e}` });
    return;
  }

  lastPanic = null;
  const t0 = performance.now();
  try {
    const output = run(new Uint8Array(bytes), mode);
    postMessage({ id, ok: true, output, ms: performance.now() - t0 });
  } catch (e) {
    const fatal = lastPanic !== null || e instanceof WebAssembly.RuntimeError || e instanceof RangeError;
    let error = lastPanic ?? (e && e.message) ?? String(e);
    if (e instanceof RangeError) error = `Ran out of stack (${e.message}). The input is too deeply nested for the browser.`;
    postMessage({ id, ok: false, fatal, error });
  }
};
