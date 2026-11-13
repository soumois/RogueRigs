// node vault/scripts/task-next.js <done-global> <done-subtask> <runs> "<result>" [<next-global> <next-subtask>]
// Closes a finished subtask and its global task, records its cost, optionally starts the next task,
// lints the vault and commits only the vault files it touched (safe while another agent works).
// Paths are relative to vault/reference/tasks, without ".md"; e.g. task-062-alert-bar-x2 task-062-01-alert-bar-x2.
// If the global task has more subtasks left, pass the same global as <next-global> and the next subtask.
// Refuse before writing (task-538): every check that can refuse runs before the first write (cost row, status,
// next_step, index, commit), so a refused call changes nothing and prints `Nothing changed.`:
//   - a listed subtask of <done-global> other than <done-subtask> is still open and <next-global> <next-subtask> are
//     missing or name another global: the message names the missing arguments and prints the command with the next
//     open subtask of the global (no guess, owner Q1);
//   - a `[[id]]` link in the result text (outside backticks, as lint reads it) names no vault node (owner Q2);
//   - the closing of the global would be refused by task-status.js (another listed subtask not done, unrecorded
//     model/tokens/usage/runs, no cost-log row);
//   - a node, field or index line that task-status.js needs is missing; <next-subtask> is unknown, not listed in its
//     global, the done subtask itself, or already done or cancelled.
// Rerun after a late failure (owner Q3): a failure that can only appear after the writes (lint problem in another file,
// hook or commit refused) leaves the writes in place; the same command, rerun, skips each step already applied
// (cost recorded when the subtask is done with its cost-log row; status, next_step and in-progress already set) and
// lints and commits the rest: one cost row, no second status change. A rerun on a close already committed prints
// `<done-subtask>: already closed; nothing to commit.` and the next line.
// Output (task-327): only lines that need action. Children run through task-quiet.js: the record-cost.js summary
// line, lint `warn:` and `vault OK` lines, git line-ending warnings and Node stacks are dropped; ALERT lines,
// refusals, record-cost.js errors, lint problem lines and hook or commit failures
// (child's own text, non-zero exit) are printed. On success: the `git log --oneline -1` line,
// `left: <git status --porcelain line>` for each committed file still modified or untracked, then last
// `next: <absolute path of <next-global>/<next-subtask>.md>`, or `next: none` when no next subtask was given.
const fs = require('fs');
const path = require('path');
const { ROOT, node, git, gitData, leftovers, nextLine } = require('./task-quiet');
const { idMap } = require('./path-of');
const TASKS = path.join(ROOT, 'vault', 'reference', 'tasks');
const COST_LOG = path.join(ROOT, 'vault', 'reference', 'production', 'cost-log.md');
const refuse = msg => { console.error('task-next: refusing: ' + msg + (msg.endsWith('\n') ? '' : ' ') + 'Nothing changed.'); process.exit(1); };

const [doneGlobal, doneSub, runs, result, nextGlobal, nextSub] = process.argv.slice(2);
if (!doneGlobal || !doneSub || !runs || result === undefined) {
  console.error('usage: task-next.js <done-global> <done-subtask> <runs> "<result>" [<next-global> <next-subtask>]');
  process.exit(1);
}

