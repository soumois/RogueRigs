// node vault/scripts/task-status.js <path under vault/reference/tasks> status=<s> [next_step=<v>] [result=<text>]
// Sets the frontmatter status and bullet fields of a task node; keeps the index status line in sync for global tasks (done and cancelled ones move to tasks-done-index).
// status=cancelled on a global also sets each of its subtasks that is not done to cancelled and its next_step to none.
// Every status=in-progress (global or subtask, each move) also writes `- started: <UTC ISO 8601 to the second>`, replacing
// the bullet or inserting it before `- result:` on older nodes; other statuses leave `started` unchanged.
// status=wait-answers (the agent stopped for an owner answer or action) needs waiting_for=<question or owner action>
// (refused without it, nothing written); it writes `- waiting_for: <text>`, inserted before `- result:` on older nodes.
// Any other status on a node whose waiting_for is not none sets it to none. A waiting global stays in tasks-to-do-index.
// budget_grants=<n> (global, owner "continue" after a BUDGET stop of task-cost.js) is written, inserted before `- result:` when missing.
// status=done on a global also requires every listed subtask to be done, its model/tokens/usage/runs recorded, and a cost-log row.
const fs = require('fs');
const f = require('path').resolve(__dirname, '..', 'reference', 'tasks', process.argv[2]);
let s = fs.readFileSync(f, 'utf8');
// A subtask is not closed with its cost unrecorded: run record-cost.js (task-next.js does) or say why it cannot be measured.
const extra = process.argv.slice(3);
if (/^task-\d+-[^/]+[\\/]task-\d+-\d+-/.test(process.argv[2]) && extra.includes('status=done') && /^- tokens: open\s*$/m.test(s) && !extra.some(x => x.startsWith('tokens='))) {
  throw new Error('refusing to close ' + process.argv[2] + ': tokens is still open. Run record-cost.js (task-next.js does), or pass tokens=<why it cannot be measured>.');
}
const st = (process.argv.find(a => a.startsWith('status=')) || '').slice(7);
const globalTask = process.argv[2].match(/^(task-\d+-[^/]+)\.md$/);
if (globalTask && st === 'done') {
  const taskDir = require('path').resolve(__dirname, '..', 'reference', 'tasks');
  const subSection = (s.split(/^## Subtasks\s*$/m)[1] || '').split(/^## /m)[0];
  const subtasks = [...subSection.matchAll(/^\d+\.\s+(task-\d+-\d{2}-[a-z0-9-]+)/gm)].map(m => m[1]);
  if (!subtasks.length) throw new Error(`refusing ${globalTask[1]}: it has no readable subtasks, so task closure cannot verify recorded costs`);
  const costLog = fs.readFileSync(require('path').resolve(__dirname, '..', 'reference', 'production', 'cost-log.md'), 'utf8');
  for (const subtask of subtasks) {
    const subFile = require('path').join(taskDir, globalTask[1], subtask + '.md');
    if (!fs.existsSync(subFile)) throw new Error(`refusing ${globalTask[1]}: missing subtask node ${subtask}`);
    const subText = fs.readFileSync(subFile, 'utf8');
    if (!/^status: done\s*$/m.test(subText)) throw new Error(`refusing ${globalTask[1]}: ${subtask} is not done`);
    const openFields = ['model', 'tokens', 'usage', 'runs'].filter(k => new RegExp('^- ' + k + ': open\\s*$', 'm').test(subText));
    if (openFields.length) throw new Error(`refusing ${globalTask[1]}: ${subtask} has unrecorded ${openFields.join(', ')}`);
    if (!new RegExp('^\\| \\[\\[' + subtask + '\\]\\] \\|', 'm').test(costLog)) {
      throw new Error(`refusing ${globalTask[1]}: ${subtask} has no cost-log row`);
    }
  }
}
const waitArg = extra.find(a => a.startsWith('waiting_for='));
const waitText = waitArg === undefined ? '' : waitArg.slice(12).trim();
if (st === 'wait-answers' && !waitText) throw new Error('refusing status=wait-answers on ' + process.argv[2] + ': pass waiting_for=<question or owner action>. Nothing changed.');
if (waitArg !== undefined && st !== 'wait-answers') throw new Error('refusing waiting_for= without status=wait-answers on ' + process.argv[2] + '. Nothing changed.');
for (const a of extra) {
  const i = a.indexOf('='); const k = a.slice(0, i), v = a.slice(i + 1);
  if (k === 'waiting_for') continue; // written below, inserted when missing
  if (k === 'status') s = s.replace(/^status: .*$/m, 'status: ' + v);
  // budget_grants (task-502): owner "continue" after a BUDGET stop; inserted before result when missing.
  else if (k === 'budget_grants' && !/^- budget_grants: .*$/m.test(s)) {
    if (!/^\d+$/.test(v) || !/^- result: /m.test(s)) throw new Error('budget_grants needs an integer and a result field');
    s = s.replace(/^- result: /m, () => '- budget_grants: ' + v + '\n- result: ');
  }
  else { const re = new RegExp('^- ' + k + ': .*$', 'm'); if (!re.test(s)) throw new Error('no field ' + k); s = s.replace(re, () => '- ' + k + ': ' + v); }
}
if (extra.includes('status=in-progress')) {
  const started = '- started: ' + new Date().toISOString().replace(/\.\d{3}Z$/, 'Z');
  if (/^- started: .*$/m.test(s)) s = s.replace(/^- started: .*$/m, () => started);
  else if (/^- result: /m.test(s)) s = s.replace(/^- result: /m, () => started + '\n- result: ');
  else throw new Error('no field result to insert started before');
}
if (st === 'wait-answers') {
  const waiting = '- waiting_for: ' + waitText;
  if (/^- waiting_for: .*$/m.test(s)) s = s.replace(/^- waiting_for: .*$/m, () => waiting);
  else if (/^- result: /m.test(s)) s = s.replace(/^- result: /m, () => waiting + '\n- result: ');
  else throw new Error('no field result to insert waiting_for before');
} else if (st && /^- waiting_for: (?!none\s*$).*$/m.test(s)) s = s.replace(/^- waiting_for: .*$/m, '- waiting_for: none');
const m = process.argv[2].match(/^(task-\d+-[^/]+)\.md$/);
// cancelling a global also cancels each of its subtasks that is not done and sets its next_step to none
if (m && st === 'cancelled') {
  s = s.replace(/^- next_step: .*$/m, '- next_step: none');
  const sub = require('path').resolve(__dirname, '..', 'reference', 'tasks', m[1]);
  if (fs.existsSync(sub)) for (const x of fs.readdirSync(sub).filter(n => n.endsWith('.md'))) {
    const p = require('path').join(sub, x), t = fs.readFileSync(p, 'utf8');
    if (!/^status: done\s*$/m.test(t)) fs.writeFileSync(p, t.replace(/^status: .*$/m, 'status: cancelled'));
  }
}
fs.writeFileSync(f, s);
// keep the index status line in sync for global tasks; a global task that becomes done or cancelled moves from
// tasks-to-do-index to tasks-done-index (and back if it leaves both), with its parent field and parent link.
if (m && st) {
  const dir = require('path').resolve(__dirname, '..', 'reference', 'tasks');
  const P = n => require('path').join(dir, n + '.md');
  const lineRe = new RegExp('^- \\[\\[' + m[1] + '\\]\\].*$', 'm');
  const from = ['tasks-to-do-index', 'tasks-done-index'].find(n => lineRe.test(fs.readFileSync(P(n), 'utf8')));
  if (!from) throw new Error(m[1] + ' is in neither tasks-to-do-index nor tasks-done-index');
  let src = fs.readFileSync(P(from), 'utf8');
  const line = src.match(lineRe)[0].replace(/(Status )[a-z-]+\./, '$1' + st + '.');
  const to = st === 'done' || st === 'cancelled' ?'tasks-done-index' : 'tasks-to-do-index';
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
