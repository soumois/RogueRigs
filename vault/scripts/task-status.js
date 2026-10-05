// node vault/scripts/task-status.js <path under vault/reference/tasks> status=<s> [next_step=<v>] [result=<text>]
// Sets the frontmatter status and bullet fields of a task node; keeps the tasks-index status line in sync for global tasks.
const fs = require('fs');
const f = require('path').resolve(__dirname, '..', 'reference', 'tasks', process.argv[2]);
let s = fs.readFileSync(f, 'utf8');
for (const a of process.argv.slice(3)) {
  const i = a.indexOf('='); const k = a.slice(0, i), v = a.slice(i + 1);
  if (k === 'status') s = s.replace(/^status: .*$/m, 'status: ' + v);
  else { const re = new RegExp('^- ' + k + ': .*$', 'm'); if (!re.test(s)) throw new Error('no field ' + k); s = s.replace(re, () => '- ' + k + ': ' + v); }
}
fs.writeFileSync(f, s);
// keep tasks-index status lines in sync for global tasks
const m = process.argv[2].match(/^(task-\d+-[^/]+)\.md$/);
const st = (process.argv.find(a => a.startsWith('status=')) || '').slice(7);
if (m && st) {
  const idx = require('path').resolve(__dirname, '..', 'reference', 'tasks', 'tasks-index.md');
  let t = fs.readFileSync(idx, 'utf8');
  t = t.replace(new RegExp('(\\[\\[' + m[1] + '\\]\\].*Status )[a-z-]+\\.'), '$1' + st + '.');
  fs.writeFileSync(idx, t);
}
