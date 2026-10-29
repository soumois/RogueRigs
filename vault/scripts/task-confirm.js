// node vault/scripts/task-confirm.js <defaults.json> "<owner words verbatim>"
// Applies an all-defaults owner answer (a pure "ok" / "tout par défaut") to the globals a planning pass handed back,
// without resuming the planner (task-358, Q1). Run by the main session; any other answer, or a missing defaults
// file, resumes the planner instead.
// Defaults file (written by the planner at step 4, or by task-create.js spec v2 for its wait-answers globals; usually
// Saved/Planner/<first-global-id>.defaults.json, not committed):
//   { "globals": [ { "id": "task-NNN-slug", "questions": ["Q1", "Q2"] }, ... ],
//     "edits": [ { "file": "vault/reference/gdd/<node>.md", "old": "<exact text>", "new": "<text>" }, ... ] }
//   `questions` lists the Q ids whose default is accepted (omitted: every `- Qn` line of the global's ## Questions);
//   `edits` are exact replacements implied by the defaults (repo-relative file, `old` must occur exactly once).
// Per global: the `## Questions` header line becomes `Decided <date> (owner "<words>"): ...accepted; values
// provisional, owner <date>.`; ` Owner answer <date> (relayed by the main session, verbatim): "<words>" = the plan and
// the defaults of <Q ids> accepted.` is appended to `request`; `confirmed_by_owner: yes (<date>, "<words>")`; status
// todo through task-status.js (index line kept in sync, waiting_for reset to none), and each subtask in wait-answers
// back to todo. Then the edits, lint --write, and one commit of exactly the touched files.
// Refuses, before changing anything: a global missing or already confirmed, a listed Q absent from its ## Questions,
// an edit whose file is missing, whose old text is missing or occurs more than once.
// Output (as task-start.js): only action lines, the `git log --oneline -1` line, `left: <porcelain line>` per committed
// file still modified or untracked. Exit 1 on a refusal or a failing child.
const fs = require('fs');
const path = require('path');
const { ROOT, node, git, gitData, leftovers } = require('./task-quiet');
const TASKS = path.join(ROOT, 'vault', 'reference', 'tasks');
const fail = msg => { console.error('task-confirm: ' + msg); process.exit(1); };

const [defFile, words] = process.argv.slice(2);
if (!defFile || !words || !words.trim()) fail('usage: task-confirm.js <defaults.json> "<owner words verbatim>"');
if (!fs.existsSync(defFile)) fail(`no defaults file ${defFile}: resume the planner. Nothing changed.`);
let def;
try { def = JSON.parse(fs.readFileSync(defFile, 'utf8')); } catch (e) { fail(`defaults file ${defFile} is not JSON (${e.message}). Nothing changed.`); }
if (!Array.isArray(def.globals) || !def.globals.length) fail('defaults file has no globals. Nothing changed.');
const today = new Date().toISOString().slice(0, 10);
const w = words.trim();
const field = (s, k) => ((s.match(new RegExp('^- ' + k + ': (.*)$', 'm')) || [])[1] || '').trim();
const status = s => ((s.match(/^status: (.*)$/m) || [])[1] || '').trim();
const files = new Set(['vault/_paths.md', 'vault/reference/tasks/tasks-to-do-index.md']);

