// node vault/scripts/task-create.js <spec.js> [<index-anchor-task-id>]
// Writes the task nodes of one planning pass from a spec module: global nodes, every subtask node, their
// `## Subtasks` and `## Relations` lines, and one line per global in tasks-to-do-index, inserted after the line of
// <index-anchor-task-id> (or the spec's `anchor`); without an anchor, or when the anchor is not in tasks-to-do-index,
// at the end of the list. Ids in inputs are resolved to paths through path-of.js (exact ids, task ids and the ids
// created by the same spec included), so delegations hand agents direct paths. Refuses, before writing anything,
// a spec whose node already exists, an unknown id, a missing field, a missing or unknown level, wait-answers without waiting_for.
// Does not lint or commit: run `node vault/scripts/lint.js --write`, `git add` the printed files, commit by path.
// Output: `created <global-id> (<n> subtasks)` per global, `defaults: <path>` when a defaults file is written,
// last `files: <repo-relative paths>` (every file written, for git add / commit).
//
// Spec v2 (task-358, Q3): module.exports = { anchor?, edits?, globals: [ <global>, ... ] }
//   global: { num, slug, title, description, level ("light" | "standard" | "exploratory", written `- verification_level:`),
//     family? ("reglage" | "comportement" | "visuel" | "gros", request family of the relay, written `- family:`, default "open"),
//     request, goal, acceptance, gdd_check,
//     confirmed_by_owner? (default "no (plan and questions handed back <today>)"), milestone? ("M2"),
//     status? ("todo" | "wait-answers", default "todo"), waiting_for? (required with wait-answers),
//     questions? [ "Q1 ... Default: ...", ... ] (written as `- ` lines under `## Questions`),
//     questions_header? ("Asked <today>, each with a default; open until the owner answers."),
//     relations? [ [id, gloss], ... ] (extra Relations lines), index_line? (default description),
//     subtasks: [ <subtask>, ... ] (ordered; step numbers 01, 02, ... follow the order) }
//   subtask: { slug (after task-NNN-SS-), agent, line (one-line gloss for ## Subtasks), inputs [...], done_when,
//     goal? (default line), expected_output? (default "see done_when"), editor_state? (default "decided by the Executor"),
//     token_ceiling? ("<x> M units", default "open"),
//     title? (default "<global title>: <agent>"), description? (default "<Agent>: <line>"),
//     depends_on? (ids, step numbers, or "none"; default "none" for step 1, the previous subtask otherwise),
//     milestone? (the global's) }
//   input item: a bare id (one word of letters, digits, `_`, `-`; `agent-cpp`) -> `agent-cpp (vault/<path>)`, refused
//     if unknown (a literal file name has a `.` or `/`, so it is never read as an id); `<id> <text>` whose first
//     word is a known id -> `<id> (vault/<path>) <text>`; anything else (a literal path, a sentence) is written as-is.
//   edits?: [ { file, old, new } ] exact replacements (repo-relative file) that the defaults of the questions imply.
//   When a global is wait-answers, Saved/Planner/<first-global-id>.defaults.json is written for task-confirm.js:
//   { globals: [ { id, questions: ["Q1", ...] } ], edits } (questions = the Q ids of its `questions` lines).
//
// Old form (still accepted): module.exports = [ [num, slug, desc, agent, title, request, [input ids], expected_output,
//   done_when, editor_state, token_ceiling, confirmed_by_owner (null for the default "yes (<today>)"), level], ... ]
//   one global with one subtask `task-NNN-01-<slug>` each, status todo; the anchor argument is required.
// token_ceiling is written as-is: give it as "<x> M units" (weighted units of record-cost.js, e.g. "1.2 M units") or
// "open"; record-cost.js ignores any other form for its ceiling alert.
// The global's "## Subtasks" line gives the subtask's repo-relative path (task-327):
// `1. task-NNN-01-slug (vault/reference/tasks/task-NNN-slug/task-NNN-01-slug.md) — <agent>: <line>`; task-start.js parses it.
const fs = require('fs'), path = require('path');
const { idMap } = require('./path-of');
const V = path.resolve(__dirname, '..');
const ROOT = path.resolve(V, '..');
const T = path.join(V, 'reference', 'tasks');
const fail = msg => { console.error('task-create: ' + msg); process.exit(1); };
const LEVELS = ['light', 'standard', 'exploratory'];
const checkLevel = (level, who) => { if (!LEVELS.includes(level)) fail(`${who}: level must be ${LEVELS.join(', ')}, not ${level === undefined ? 'missing' : level}. Nothing written.`); };
const [specFile, anchorArg] = process.argv.slice(2);
if (!specFile) fail('usage: task-create.js <spec.js> [<index-anchor-task-id>]');
const spec = require(path.resolve(specFile));
const today = new Date().toISOString().slice(0, 10);
const P = idMap();
const rel = abs => path.relative(ROOT, abs).split(path.sep).join('/');
const IDX = path.join(T, 'tasks-to-do-index.md');
const writes = []; // [abs, content], written only once the whole spec is valid
const idxLines = [];
let defaults = null;

