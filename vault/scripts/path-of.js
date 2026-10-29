// node vault/scripts/path-of.js <id> [<id>...]
// Prints the repo-relative path of each exact vault id, one line per id in argument order (task-358, Q2).
// Resolution, not search: an id matches only a node file named exactly `<id>.md`; no partial, prefix, case or fuzzy
// match. A `[[id]]` or `[[id|alias]]` argument is accepted and read as `id`. Covers every node lint.js walks (same
// skipped folders), task globals and subtasks and the `_` meta nodes included, so `_paths.md` need not be opened.
// Output: `vault/<path>` per known id on stdout; `path-of: unknown id <id>` on stderr per unknown id; exit 1 when
// any id is unknown (the known ones are still printed), 0 otherwise.
// Module use (task-create.js): const { idMap } = require('./path-of'); idMap() -> { id: 'vault/<path>' }.
const fs = require('fs');
const path = require('path');
const VAULT = path.resolve(__dirname, '..');
// same folders lint.js skips
const SKIP_DIRS = new Set(['.git', '.obsidian', 'private', 'node_modules', 'scripts', 'templates', 'assets']);

function idMap() {
  const map = {};
  const walk = dir => {
    for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
      const p = path.join(dir, e.name);
      if (e.isDirectory()) { if (!SKIP_DIRS.has(e.name)) walk(p); }
      else if (e.name.endsWith('.md')) map[e.name.slice(0, -3)] = 'vault/' + path.relative(VAULT, p).split(path.sep).join('/');
    }
  };
  walk(VAULT);
  return map;
}

module.exports = { idMap };

if (require.main === module) {
  const ids = process.argv.slice(2).map(a => a.replace(/^\[\[/, '').replace(/\]\]$/, '').split('|')[0].trim());
  if (!ids.length) { console.error('usage: path-of.js <id> [<id>...]'); process.exit(1); }
  const map = idMap();
  let bad = 0;
  for (const id of ids) {
    if (Object.prototype.hasOwnProperty.call(map, id)) console.log(map[id]);
    else { console.error('path-of: unknown id ' + id); bad++; }
  }
  process.exit(bad ? 1 : 0);
}