// ---------- validate everything first ----------
const plan = [];
for (const g of def.globals) {
  const id = typeof g === 'string' ? g : g.id;
  const f = path.join(TASKS, id + '.md');
  if (!id || !fs.existsSync(f)) fail(`no global task node for ${id}. Nothing changed.`);
  const s = fs.readFileSync(f, 'utf8');
  if (/^yes\b/.test(field(s, 'confirmed_by_owner'))) fail(`refusing ${id}: already confirmed (${field(s, 'confirmed_by_owner')}). Nothing changed.`);
  const sec = (s.split(/^## Questions\s*$/m)[1] || '').split(/^## /m)[0];
  const all = [...sec.matchAll(/^- (Q\d+)\b/gm)].map(m => m[1]);
  const qs = g.questions || all;
  for (const q of qs) if (!all.includes(q)) fail(`refusing ${id}: ${q} is not in its ## Questions. Nothing changed.`);
  const sub = path.join(TASKS, id);
  const waiting = fs.existsSync(sub) ? fs.readdirSync(sub).filter(n => n.endsWith('.md') && status(fs.readFileSync(path.join(sub, n), 'utf8')) === 'wait-answers') : [];
  plan.push({ id, f, qs, all, waiting });
}
// edits are checked in order on an in-memory copy, so two edits of one file see each other
const text = {}; // abs path -> new content
const edited = [];
for (const e of def.edits || []) {
  if (!e.file || typeof e.old !== "string" || !e.old || typeof e.new !== "string") fail("an edit needs file, old and new. Nothing changed.");
  const f = path.resolve(ROOT, e.file);
  if (!f.startsWith(ROOT + path.sep)) fail(`refusing edit of ${e.file}: outside the repository. Nothing changed.`);
  if (!(f in text)) { if (!fs.existsSync(f)) fail(`refusing edit of ${e.file}: no such file. Nothing changed.`); text[f] = fs.readFileSync(f, "utf8"); }
  const conv = t => text[f].includes('\r\n') ? t.replace(/\r?\n/g, '\r\n') : t;
  const n = text[f].split(conv(e.old)).length - 1;
  if (n === 0) fail(`refusing edit of ${e.file}: old text not found: "${e.old.slice(0, 80)}". Nothing changed.`);
  if (n > 1) fail(`refusing edit of ${e.file}: old text found ${n} times, must be unique. Nothing changed.`);
  text[f] = text[f].replace(conv(e.old), () => conv(e.new));
  edited.push(path.relative(ROOT, f).split(path.sep).join("/"));
}

// ---------- apply ----------
for (const p of plan) {
  let s = fs.readFileSync(p.f, 'utf8');
  const qlist = p.qs.length === p.all.length ? 'every default below is' : `the defaults of ${p.qs.join(', ')} are`;
  const decided = `Decided ${today} (owner "${w}"): ${qlist} accepted; values provisional, owner ${today}.`;
  if (p.all.length) {
    // replace the header line (first non-bullet line of ## Questions), or insert one
    s = s.replace(/(^## Questions[ \t]*\r?\n)((?:[ \t]*\r?\n)*)([^\r\n]*)/m, (m0, h, blank, first) =>
      /^- /.test(first) ? h + blank + decided + '\n' + first : h + blank + decided);
  }
  const accepted = p.qs.length ? ` the defaults of ${p.qs.join(', ')}` : '';
  s = s.replace(/^(- request: .*?)[ \t]*$/m, (m0, r) => `${r}${/[.!?]$/.test(r) ? '' : '.'} Owner answer ${today} (relayed by the main session, verbatim): "${w}" = the plan${accepted ? ' and' + accepted : ''} accepted.`);
  s = s.replace(/^- confirmed_by_owner: .*$/m, () => `- confirmed_by_owner: yes (${today}, "${w}")`);
  fs.writeFileSync(p.f, s);
  node('task-status.js', `${p.id}.md`, 'status=todo');
  files.add(`vault/reference/tasks/${p.id}.md`);
  for (const n of p.waiting) { node('task-status.js', `${p.id}/${n}`, 'status=todo'); files.add(`vault/reference/tasks/${p.id}/${n}`); }
}
for (const f of Object.keys(text)) fs.writeFileSync(f, text[f]);
for (const r of edited) files.add(r);
node('lint.js', '--write');
const list = [...files];
for (const f of list) if (!gitData('ls-files', '--', f).trim()) git('add', '--', f);
const ids = plan.map(p => p.id);
const head = ids.length > 1 ? `${ids[0]}..${ids[ids.length - 1].match(/^task-(\d+)/)[1]}` : ids[0];
git('commit', '-q', '-m', `${head}: owner confirmed (${w}), questions decided, todo\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`, '--', ...list);
process.stdout.write(gitData('log', '--oneline', '-1'));
leftovers(list);
