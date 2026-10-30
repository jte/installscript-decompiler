// Build the static site into web/dist, ready to upload to Cloudflare (or any
// static host): compiles the wasm package, then copies www/ without the extra
// files wasm-pack leaves in pkg/ (README, package.json, .d.ts typings).
import { execSync } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const web = path.dirname(fileURLToPath(import.meta.url));
const www = path.join(web, "www");
const dist = path.join(web, "dist");

execSync("wasm-pack build . --target web --release --out-dir www/pkg", { cwd: web, stdio: "inherit" });

fs.rmSync(dist, { recursive: true, force: true });
fs.cpSync(www, dist, {
  recursive: true,
  filter: (src) => {
    const rel = path.relative(www, src);
    if (!rel.startsWith("pkg" + path.sep)) return true;
    return /\.(js|wasm)$/.test(src);
  },
});

for (const f of fs.readdirSync(dist, { recursive: true })) {
  const p = path.join(dist, f);
  if (fs.statSync(p).isFile()) console.log(`  ${f.padEnd(40)} ${(fs.statSync(p).size / 1024).toFixed(1)} KB`);
}
console.log(`\nBuilt ${path.relative(process.cwd(), dist) || dist}`);
