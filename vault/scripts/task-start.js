// node vault/scripts/task-start.js <global-id>
// Starts a confirmed global task: sets the global and its current subtask in-progress and the global
// next_step to that subtask, through task-status.js only (index line kept in sync), then lints the vault
// and commits only the vault files it touched (safe while another agent works).
// <global-id> is relative to vault/reference/tasks, without ".md"; e.g. task-320-task-start-script.
// Subtask picked: the global next_step if it names a subtask that is not done, else the first subtask of
// "## Subtasks" whose status is todo. Refuses a global without "confirmed_by_owner: yes" and a global with
// no subtask left. Idempotent: when everything is already set it changes nothing and exits 0.
// Later subtasks of the same global are started by task-next.js when the previous one closes.
const { execFileSync } = require('child_process');
const fs = require('fs');
const path = require('path');
const ROOT = path.resolve(__dirname, '..', '..');
const TASKS = path.join(ROOT, 'vault', 'reference', 'tasks');
const run = (cmd, args) => execFileSync(cmd, args, { cwd: ROOT, stdio: ['ignore', 'pipe', 'inherit'] }).toString();
const node = (script, ...args) => run(process.execPath, [path.join('vault', 'scripts', script), ...args]);
const git = (...args) => run('git', ['-c', 'safe.directory=' + ROOT.replace(/\\/g, '/'), ...args]);
const fail = msg => { console.error('task-start: ' + msg); process.exit(1); };

const globalId = (process.argv[2] || '').replace(/\.md$/, '');
if (!globalId) fail('usage: task-start.js <global-id>');
const globalFile = path.join(TASKS, globalId + '.md');
if (!fs.existsSync(globalFile)) fail('no global task node ' + globalFile);
const g = fs.readFileSync(globalFile, 'utf8');
const field = (s, k) => ((s.match(new RegExp('^- ' + k + ': (.*)$', 'm')) || [])[1] || '').trim();
const status = s => ((s.match(/^status: (.*)$/m) || [])[1] || '').trim();

// Q7: only an owner-confirmed global is started
if (!/^yes\b/.test(field(g, 'confirmed_by_owner'))) fail(`refusing ${globalId}: confirmed_by_owner is not "yes" (${field(g, 'confirmed_by_owner') || 'missing'}). Nothing changed.`);
if (status(g) === 'done') fail(`refusing ${globalId}: the global task is done. Nothing changed.`);

const subFile = id => path.join(TASKS, globalId, id + '.md');
const subStatus = id => fs.existsSync(subFile(id)) ? status(fs.readFileSync(subFile(id), 'utf8')) : null;
const section = (g.split(/^## Subtasks\s*$/m)[1] || '').split(/^## /m)[0];
const subs = [...section.matchAll(/^\d+\.\s+(task-\d+-\d+-[a-z0-9-]+)/gm)].map(m => m[1]);
const next = field(g, 'next_step');
let sub = null;
if (/^task-\d+-\d+-/.test(next) && subStatus(next) !== null && subStatus(next) !== 'done') sub = next;
else sub = subs.find(id => subStatus(id) === 'todo') || null;
if (!sub) fail(`refusing ${globalId}: no subtask left to start (next_step "${next}", none of ${subs.join(', ') || 'no listed subtask'} is todo). Nothing changed.`);

// set only what differs, so a second run touches nothing
const subRel = `${globalId}/${sub}.md`;
let changed = false;
if (status(g) !== 'in-progress') { node('task-status.js', `${globalId}.md`, 'status=in-progress'); changed = true; }
if (subStatus(sub) !== 'in-progress') { node('task-status.js', subRel, 'status=in-progress'); changed = true; }
if (field(fs.readFileSync(globalFile, 'utf8'), 'next_step') !== sub) { node('task-status.js', `${globalId}.md`, `next_step=${sub}`); changed = true; }
if (!changed) { console.log(`${globalId}: already in-progress at ${sub}; nothing to do.`); process.exit(0); }

process.stdout.write(node('lint.js', '--write').split('\n').filter(Boolean).pop() + '\n');
const files = ['vault/_paths.md', 'vault/reference/tasks/tasks-to-do-index.md', 'vault/reference/tasks/tasks-done-index.md',
  `vault/reference/tasks/${globalId}.md`, `vault/reference/tasks/${subRel}`];
// a node not yet tracked fails a commit by path: add it by name first
for (const f of files) if (!git('ls-files', '--', f).trim()) git('add', '--', f);
git('commit', '-q', '-m', `${sub}: in-progress\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`, '--', ...files);
process.stdout.write(git('log', '--oneline', '-1'));
