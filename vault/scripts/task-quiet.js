// Shared by task-start.js and task-next.js: runs a child (node script or git) from the repo root with its stdout and
// stderr captured, and prints only the lines that need action (task-327, owner rule: errors, ALERT lines, refusals,
// commit line, leftovers and the next path are never hidden; noise is dropped).
// Dropped (NOISE): lint `warn:` lines, lint `vault OK ...` lines, git `LF will be replaced by CRLF` /
// `CRLF will be replaced by LF` warnings (and the old-git follow-up line), the record-cost.js summary line
// (`task-NNN-SS <model> <k figures> units <x> M, <n> turns`), Node stack traces. Every other line is printed.
// A failing child (non-zero exit) prints its own error text: the non-noise stdout lines (lint problem lines and count,
// hook output) and, from stderr, the `Error: ...` message of a Node throw (refusals of task-status.js, record-cost.js
// errors) without the source excerpt and stack, or the non-noise stderr lines of any other child (git, hook); then the
// calling script exits with the child's exit code (1 when unknown).
// API: run(cmd, args, { data }) -> stdout. With data: true the stdout is returned unprinted (git ls-files, log, status).
//      node(script, ...args), git(...args), gitData(...args), leftovers(files), nextLine(absPathOrNull).
const { spawnSync } = require('child_process');
const path = require('path');
const ROOT = path.resolve(__dirname, '..', '..');

const NOISE = [
  /^warn: /,
  /^vault OK\b/,
  /LF will be replaced by CRLF|CRLF will be replaced by LF/,
  /^The file will have its original line endings in your working directory/,
  /^task-\d+-\d+ .*units [\d.]+ M, \d+ turns(, USD [\d.]+)?$/,
];
const isNoise = l => NOISE.some(re => re.test(l));
const lines = s => s.split(/\r?\n/).filter(l => l.trim() && !isNoise(l));

// Node throw on stderr: keep the message lines ("Error: ..." up to the first stack frame), drop excerpt, stack, version.
function nodeError(err) {
  const all = err.split(/\r?\n/);
  const i = all.findIndex(l => /^[A-Za-z]*Error(?: \[[A-Z_]+\])?: /.test(l));
  if (i < 0) return null;
  const out = [];
  for (const l of all.slice(i)) { if (/^\s+at /.test(l) || /^Node\.js v\d/.test(l)) break; if (l.trim() && !isNoise(l)) out.push(l); }
  return out;
}

function run(cmd, args, opts = {}) {
  const r = spawnSync(cmd, args, { cwd: ROOT, stdio: ['ignore', 'pipe', 'pipe'], maxBuffer: 1 << 28 });
  const out = r.stdout ? r.stdout.toString() : '', err = r.stderr ? r.stderr.toString() : '';
  if (r.error || r.status !== 0) {
    const who = path.basename(cmd) === path.basename(process.execPath) ? args[0].replace(/\\/g, '/') : cmd + ' ' + args.filter(a => !a.startsWith('safe.directory=') && a !== '-c').slice(0, 2).join(' ');
    console.error(`${who} failed (exit ${r.error ? r.error.message : r.status}):`);
    for (const l of lines(out)) console.error(l);
    for (const l of nodeError(err) || lines(err)) console.error(l);
    process.exit(r.status || 1);
  }
  for (const l of lines(err)) console.log(l);
  if (!opts.data) for (const l of lines(out)) console.log(l);
  return out;
}
const safe = ['-c', 'safe.directory=' + ROOT.replace(/\\/g, '/')];
const node = (script, ...args) => run(process.execPath, [path.join('vault', 'scripts', script), ...args]);
const git = (...args) => run('git', [...safe, ...args]);
const gitData = (...args) => run('git', [...safe, ...args], { data: true });
// D4: `git status --porcelain` lines of the given repo-relative files still modified or untracked after the commit
const leftovers = files => { for (const l of gitData('status', '--porcelain', '--', ...files).split(/\r?\n/)) if (l.trim()) console.log('left: ' + l); };
// D1: last line, absolute path of the started subtask node, or none
const nextLine = abs => console.log('next: ' + (abs || 'none'));

module.exports = { ROOT, run, node, git, gitData, leftovers, nextLine };
