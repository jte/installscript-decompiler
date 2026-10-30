# Web front end

A static site that runs the decompiler in the browser via WebAssembly. Files are
processed locally in a Web Worker and never uploaded.

## Build

Prerequisites (once):

```sh
rustup target add wasm32-unknown-unknown
cargo install wasm-pack      # or: npm install -g wasm-pack
```

Build the wasm package into `www/pkg/` (from the repository root):

```sh
wasm-pack build web --target web --release --out-dir www/pkg
```

Without wasm-pack, the same output comes from `wasm-bindgen-cli` (its version
must match the `wasm-bindgen` crate in `Cargo.lock`):

```sh
cargo build -p installscript-decompiler-web --target wasm32-unknown-unknown --release
wasm-bindgen --target web --out-dir web/www/pkg target/wasm32-unknown-unknown/release/installscript_decompiler_web.wasm
```

## Run locally

Module workers don't load from `file://`, so serve `www/` over HTTP:

```sh
python -m http.server -d web/www 8080
```

then open http://localhost:8080.

## Deploy

`npm run build` (in `web/`) builds the wasm package and assembles a clean
`dist/`; `npm run deploy` also uploads it to Cloudflare Workers static assets
with wrangler (config: `wrangler.jsonc`; run `npx wrangler@4 login` first).
`dist/` is a plain static folder, so any static host that serves `.wasm` as
`application/wasm` works too.

Once the site has a domain, add `<link rel="canonical">` and `og:url` to
`www/index.html` and a `Sitemap:` line to `www/robots.txt`.

## Analytics

Cloudflare Web Analytics (cookieless, page views only — nothing about the
user's file is ever sent) is loaded only if
`<meta name="cf-beacon-token" content="">` in `www/index.html` holds a token.

## Notes

- A Rust panic aborts the wasm instance, so the page discards the worker and
  starts a fresh one after any crash. Jobs that run longer than 60 s are killed
  the same way.
- `.cargo/config.toml` gives the wasm build a 64 MiB shadow stack to match the
  CLI, but the browser's own call-stack limit still applies.
