// node vault/scripts/vault-map.js
// Regenerates the vault map in the root README: a Mermaid diagram of root -> branches -> sub-indexes
// (nodes that have children), each with the number of nodes under it, between the markers
// <!-- vault-map:start --> and <!-- vault-map:end -->; also refreshes the node counts of the branch table
// and the "(N nodes)" total. Same node set as lint.js. Does not commit.
const fs = require('fs'), path = require('path');
const VAULT = path.resolve(__dirname, '..'), README = path.resolve(VAULT, '..', 'README.md');
const SKIP = new Set(['.git', '.obsidian', 'private', 'node_modules', 'scripts', 'templates', 'assets']);
const walk = (d, out = []) => { for (const e of fs.readdirSync(d, { withFileTypes: true })) {
  const p = path.join(d, e.name);
  if (e.isDirectory()) { if (!SKIP.has(e.name)) walk(p, out); } else if (e.name.endsWith('.md')) out.push(p); }
  return out; };
const parent = {}, kids = {};
for (const f of walk(VAULT)) {
  const fm = fs.readFileSync(f, 'utf8').match(/^---\r?\n([\s\S]*?)\r?\n---/); if (!fm) continue;
  const id = (fm[1].match(/^id:\s*(.+)$/m) || [])[1], par = (fm[1].match(/^parent:\s*(.+)$/m) || [])[1];
  if (!id) continue;
  parent[id.trim()] = par ? par.trim() : null;
  if (par) (kids[par.trim()] = kids[par.trim()] || []).push(id.trim());
}
const total = Object.keys(parent).filter(id => !id.startsWith('_')).length; // same count as lint.js
const size = id => 1 + (kids[id] || []).reduce((s, k) => s + size(k), 0);
const safe = id => id.replace(/[^A-Za-z0-9]/g, '_');
const label = id => id.replace(/-index$/, '');
const branches = (kids.root || []).filter(id => !id.startsWith('_')).sort((a, b) => size(b) - size(a));
const lines = ['```mermaid', 'flowchart LR', `  root(("root · ${total} nodes"))`];
for (const b of branches) {
  lines.push(`  root --> ${safe(b)}["${label(b)} · ${size(b)}"]`);
  for (const c of (kids[b] || []).filter(k => kids[k]).sort((x, y) => size(y) - size(x)))
    lines.push(`  ${safe(b)} --> ${safe(c)}["${label(c)} · ${size(c)}"]`);
}
lines.push('```');
let r = fs.readFileSync(README, 'utf8');
const re = /(<!-- vault-map:start -->)[\s\S]*?(<!-- vault-map:end -->)/;
if (!re.test(r)) { console.error('markers <!-- vault-map:start --> / <!-- vault-map:end --> not found in README.md'); process.exit(1); }
r = r.replace(re, (m, a, z) => `${a}\n${lines.join('\n')}\n${z}`);
r = r.replace(/\((\d+) nodes\)/, `(${total} nodes)`);
for (const b of branches) {
  const name = label(b);
  r = r.replace(new RegExp('(\\| \\[`' + name + '`\\]\\([^)]*\\) \\| )\\d+( \\|)'), `$1${size(b)}$2`);
}
fs.writeFileSync(README, r);
console.log(`vault map: ${total} nodes, ${branches.length} branches`);
