#!/usr/bin/env node
// Rogue Rigs vault lint (adapted from kb-vault) — enforces the invariants stated in _navigation.md and _authoring.md.
//
//   node scripts/lint.js          check only (exit 1 on any problem)
//   node scripts/lint.js --write  also regenerate _paths.md (the id -> path map)
//
// Invariants (blocking):
//   - every [[wikilink]] in a body (code stripped) targets an existing id
//   - id == file name (stem); ids unique
//   - every node has frontmatter with id, description, parent (root excepted), kind
//   - kind: rule => applies_to present
//   - every node ends with a "## Relations" section
//   - a declared parent lists its child in its own ## Relations
//   - every node is reachable from root by body links only
//   - PLACEMENT: a child lives in its parent's directory or in a direct
//     subdirectory of it (root's children live in reference/<branch>/), so that
//     list_directory on the current node's directory always resolves a parent link
//   - _paths.md is present and up to date (run with --write to regenerate)
// Warnings (non-blocking): description longer than 25 words.

const fs = require('fs');
const path = require('path');

const VAULT = path.resolve(process.env.RR_VAULT || path.join(__dirname, '..'));
const WRITE = process.argv.includes('--write');
const PATHS_FILE = path.join(VAULT, '_paths.md');
const SKIP_DIRS = new Set(['.git', '.obsidian', 'private', 'node_modules', 'scripts', 'templates', 'assets']);
const KINDS = new Set(['note', 'rule', 'spec', 'doc', 'task']);
const SPEC_STATUS = new Set(['approved', 'provisional', 'open', 'deferred']);
const TASK_STATUS = new Set(['todo', 'in-progress', 'review', 'done', 'blocked', 'cancelled']);

const toPosix = (p) => p.split(path.sep).join('/');

function walk(dir, out = []) {
  for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
    const p = path.join(dir, e.name);
    if (e.isDirectory()) { if (!SKIP_DIRS.has(e.name)) walk(p, out); }
    else if (e.name.endsWith('.md')) out.push(p);
  }
  return out;
}

