// node vault/scripts/task-next.js <done-global> <done-subtask> <runs> "<result>" [<next-global> <next-subtask>]
// Closes a finished subtask and its global task, records its cost, optionally starts the next task,
// lints the vault and commits only the vault files it touched (safe while another agent works).
// Paths are relative to vault/reference/tasks, without ".md"; e.g. task-062-alert-bar-x2 task-062-01-alert-bar-x2.
// If the global task has more subtasks left, pass the same global as <next-global> and the next subtask.
// Output (task-327): only lines that need action. Children run through task-quiet.js: the record-cost.js summary
// line, lint `warn:` and `vault OK` lines, git line-ending warnings and Node stacks are dropped; ALERT lines,
// refusals (e.g. `tokens is still open`), record-cost.js errors, lint problem lines and hook or commit failures
// (child's own text, non-zero exit) are printed. On success: the `git log --oneline -1` line,
// `left: <git status --porcelain line>` for each committed file still modified or untracked, `context: <n>k (<source>)` and,
// over the threshold with subtasks left, a HANDOVER line (context-now.js, task-502), then last
// `next: <absolute path of <next-global>/<next-subtask>.md>`, or `next: none` when no next subtask was given.
const path = require('path');
const { ROOT, node, git, gitData, leftovers, nextLine } = require('./task-quiet');

const [doneGlobal, doneSub, runs, result, nextGlobal, nextSub] = process.argv.slice(2);
if (!doneGlobal || !doneSub || !runs || result === undefined) {
  console.error('usage: task-next.js <done-global> <done-subtask> <runs> "<result>" [<next-global> <next-subtask>]');
  process.exit(1);
}
const costId = doneSub.split('-').slice(0, 3).join('-'); // task-NNN-SS
node('record-cost.js', costId, runs);
node('task-status.js', `${doneGlobal}/${doneSub}.md`, 'status=done', `result=${result}`);
const files = ['vault/_paths.md', 'vault/reference/production/cost-log.md', 'vault/reference/tasks/tasks-to-do-index.md', 'vault/reference/tasks/tasks-done-index.md',
  `vault/reference/tasks/${doneGlobal}.md`, `vault/reference/tasks/${doneGlobal}/${doneSub}.md`];
if (nextGlobal === doneGlobal) {
  node('task-status.js', `${doneGlobal}.md`, `next_step=${nextSub}`);
} else {
  node('task-status.js', `${doneGlobal}.md`, 'status=done', 'next_step=none', `result=Done. ${result}`);
}
if (nextGlobal && nextSub) {
  if (nextGlobal !== doneGlobal) node('task-status.js', `${nextGlobal}.md`, 'status=in-progress');
  node('task-status.js', `${nextGlobal}/${nextSub}.md`, 'status=in-progress');
  files.push(`vault/reference/tasks/${nextGlobal}.md`, `vault/reference/tasks/${nextGlobal}/${nextSub}.md`);
}
node('lint.js', '--write');
const committed = [...new Set(files)];
git('commit', '-q', '-m', `${doneSub}: done; next ${nextSub || 'none'}\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`, '--', ...committed);
process.stdout.write(gitData('log', '--oneline', '-1'));
leftovers(committed);
// task-502: executor context at this closing, and HANDOVER over the threshold when the global has subtasks left (never blocks).
for (const l of require('./context-now').handoverLines(doneGlobal, nextGlobal === doneGlobal && !!nextSub)) console.log(l);
nextLine(nextGlobal && nextSub ? path.join(ROOT, 'vault', 'reference', 'tasks', nextGlobal, nextSub + '.md') : null);
