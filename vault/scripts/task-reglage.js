// node vault/scripts/task-reglage.js "<owner words verbatim>" [--slug <kebab-slug>] [--no-commit]
// Réglage family (task-500, request-families): writes the global task node of a setting change that runs without a planner,
// BEFORE the executor starts (owner Q2 2026-11-11), so resume, cost recording and the closing gate work as for a planned task.
// The relay runs it, then launches one fresh executor whose prompt names the printed global id first (agent-usage.js ties
// the executor run to the first task number of its prompt).
// Node written through task-create.js (spec v2): next free task number, level light, family reglage, the owner words as
// `request`, confirmed by the request itself, one subtask `task-NNN-01-<slug>` with agent `executor` (the executor applies
// the role rules the change touches). The slug defaults to the first words of the request, ASCII kebab-case.
// Then lints (`lint.js --write`), `git add` and commits the written files by path, unless --no-commit (sample runs).
// Output: task-create lines, the commit line, then `id: <global-id>` and last `next: <absolute path of the global node>`.
const fs = require('fs'), path = require('path');
const { ROOT, node, git } = require('./task-quiet');
const T = path.join(ROOT, 'vault', 'reference', 'tasks');
const fail = msg => { console.error('task-reglage: ' + msg); process.exit(1); };

const args = process.argv.slice(2);
const noCommit = args.includes('--no-commit');
const si = args.indexOf('--slug');
const slugArg = si >= 0 ? args[si + 1] : null;
const words = args.filter((a, i) => a !== '--no-commit' && (si < 0 || (i !== si && i !== si + 1)))[0];
if (!words || !words.trim()) fail('usage: task-reglage.js "<owner words verbatim>" [--slug <kebab-slug>] [--no-commit]');
if (slugArg && !/^[a-z0-9]+(-[a-z0-9]+)*$/.test(slugArg)) fail('slug must be ASCII kebab-case: ' + slugArg);

// next free number: highest task-NNN among the nodes and folders of vault/reference/tasks, plus one
const nums = fs.readdirSync(T).map(f => (f.match(/^task-(\d{3})-/) || [])[1]).filter(Boolean).map(Number);
const num = String((nums.length ? Math.max(...nums) : 0) + 1).padStart(3, '0');

const ascii = s => s.normalize('NFD').replace(/[̀-ͯ]/g, '').toLowerCase();
const STOP = new Set(['le', 'la', 'les', 'un', 'une', 'des', 'de', 'du', 'a', 'au', 'aux', 'et', 'en', 'pour', 'sur', 'the', 'to', 'of', 'and', 'on', 'in', 'mets', 'met', 'passe', 'fais']);
const slug = slugArg || ascii(words).replace(/[^a-z0-9]+/g, ' ').trim().split(' ').filter(w => w && !STOP.has(w)).slice(0, 4).join('-') || 'setting';
const today = new Date().toISOString().slice(0, 10);
const flat = words.replace(/\s+/g, ' ').trim();
const short = flat.split(' ').slice(0, 16).join(' ').replace(/"/g, "'") + (flat.split(' ').length > 16 ? ' ...' : '');

const spec = {
  globals: [{
    num, slug, family: 'reglage', level: 'light',
    title: 'Réglage: ' + short,
    description: 'Réglage (no planner): ' + short,
    request: `Owner ${today}, verbatim: "${flat}". Family réglage, picked or forced at the entry; node written by task-reglage.js before the executor run, no planner.`,
    goal: 'The setting the owner names is changed on the existing system, nothing else.',
    acceptance: 'A1 the change named in the request is in place, checked at light size (verification-levels); A2 no new asset, no new system, no gdd value decided by the executor; A3 lint clean, committed.',
    gdd_check: 'Not done: réglage family, no planner (request-families). The executor stops before editing on a new asset or system, an open value or a design question, and reports for a relaunch through the planner (agent-executor wrong_family).',
    confirmed_by_owner: `yes (${today}, the request itself, réglage family)`,
    relations: [['request-families', 'the family path this task follows.']],
    subtasks: [{ slug, agent: 'executor', line: 'Apply the setting change of the request and check it at light size.', inputs: ['request-families', 'verification-levels'],
      done_when: 'A1 to A3 of the global pass.' }],
  }],
};
const dir = path.join(ROOT, 'Saved', 'Planner');
fs.mkdirSync(dir, { recursive: true });
const specFile = path.join(dir, `task-${num}-${slug}.reglage-spec.js`);
fs.writeFileSync(specFile, 'module.exports = ' + JSON.stringify(spec, null, 2) + ';\n');

const out = node('task-create.js', specFile);
const files = ((out.match(/^files: (.*)$/m) || [])[1] || '').trim().split(/\s+/).filter(Boolean);
if (!files.length) fail('task-create.js printed no files line');
node('lint.js', '--write');
const gid = `task-${num}-${slug}`;
if (!noCommit) {
  const all = [...files, 'vault/_paths.md'];
  git('add', '--', ...files);
  git('commit', '-q', '-m', `${gid}: réglage node from the owner words (no planner)\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`, '--', ...all);
  git('log', '--oneline', '-1');
}
console.log('id: ' + gid);
console.log('next: ' + path.join(T, gid + '.md'));