// ---------- checks, before any write ----------
const read = f => fs.readFileSync(f, 'utf8');
const status = s => ((s.match(/^status: (.*)$/m) || [])[1] || '').trim();
const hasField = (s, k) => new RegExp('^- ' + k + ': ', 'm').test(s);
const field = (s, k) => ((s.match(new RegExp('^- ' + k + ': (.*)$', 'm')) || [])[1] || '').trim();
const globalFile = g => path.join(TASKS, g + '.md');
const subFile = (g, id) => path.join(TASKS, g, id + '.md');
const subsOf = s => [...((s.split(/^## Subtasks\s*$/m)[1] || '').split(/^## /m)[0]).matchAll(/^\d+\.\s+(task-\d+-\d+-[a-z0-9-]+)(?=\s|$)/gm)].map(m => m[1]);
const indexed = g => ['tasks-to-do-index', 'tasks-done-index'].some(n => new RegExp('^- \\[\\[' + g + '\\]\\]', 'm').test(read(path.join(TASKS, n + '.md'))));
const costLog = read(COST_LOG);
const hasRow = id => new RegExp('^\\| \\[\\[' + id + '\\]\\] \\|', 'm').test(costLog);

if (!fs.existsSync(globalFile(doneGlobal))) refuse(`no global task node ${doneGlobal}.md.`);
if (!fs.existsSync(subFile(doneGlobal, doneSub))) refuse(`no subtask node ${doneGlobal}/${doneSub}.md.`);
const g = read(globalFile(doneGlobal)), sub = read(subFile(doneGlobal, doneSub));
if (!hasField(sub, 'result')) refuse(`${doneSub} has no "- result:" field.`);
if (!hasField(g, 'next_step') || !hasField(g, 'result')) refuse(`${doneGlobal} lacks a "- next_step:" or "- result:" field.`);
if (!indexed(doneGlobal)) refuse(`${doneGlobal} is in neither tasks-to-do-index nor tasks-done-index.`);

// Q2: a link of the result text must name a vault node (code spans stripped, as lint.js reads a body)
const ids = idMap();
const dead = [...new Set([...result.replace(/`[^`]*`/g, '').matchAll(/\[\[([^\]]+)\]\]/g)].map(m => m[1].split('|')[0].split('#')[0].trim()).filter(t => t && !Object.prototype.hasOwnProperty.call(ids, t)))];
if (dead.length) refuse(`the result text links ${dead.map(t => '[[' + t + ']]').join(', ')}, no vault node by that id (lint would fail the commit); write the id without brackets or link an existing node.`);

// Q1: the global keeps open subtasks, so the call must name the next one of this global
const subs = subsOf(g);
const subStatus = id => fs.existsSync(subFile(doneGlobal, id)) ? status(read(subFile(doneGlobal, id))) : null;
const open = subs.filter(id => id !== doneSub && !['done', 'cancelled'].includes(subStatus(id)));
if (open.length && nextGlobal !== doneGlobal) {
  const missing = !nextGlobal ? '<next-global> <next-subtask>' : !nextSub ? '<next-subtask>' : null;
  const what = missing ? `the arguments ${missing} are missing` : `<next-global> ${nextGlobal} is not ${doneGlobal}`;
  refuse(`${doneGlobal} still has open subtask(s) ${open.join(', ')}, and ${what}. Run:\n  node vault/scripts/task-next.js ${doneGlobal} ${doneSub} ${runs} "<result>" ${doneGlobal} ${open[0]}\n`);
}
if (nextGlobal && !nextSub) refuse('<next-global> is given without <next-subtask>.');
if (nextGlobal === doneGlobal) {
  if (nextSub === doneSub) refuse(`<next-subtask> is the done subtask ${doneSub}; name the next open one${open.length ? ' (' + open[0] + ')' : ''}.`);
  if (!subs.includes(nextSub)) refuse(`${nextSub} is not listed under "## Subtasks" of ${doneGlobal}${open.length ? '; next open subtask: ' + open[0] : ''}.`);
}
if (nextGlobal && nextSub) {
  if (!fs.existsSync(globalFile(nextGlobal))) refuse(`no global task node ${nextGlobal}.md.`);
  if (!fs.existsSync(subFile(nextGlobal, nextSub))) refuse(`no subtask node ${nextGlobal}/${nextSub}.md.`);
  const ns = status(read(subFile(nextGlobal, nextSub)));
  if (ns === 'done' || ns === 'cancelled') refuse(`next subtask ${nextSub} is already ${ns}.`);
  if (!hasField(read(subFile(nextGlobal, nextSub)), 'result')) refuse(`${nextSub} has no "- result:" field.`);
  if (nextGlobal !== doneGlobal) {
    if (!hasField(read(globalFile(nextGlobal)), 'result')) refuse(`${nextGlobal} has no "- result:" field.`);
    if (!indexed(nextGlobal)) refuse(`${nextGlobal} is in neither tasks-to-do-index nor tasks-done-index.`);
  }
}
// A3: the closing gate of task-status.js, checked now for the subtasks this call does not record
const closing = nextGlobal !== doneGlobal;
if (closing && status(g) !== 'done') {
  if (!subs.length) refuse(`${doneGlobal} has no readable subtasks, so its closure cannot verify recorded costs.`);
  for (const id of subs.filter(x => x !== doneSub)) {
    if (subStatus(id) === null) refuse(`closing ${doneGlobal}: missing subtask node ${id}.`);
    if (subStatus(id) !== 'done') refuse(`closing ${doneGlobal}: ${id} is not done.`);
    const t = read(subFile(doneGlobal, id));
    const openFields = ['model', 'tokens', 'usage', 'runs'].filter(k => new RegExp('^- ' + k + ': open\\s*$', 'm').test(t));
    if (openFields.length) refuse(`closing ${doneGlobal}: ${id} has unrecorded ${openFields.join(', ')}.`);
    if (!hasRow(id)) refuse(`closing ${doneGlobal}: ${id} has no cost-log row.`);
  }
}

// ---------- writes, each skipped when a previous run already applied it (Q3) ----------
const resumed = status(sub) === 'done' && hasRow(doneSub);
if (!resumed) {
  const costId = doneSub.split('-').slice(0, 3).join('-'); // task-NNN-SS
  node('record-cost.js', costId, runs);
  node('task-status.js', `${doneGlobal}/${doneSub}.md`, 'status=done', `result=${result}`);
}
const files = ['vault/_paths.md', 'vault/reference/production/cost-log.md', 'vault/reference/tasks/tasks-to-do-index.md', 'vault/reference/tasks/tasks-done-index.md',
  `vault/reference/tasks/${doneGlobal}.md`, `vault/reference/tasks/${doneGlobal}/${doneSub}.md`];
const g2 = read(globalFile(doneGlobal));
if (!closing) {
  if (field(g2, 'next_step') !== nextSub) node('task-status.js', `${doneGlobal}.md`, `next_step=${nextSub}`);
} else if (status(g2) !== 'done') {
  node('task-status.js', `${doneGlobal}.md`, 'status=done', 'next_step=none', `result=Done. ${result}`);
}
if (nextGlobal && nextSub) {
  if (nextGlobal !== doneGlobal && status(read(globalFile(nextGlobal))) !== 'in-progress') node('task-status.js', `${nextGlobal}.md`, 'status=in-progress');
  if (status(read(subFile(nextGlobal, nextSub))) !== 'in-progress') node('task-status.js', `${nextGlobal}/${nextSub}.md`, 'status=in-progress');
  files.push(`vault/reference/tasks/${nextGlobal}.md`, `vault/reference/tasks/${nextGlobal}/${nextSub}.md`);
}
node('lint.js', '--write');
const committed = [...new Set(files)];
const nextAbs = nextGlobal && nextSub ? subFile(nextGlobal, nextSub) : null;
if (!gitData('status', '--porcelain', '--', ...committed).trim()) {
  console.log(`${doneSub}: already closed; nothing to commit.`);
  nextLine(nextAbs);
  process.exit(0);
}
// a node not yet tracked fails a commit by path: add it by name first
for (const f of committed) if (fs.existsSync(path.join(ROOT, f)) && !gitData('ls-files', '--', f).trim()) git('add', '--', f);
git('commit', '-q', '-m', `${doneSub}: done; next ${nextSub || 'none'}\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`, '--', ...committed);
process.stdout.write(gitData('log', '--oneline', '-1'));
leftovers(committed);
nextLine(nextAbs);
