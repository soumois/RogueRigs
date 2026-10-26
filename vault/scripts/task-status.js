// node vault/scripts/task-status.js <path under vault/reference/tasks> status=<s> [next_step=<v>] [result=<text>]
// Sets the frontmatter status and bullet fields of a task node; keeps the index status line in sync for global tasks (done ones move to tasks-done-index).
const fs = require('fs');
const f = require('path').resolve(__dirname, '..', 'reference', 'tasks', process.argv[2]);
let s = fs.readFileSync(f, 'utf8');
// A subtask is not closed with its cost unrecorded: run record-cost.js (task-next.js does) or say why it cannot be measured.
const extra = process.argv.slice(3);
if (/^task-\d+-[^/]+[\\/]task-\d+-\d+-/.test(process.argv[2]) && extra.includes('status=done') && /^- tokens: open\s*$/m.test(s) && !extra.some(x => x.startsWith('tokens='))) {
  throw new Error('refusing to close ' + process.argv[2] + ': tokens is still open. Run record-cost.js (task-next.js does), or pass tokens=<why it cannot be measured>.');
}
for (const a of process.argv.slice(3)) {
  const i = a.indexOf('='); const k = a.slice(0, i), v = a.slice(i + 1);
  if (k === 'status') s = s.replace(/^status: .*$/m, 'status: ' + v);
  else { const re = new RegExp('^- ' + k + ': .*$', 'm'); if (!re.test(s)) throw new Error('no field ' + k); s = s.replace(re, () => '- ' + k + ': ' + v); }
}
fs.writeFileSync(f, s);
// keep the index status line in sync for global tasks; a global task that becomes done moves from
// tasks-to-do-index to tasks-done-index (and back if it leaves done), with its parent field and parent link.
const m = process.argv[2].match(/^(task-\d+-[^/]+)\.md$/);
const st = (process.argv.find(a => a.startsWith('status=')) || '').slice(7);
if (m && st) {
  const dir = require('path').resolve(__dirname, '..', 'reference', 'tasks');
  const P = n => require('path').join(dir, n + '.md');
  const lineRe = new RegExp('^- \\[\\[' + m[1] + '\\]\\].*$', 'm');
  const from = ['tasks-to-do-index', 'tasks-done-index'].find(n => lineRe.test(fs.readFileSync(P(n), 'utf8')));
  if (!from) throw new Error(m[1] + ' is in neither tasks-to-do-index nor tasks-done-index');
  let src = fs.readFileSync(P(from), 'utf8');
  const line = src.match(lineRe)[0].replace(/(Status )[a-z-]+\./, '$1' + st + '.');
  const to = st === 'done' ? 'tasks-done-index' : 'tasks-to-do-index';
  if (to === from) fs.writeFileSync(P(from), src.replace(lineRe, () => line));
  else {
    fs.writeFileSync(P(from), src.replace(new RegExp(lineRe.source + '\\n?', 'm'), ''));
    // append, then keep the task lines ordered by task number
    const t = (fs.readFileSync(P(to), 'utf8').replace(/\n*$/, '') + '\n' + line + '\n').split('\n');
    const isTask = l => /^- \[\[task-\d+/.test(l), num = l => +l.match(/task-(\d+)/)[1];
    const first = t.findIndex(isTask), tasks = t.filter(isTask).sort((x, y) => num(x) - num(y));
    fs.writeFileSync(P(to), [...t.slice(0, first).filter(l => !isTask(l)), ...tasks, ...t.slice(first).filter(l => !isTask(l))].join('\n'));
    let node = fs.readFileSync(f, 'utf8');
    node = node.replace(/^parent: .*$/m, 'parent: ' + to).replace('- [[' + from + ']] — parent index.', '- [[' + to + ']] — parent index.');
    fs.writeFileSync(f, node);
  }
}