const isOld = Array.isArray(spec) && spec.every(Array.isArray);
const anchor = anchorArg || (!isOld && spec.anchor) || null;
if (isOld && !anchor) fail('usage: task-create.js <spec.js> <index-anchor-task-id> (required with the array form)');

if (isOld) {
  const res = ids => ids.map(i => { if (!P[i]) fail('no path for ' + i); return `${i} (${P[i]})`; }).join(', ');
  for (const [num, slug, desc, agent, title, req, inputs, out, done, ed, ceil, confirmed, level] of spec) {
    const gid = `task-${num}-${slug}`, sid = `task-${num}-01-${slug}`;
    checkLevel(level, gid);
    const conf = confirmed || `yes (${today})`;
    writes.push([path.join(T, gid + '.md'), ['---', `id: ${gid}`, `description: "Global task: ${desc}"`, 'parent: tasks-to-do-index',
      'kind: task', 'status: todo', '---', `# ${title}`, '', `- request: ${req} (owner, ${today})`, `- goal: ${desc}`,
      `- acceptance: ${done.replace(/\.$/, '')}; committed.`, `- gdd_check: done by the Orchestrator when the task is planned (${today}).`,
      `- verification_level: ${level}`, `- confirmed_by_owner: ${conf}`, '- milestone: M2', `- next_step: ${sid}`, '- started: open', '- waiting_for: none', '- result: open', '', '## Subtasks', `1. ${sid} (vault/reference/tasks/${gid}/${sid}.md) — ${agent}: ${desc}`, '',
      '## Relations', '- [[tasks-to-do-index]] — parent index.', `- [[${sid}]] — step 1, ${agent}.`, ''].join('\n')]);
    writes.push([path.join(T, gid, sid + '.md'), ['---', `id: ${sid}`, `description: "${desc}"`, `parent: ${gid}`, 'kind: task',
      'status: todo', '---', `# ${title}`, '', `- goal: ${desc}`, `- agent: ${agent}`, '- depends_on: none', `- inputs: ${res(inputs)}`,
      `- expected_output: ${out}`, `- done_when: ${done}`, `- editor_state: ${ed}`, `- token_ceiling: ${ceil}`, '- model: open',
      '- tokens: open', '- usage: open', '- runs: open', '- milestone: M2', '- started: open', '- waiting_for: none', '- result: open', '', '## Relations', `- [[${gid}]] — parent global task.`, ''].join('\n')]);
    idxLines.push([gid, `- [[${gid}]] — ${desc.replace(/\.$/, '')}. Status todo.`, 1]);
  }
} else {
  const globals = Array.isArray(spec) ? spec : spec.globals;
  if (!Array.isArray(globals) || !globals.length) fail('spec v2 needs globals: [ ... ]');
  const ids = g => ({ gid: `task-${g.num}-${g.slug}`, sids: (g.subtasks || []).map((s, i) => `task-${g.num}-${String(i + 1).padStart(2, '0')}-${s.slug}`) });
  // ids created by this spec resolve like existing ones (a doc subtask may take its cpp sibling as input)
  for (const g of globals) {
    const { gid, sids } = ids(g);
    P[gid] = `vault/reference/tasks/${gid}.md`;
    sids.forEach(sid => { P[sid] = `vault/reference/tasks/${gid}/${sid}.md`; });
  }
  const need = (o, keys, who) => { for (const k of keys) if (o[k] === undefined || o[k] === null || o[k] === '') fail(`${who}: missing ${k}`); };
  const input = (x, who) => {
    const s = String(x).trim();
    if (s !== 'none' && /^[A-Za-z0-9_][\w-]*$/.test(s)) { if (!P[s]) fail(`${who}: unknown id ${s} in inputs`); return `${s} (${P[s]})`; }
    const m = s.match(/^(\S+)\s+([\s\S]+)$/);
    if (m && P[m[1]]) return `${m[1]} (${P[m[1]]}) ${m[2]}`;
    return s;
  };
  const cap = s => s.charAt(0).toUpperCase() + s.slice(1);
  const defGlobals = [];
  for (const g of globals) {
    const { gid, sids } = ids(g);
    need(g, ['num', 'slug', 'title', 'description', 'request', 'goal', 'acceptance', 'gdd_check'], gid);
    checkLevel(g.level, gid);
    if (!Array.isArray(g.subtasks) || !g.subtasks.length) fail(`${gid}: needs at least one subtask`);
    const status = g.status || 'todo';
    if (!['todo', 'wait-answers'].includes(status)) fail(`${gid}: status must be todo or wait-answers, not ${status}`);
    if (status === 'wait-answers' && !g.waiting_for) fail(`${gid}: status wait-answers needs waiting_for`);
    const milestone = g.milestone || 'M2';
    const qs = (g.questions || []).map(q => String(q).replace(/^- /, ''));
    const desc = String(g.description).replace(/^Global task: /, '');
    const subLines = [], relLines = [];
    g.subtasks.forEach((s, i) => {
      const sid = sids[i];
      need(s, ['slug', 'agent', 'line', 'inputs', 'done_when'], sid);
      if (!Array.isArray(s.inputs)) fail(`${sid}: inputs must be a list`);
      let dep = s.depends_on;
      if (dep === undefined) dep = i ? [sids[i - 1]] : 'none';
      if (dep === 'none' || (Array.isArray(dep) && !dep.length)) dep = 'none';
      else dep = [].concat(dep).map(d => {
        const id = typeof d === 'number' ? sids[d - 1] : String(d);
        if (!id || !P[id]) fail(`${sid}: unknown depends_on ${d}`);
        return id;
      }).join(', ');
      const sdesc = s.description || `${cap(s.agent)}: ${s.line}`;
      writes.push([path.join(T, gid, sid + '.md'), ['---', `id: ${sid}`, `description: "${sdesc}"`, `parent: ${gid}`, 'kind: task',
        'status: todo', '---', `# ${s.title || g.title + ': ' + s.agent}`, '', `- goal: ${s.goal || s.line}`, `- agent: ${s.agent}`, `- depends_on: ${dep}`,
        `- inputs: ${s.inputs.map(x => input(x, sid)).join(', ')}`, `- expected_output: ${s.expected_output || 'see done_when'}`, `- done_when: ${s.done_when}`,
        `- editor_state: ${s.editor_state || 'decided by the Executor'}`, `- token_ceiling: ${s.token_ceiling || 'open'}`, '- model: open', '- tokens: open', '- usage: open',
        '- runs: open', `- milestone: ${s.milestone || milestone}`, '- started: open', '- waiting_for: none', '- result: open', '',
        '## Relations', `- [[${gid}]] — parent global task.`, ''].join('\n')]);
      subLines.push(`${i + 1}. ${sid} (vault/reference/tasks/${gid}/${sid}.md) — ${s.agent}: ${s.line}`);
      relLines.push(`- [[${sid}]] — step ${i + 1}, ${s.agent}.`);
    });
    for (const [id, gloss] of g.relations || []) {
      if (!P[id]) fail(`${gid}: unknown id ${id} in relations`);
      relLines.push(`- [[${id}]] — ${gloss}`);
    }
    const body = ['---', `id: ${gid}`, `description: "Global task: ${desc}"`, 'parent: tasks-to-do-index', 'kind: task', `status: ${status}`, '---',
      `# ${g.title}`, '', `- request: ${g.request}`, `- goal: ${g.goal}`, `- acceptance: ${g.acceptance}`, `- gdd_check: ${g.gdd_check}`,
      `- verification_level: ${g.level}`, `- family: ${g.family || 'open'}`, `- confirmed_by_owner: ${g.confirmed_by_owner || `no (plan and questions handed back ${today})`}`, `- milestone: ${milestone}`,
      `- next_step: ${sids[0]}`, '- started: open', `- waiting_for: ${status === 'wait-answers' ? g.waiting_for : 'none'}`, '- result: open', ''];
    if (qs.length) body.push('## Questions', g.questions_header || `Asked ${today}, each with a default; open until the owner answers.`, ...qs.map(q => '- ' + q), '');
    body.push('## Subtasks', ...subLines, '', '## Relations', '- [[tasks-to-do-index]] — parent index.', ...relLines, '');
    writes.push([path.join(T, gid + '.md'), body.join('\n')]);
    idxLines.push([gid, `- [[${gid}]] — ${String(g.index_line || desc).replace(/\.$/, '')}. Status ${status}.`, sids.length]);
    if (status === 'wait-answers') defGlobals.push({ id: gid, questions: qs.map(q => (q.match(/^(Q\d+)\b/) || [])[1]).filter(Boolean) });
  }
  const edits = spec.edits || [];
  for (const e of edits) {
    need(e, ['file', 'old'], 'edit');
    if (e.new === undefined) fail(`edit of ${e.file}: missing new`);
    const f = path.join(ROOT, e.file);
    if (!fs.existsSync(f)) fail(`edit: no file ${e.file}`);
    if (!fs.readFileSync(f, 'utf8').replace(/\r\n/g, '\n').includes(e.old)) fail(`edit of ${e.file}: old text not found`);
  }
  if (defGlobals.length) defaults = [path.join(ROOT, 'Saved', 'Planner', defGlobals[0].id + '.defaults.json'), JSON.stringify({ globals: defGlobals, edits }, null, 2) + '\n'];
}

// refuse before writing anything when a node already exists
for (const [f] of writes) if (fs.existsSync(f)) fail(`refusing: ${rel(f)} already exists. Nothing written.`);
for (const [f, s] of writes) { fs.mkdirSync(path.dirname(f), { recursive: true }); fs.writeFileSync(f, s); }
let idx = fs.readFileSync(IDX, 'utf8');
const add = idxLines.map(([, l]) => '\n' + l).join('');
const re = anchor && new RegExp(`(- \\[\\[${anchor}\\]\\][^\\n]*)`);
if (re && re.test(idx)) idx = idx.replace(re, m => m + add);
else idx = idx.replace(/\n*$/, '') + add + '\n';
fs.writeFileSync(IDX, idx);
for (const [gid, , n] of idxLines) console.log(`created ${gid} (${n} subtask${n > 1 ? 's' : ''})`);
if (defaults) {
  fs.mkdirSync(path.dirname(defaults[0]), { recursive: true });
  fs.writeFileSync(defaults[0], defaults[1]);
  console.log('defaults: ' + rel(defaults[0]));
}
console.log('files: ' + [...writes.map(([f]) => rel(f)), rel(IDX)].join(' '));
