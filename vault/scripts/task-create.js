// node vault/scripts/task-create.js <spec.js> <index-anchor-task-id>
// Creates global tasks with one subtask each from a spec module, and adds them to tasks-to-do-index after the
// line of <index-anchor-task-id> (the last listed task; if it is not in tasks-to-do-index, at the end of the list). Input ids are resolved to paths through _paths.md,
// so delegations can hand agents direct paths. Does not commit; lint and commit by name afterwards.
// Spec: module.exports = [ [num, slug, desc, agent, title, request, [input ids], expected_output, done_when,
//        editor_state, token_ceiling, confirmed_by_owner (optional, default "yes (<today>)")], ... ]
// token_ceiling is written as-is: give it as "<x> M units" (weighted units of record-cost.js, e.g. "1.2 M units") or "open";
// record-cost.js ignores any other form (legacy integer or k) for its ceiling alert.
// The global's "## Subtasks" line gives the subtask's repo-relative path (task-327):
// `1. task-NNN-01-slug (vault/reference/tasks/task-NNN-slug/task-NNN-01-slug.md) — <agent>: <desc>`; task-start.js parses it.
// For a second subtask (another agent), write its node by hand from vault/templates/task.md, its Subtasks line in the same form.
const fs = require('fs'), path = require('path');
const V = path.resolve(__dirname, '..');
const T = path.join(V, 'reference', 'tasks');
const [specFile, anchor] = process.argv.slice(2);
if (!specFile || !anchor) { console.error('usage: task-create.js <spec.js> <index-anchor-task-id>'); process.exit(1); }
const P = {};
for (const l of fs.readFileSync(path.join(V, '_paths.md'), 'utf8').split('\n')) {
  const m = l.match(/^(\S+)\s+(reference\/\S+\.md)/); if (m) P[m[1]] = m[2];
}
const res = ids => ids.map(i => { if (!P[i]) throw new Error('no path for ' + i); return `${i} (vault/${P[i]})`; }).join(', ');
const today = new Date().toISOString().slice(0, 10);
const IDX = path.join(T, 'tasks-to-do-index.md');
let idx = fs.readFileSync(IDX, 'utf8');
let add = '';
for (const [num, slug, desc, agent, title, req, inputs, out, done, ed, ceil, confirmed] of require(path.resolve(specFile))) {
  const gid = `task-${num}-${slug}`, sid = `task-${num}-01-${slug}`;
  const conf = confirmed || `yes (${today})`;
  fs.writeFileSync(path.join(T, gid + '.md'), ['---', `id: ${gid}`, `description: "Global task: ${desc}"`, 'parent: tasks-to-do-index',
    'kind: task', 'status: todo', '---', `# ${title}`, '', `- request: ${req} (owner, ${today})`, `- goal: ${desc}`,
    `- acceptance: ${done.replace(/\.$/, '')}; committed.`, `- gdd_check: done by the Orchestrator when the task is planned (${today}).`,
    `- confirmed_by_owner: ${conf}`, '- milestone: M2', `- next_step: ${sid}`, '- started: open', '- waiting_for: none', '- result: open', '', '## Subtasks', `1. ${sid} (vault/reference/tasks/${gid}/${sid}.md) — ${agent}: ${desc}`, '',
    '## Relations', '- [[tasks-to-do-index]] — parent index.', `- [[${sid}]] — step 1, ${agent}.`, ''].join('\n'));
  fs.mkdirSync(path.join(T, gid), { recursive: true });
  fs.writeFileSync(path.join(T, gid, sid + '.md'), ['---', `id: ${sid}`, `description: "${desc}"`, `parent: ${gid}`, 'kind: task',
    'status: todo', '---', `# ${title}`, '', `- goal: ${desc}`, `- agent: ${agent}`, '- depends_on: none', `- inputs: ${res(inputs)}`,
    `- expected_output: ${out}`, `- done_when: ${done}`, `- editor_state: ${ed}`, `- token_ceiling: ${ceil}`, '- model: open',
    '- tokens: open', '- usage: open', '- runs: open', '- milestone: M2', '- started: open', '- waiting_for: none', '- result: open', '', '## Relations', `- [[${gid}]] — parent global task.`, ''].join('\n'));
  add += `\n- [[${gid}]] — ${desc.replace(/\.$/, '')}. Status todo.`;
  console.log('created', gid);
}
const re = new RegExp(`(- \\[\\[${anchor}\\]\\][^\\n]*)`);
if (re.test(idx)) idx = idx.replace(re, m => m + add);
else idx = idx.replace(/\n*$/, '') + add + '\n';
fs.writeFileSync(IDX, idx);
