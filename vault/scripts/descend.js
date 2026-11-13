// node vault/scripts/descend.js <index-id> [<index-id>...]
// Prints, in one call, the `## Children` entries of each given index and, under each child that is itself an index
// (has a `## Children` section), that child's `## Children` entries (task-534, Q1: fixed depth, the index plus one
// level of sub-indexes). Each line: id, repo-relative path, gloss. It is a descent by glosses, not a search: it takes
// only exact ids (resolved like path-of.js, a `[[id]]` argument is read as `id`), has no text or topic parameter,
// and prints no node body, no `## Relations` and nothing outside the descent tree.
// Output per start id:
//   <id>  vault/<path>
//   - <child>  vault/<path> — <gloss>
//     - <grandchild>  vault/<path> — <gloss>
// Errors: `descend: unknown id <id>` or `descend: <id> has no ## Children` on stderr; exit 1 when any start id fails
// (the others are still printed), 0 otherwise. A child link that resolves to no node prints `?` as its path.
const fs = require('fs');
const path = require('path');
const { idMap } = require('./path-of');
const ROOT = path.resolve(__dirname, '..', '..');

// Entries of the `## Children` section of a node file: [{ id, gloss }], or null when the section is absent.
function children(rel) {
  const text = fs.readFileSync(path.join(ROOT, rel), 'utf8').replace(/\r\n/g, '\n');
  const lines = text.split('\n');
  const start = lines.findIndex(l => /^##\s+Children\s*$/.test(l));
  if (start < 0) return null;
  const out = [];
  for (let i = start + 1; i < lines.length && !/^#{1,2}\s/.test(lines[i]); i++) {
    const m = lines[i].match(/^\s*[-*]\s+\[\[([^\]|]+)(?:\|[^\]]*)?\]\]\s*(?:[—–-]+\s*)?(.*)$/);
    if (m) out.push({ id: m[1].trim(), gloss: m[2].trim() });
  }
  return out;
}

function line(indent, map, c) {
  const p = map[c.id] || '?';
  return `${indent}- ${c.id}  ${p}${c.gloss ? ' — ' + c.gloss : ''}`;
}

if (require.main === module) {
  const ids = process.argv.slice(2).map(a => a.replace(/^\[\[/, '').replace(/\]\]$/, '').split('|')[0].trim());
  if (!ids.length) { console.error('usage: descend.js <index-id> [<index-id>...]'); process.exit(1); }
  const map = idMap();
  const has = id => Object.prototype.hasOwnProperty.call(map, id);
  let bad = 0;
  const out = [];
  for (const id of ids) {
    if (!has(id)) { console.error('descend: unknown id ' + id); bad++; continue; }
    const kids = children(map[id]);
    if (!kids) { console.error(`descend: ${id} has no ## Children`); bad++; continue; }
    if (out.length) out.push('');
    out.push(`${id}  ${map[id]}`);
    for (const c of kids) {
      out.push(line('', map, c));
      const sub = has(c.id) ? children(map[c.id]) : null;
      if (sub) for (const g of sub) out.push(line('  ', map, g));
    }
  }
  if (out.length) console.log(out.join('\n'));
  process.exit(bad ? 1 : 0);
}
