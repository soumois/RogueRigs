// node vault/scripts/task-next.js <done-global> <done-subtask> <runs> "<result>" [<next-global> <next-subtask>]
// Closes a finished subtask and its global task, records its cost, optionally starts the next task,
// lints the vault and commits only the vault files it touched (safe while another agent works).
// Paths are relative to vault/reference/tasks, without ".md"; e.g. task-062-alert-bar-x2 task-062-01-alert-bar-x2.
// If the global task has more subtasks left, pass the same global as <next-global> and the next subtask.
const { execFileSync } = require('child_process');
const path = require('path');
const ROOT = path.resolve(__dirname, '..', '..');
const run = (cmd, args) => execFileSync(cmd, args, { cwd: ROOT, stdio: ['ignore', 'pipe', 'inherit'] }).toString();
const node = (script, ...args) => run(process.execPath, [path.join('vault', 'scripts', script), ...args]);
const git = (...args) => run('git', ['-c', 'safe.directory=' + ROOT.replace(/\\/g, '/'), ...args]);

const [doneGlobal, doneSub, runs, result, nextGlobal, nextSub] = process.argv.slice(2);
if (!doneGlobal || !doneSub || !runs || result === undefined) {
  console.error('usage: task-next.js <done-global> <done-subtask> <runs> "<result>" [<next-global> <next-subtask>]');
  process.exit(1);
}
const costId = doneSub.split('-').slice(0, 3).join('-'); // task-NNN-SS
process.stdout.write(node('record-cost.js', costId, runs));
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
process.stdout.write(node('lint.js', '--write').split('\n').filter(Boolean).pop() + '\n');
git('commit', '-q', '-m', `${doneSub}: done; next ${nextSub || 'none'}\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`, '--', ...new Set(files));
process.stdout.write(git('log', '--oneline', '-1'));