function stripCode(s) {
  return s.replace(/```[\s\S]*?```/g, '').replace(/`[^`]*`/g, '');
}

// Minimal YAML front-matter reader: scalars and one-level "- item" lists.
function parseFrontmatter(text) {
  const m = text.match(/^---\r?\n([\s\S]*?)\r?\n---/);
  if (!m) return null;
  const fm = {};
  let key = null;
  for (const raw of m[1].split(/\r?\n/)) {
    const line = raw.replace(/\r$/, '');
    if (/^\S/.test(line) && line.includes(':')) {
      const i = line.indexOf(':');
      key = line.slice(0, i).trim();
      const v = line.slice(i + 1).trim().replace(/^"(.*)"$/, '$1');
      fm[key] = v === '' ? [] : v;
    } else if (/^\s+- /.test(line) && key) {
      const v = line.trim().replace(/^- /, '').replace(/^"(.*)"$/, '$1');
      if (Array.isArray(fm[key])) fm[key].push(v); else fm[key] = [fm[key], v];
    }
  }
  return fm;
}

const first = (v) => (Array.isArray(v) ? v[0] : v);

// ---------- load ----------
const notes = {};       // id -> {fm, body, rel, dir, meta}
const problems = [];
const warnings = [];

for (const abs of walk(VAULT)) {
  const rel = toPosix(path.relative(VAULT, abs));
  const stem = path.basename(abs, '.md');
  const meta = stem.startsWith('_');
  if (rel === '_paths.md') {
    // Generated file: never scanned (its content is compared below), but it must
    // exist as a node so that [[_paths]] links resolve and root lists it.
    notes._paths = { fm: { id: '_paths', parent: 'root', kind: 'note', description: 'generated map' },
      body: '[[_navigation]] [[root]]', rel, dir: '.', meta: true };
    continue;
  }
  const text = fs.readFileSync(abs, 'utf8');
  const fm = parseFrontmatter(text);
  if (!fm) { problems.push(`${rel}: no frontmatter`); continue; }
  const id = fm.id || stem;
  if (id !== stem) problems.push(`${rel}: id "${id}" != file name "${stem}"`);
  if (notes[id]) problems.push(`${rel}: duplicate id "${id}" (also ${notes[id].rel})`);
  if (!fm.description || !fm.description.length) problems.push(`${rel}: missing description`);
  else if (String(fm.description).split(/\s+/).length > 25)
    warnings.push(`${rel}: description is ${String(fm.description).split(/\s+/).length} words (> 25)`);
  if (id !== 'root') {
    if (!fm.kind) problems.push(`${rel}: missing kind`);
    if (!first(fm.parent)) problems.push(`${rel}: missing parent`);
    if (!/^## Relations\s*$/m.test(text)) problems.push(`${rel}: missing "## Relations" section`);
  }
  if (fm.kind === 'rule' && !fm.applies_to) problems.push(`${rel}: kind=rule without applies_to`);
  if (fm.kind && !KINDS.has(fm.kind)) problems.push(`${rel}: unknown kind "${fm.kind}" (note|rule|spec|doc|task)`);
  if (fm.kind === 'spec' && !SPEC_STATUS.has(fm.status)) problems.push(`${rel}: kind=spec needs status approved|provisional|open|deferred`);
  if (fm.kind === 'task' && !TASK_STATUS.has(fm.status)) problems.push(`${rel}: kind=task needs status todo|in-progress|review|done|blocked|cancelled`);
  notes[id] = { fm, body: stripCode(text), rel, dir: path.posix.dirname(rel), meta };
}

const ids = new Set(Object.keys(notes));

// ---------- links ----------
const children = {};
const inbound = {};
for (const id of ids) { children[id] = new Set(); inbound[id] = 0; }
for (const [id, n] of Object.entries(notes)) {
  for (const m of n.body.matchAll(/\[\[([^\]]+)\]\]/g)) {
    const target = m[1].split('|')[0].split('#')[0].trim();
    if (!target) continue; // a literal "[[ ]]" in prose is not a link
    if (!ids.has(target)) { problems.push(`${n.rel}: dead link [[${target}]]`); continue; }
    children[id].add(target);
    inbound[target]++;
  }
  const par = first(n.fm.parent);
  if (par && !ids.has(par)) problems.push(`${n.rel}: parent "${par}" does not exist`);
}

// ---------- reachability from root (body links only) ----------
if (!ids.has('root')) problems.push('root.md missing');
else {
  const seen = new Set(['root']);
  const queue = ['root'];
  while (queue.length) for (const c of children[queue.pop()]) if (!seen.has(c)) { seen.add(c); queue.push(c); }
  for (const id of [...ids].sort()) if (!seen.has(id)) problems.push(`${notes[id].rel}: unreachable from root by descent`);
}

// ---------- parent <-> Relations coherence, orphans, placement ----------
for (const [id, n] of Object.entries(notes)) {
  const par = first(n.fm.parent);
  if (!par && id !== 'root' && inbound[id] === 0) problems.push(`${n.rel}: orphan (no parent, no inbound link)`);
  if (!par || !ids.has(par)) continue;
  if (!children[par].has(id)) problems.push(`${notes[par].rel}: does not list [[${id}]] although parent=${par}`);
  // placement invariant
  const pdir = notes[par].dir;
  const ok = par === 'root'
    ? (n.meta ? n.dir === '.' : /^reference\/[^/]+$/.test(n.dir))
    : (n.dir === pdir || (n.dir.startsWith(pdir + '/') && !n.dir.slice(pdir.length + 1).includes('/')));
  if (!ok) problems.push(`${n.rel}: placement — must live in ${pdir}/ or a direct subdirectory of it (parent=${par})`);
}

// ---------- _paths.md (id -> path map) ----------
const mapLines = [...ids].filter((id) => !notes[id].meta && !notes[id].rel.startsWith('reference/tasks/')).sort().map((id) => `${id}  ${notes[id].rel}`);
const pathsContent = [
  '---',
  'id: _paths',
  'description: "Generated id -> path map for resolving a wikilink you already hold to its file. Regenerate with: node scripts/lint.js --write. Never edit by hand."',
  'parent: root',
  'kind: note',
  '---',
  '# Wikilink resolution map (generated)',
  '',
  'One line per node: `id  path`. Use it to resolve a wikilink whose target is not in the',
  'directory you are reading (cross-branch jump). This is resolution, not search: you',
  'must already hold the id from a `## Relations` gloss. Do not read it to find a topic.',
  'Task nodes are NOT listed (they are reached through tasks-index). Resolve one by its number:',
  'global `task-NNN-slug` is `reference/tasks/task-NNN-slug.md`; subtask `task-NNN-MM-slug` is',
  '`reference/tasks/task-NNN-<slug of the global>/task-NNN-MM-slug.md` (list that folder).',
  '',
  '```',
  ...mapLines,
  '```',
  '',
  '## Relations',
  '- [[_navigation]] — the protocol that says when to open this map.',
  '- [[root]] — the map this file serves.',
  '',
].join('\n');

if (WRITE) {
  fs.writeFileSync(PATHS_FILE, pathsContent, 'utf8');
} else {
  const cur = fs.existsSync(PATHS_FILE) ? fs.readFileSync(PATHS_FILE, 'utf8').replace(/\r\n/g, '\n') : null;
  if (cur === null) problems.push('_paths.md missing — run: node scripts/lint.js --write');
  else if (cur !== pathsContent) problems.push('_paths.md out of date — run: node scripts/lint.js --write');
}

// ---------- hook installed on this machine? (core.hooksPath is local git config) ----------
try {
  const hp = require('child_process').execSync('git config core.hooksPath', { cwd: VAULT, stdio: ['ignore', 'pipe', 'ignore'] }).toString().trim();
  if (hp !== 'vault/scripts/hooks') warnings.push('pre-commit hook not installed on this machine — run (repo root): git config core.hooksPath vault/scripts/hooks');
} catch { warnings.push('pre-commit hook not installed on this machine — run (repo root): git config core.hooksPath vault/scripts/hooks'); }

// ---------- report ----------
for (const w of warnings) console.log('warn: ' + w);
if (problems.length) {
  console.log(problems.join('\n'));
  console.log(`\n${problems.length} problem(s)`);
  process.exit(1);
}
console.log(`vault OK — ${mapLines.length} nodes, ${warnings.length} warning(s)${WRITE ? ', _paths.md written' : ''}`);
