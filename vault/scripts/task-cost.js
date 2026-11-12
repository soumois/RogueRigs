// node vault/scripts/task-cost.js <task-NNN | task-NNN-slug>... | --all
// Writes on each global task node the cost that is not on its subtasks: the executor run (role pilot of agent-usage.js) and the
// task's share of the planner runs that created it (a planner shared by N created tasks counts 1/N on each). Lines written, before
// `- result:` (replaced when present): executor_tokens, executor_usage, planning_tokens, planning_usage. Tokens use the subtask
// `tokens` format so the crew-board mod reads them. record-cost.js calls write() for the global of each closed subtask.
// --all rewrites every global task that has executor or planning usage.
const fs = require('fs'), path = require('path'), { execSync } = require('child_process');
const R = path.resolve(__dirname, '..', '..');
const T = path.join(R, 'vault', 'reference', 'tasks');
const LEVELS = path.join(R, 'vault', 'reference', 'agents', 'verification-levels.md');
const r0 = n => Math.round(n);
const k = n => Math.round(n / 1000) + 'k';
const Mu = n => (n / 1e6).toFixed(2);
const tok = t => `input ${r0(t.input)}, cache write ${r0(t.cacheWrite)}, cache read ${r0(t.cacheRead)}, output ${r0(t.output)}`;

function linesFor(t) {
  const p = t.pilot, pl = t.planning;
  return {
    executor_tokens: t.pilotAgents ? tok(p) : 'none (no executor run)',
    executor_usage: t.pilotAgents ? `runs ${t.pilotAgents}, turns ${p.calls}, context start ${k(p.firstCtx)} peak ${k(p.peakCtx)}, resumes ${p.resumes}, ${Math.round(p.seconds / 60)} min, units ${Mu(p.units)} M` : 'none',
    planning_tokens: pl.planners.length ? tok(pl) : 'none (no planner run attributed)',
    planning_usage: pl.planners.length ? pl.planners.map(x => `${x.id.slice(0, 8)} "${x.description}" 1/${x.tasks.length} (tasks ${x.tasks.join(', ')})`).join('; ') + `, units ${Mu(pl.units)} M` : 'none',
  };
}

function write(data, ids) {
  const changed = [];
  for (const id of ids) {
    const num = id.split('-')[1];
    const t = data.tasks.find(x => x.task === 'task-' + num);
    const f = fs.readdirSync(T).find(x => new RegExp('^task-' + num + '-.*\\.md$').test(x));
    if (!t || !f) continue;
    const file = path.join(T, f);
    const raw = fs.readFileSync(file, 'utf8'), crlf = raw.includes('\r\n');
    let s = raw.replace(/\r\n/g, '\n');
    for (const [key, val] of Object.entries(linesFor(t))) {
      const re = new RegExp('^- ' + key + ': .*$', 'm');
      if (re.test(s)) s = s.replace(re, `- ${key}: ${val}`);
      else s = s.replace(/^- result: /m, `- ${key}: ${val}\n- result: `);
    }
    if (crlf) s = s.replace(/\n/g, '\r\n');
    if (s !== raw) { fs.writeFileSync(file, s); changed.push(path.relative(R, file).split(path.sep).join('/')); }
  }
  return changed;
}

// Budget of a verification level: read from the table of the rule node verification-levels, the only place the figures live.
function budgetOf(level) {
  const row = fs.readFileSync(LEVELS, 'utf8').split(/\r?\n/).map(l => l.match(/^\|\s*([a-z]+)\s*\|.*\|\s*(\d+) turns, (\d*\.?\d+) M units\s*\|\s*$/)).find(m => m && m[1] === level);
  return row ? { turns: +row[2], unitsM: +row[3] } : null;
}

// ALERT line when the executor run of a global, as far as it has gone, has passed the turns or the units of its level's budget;
// null otherwise, and for a global without a level or without an executor run. The budget is only recorded (owner 2026-11-12,
// task-508): no handover, no owner wait. Never throws: the alert must not block a closing.
function budgetAlert(data, id) {
  const num = id.split('-')[1], task = 'task-' + num;
  try {
    const t = data.tasks.find(x => x.task === task);
    const f = fs.readdirSync(T).find(x => new RegExp('^task-' + num + '-.*\\.md$').test(x));
    if (!t || !t.pilotAgents || !f) return null;
    const m = fs.readFileSync(path.join(T, f), 'utf8').match(/^- verification_level: ([a-z]+)\s*$/m);
    if (!m || m[1] === 'open') return null;
    const b = budgetOf(m[1]);
    if (!b) return `ALERT ${task}: level ${m[1]} has no budget in verification-levels, executor run not compared`;
    const unitsM = parseFloat(Mu(t.pilot.units)); // same rounded figure as the executor_usage line
    if (t.pilot.calls <= b.turns && unitsM <= b.unitsM) return null;
    return `ALERT ${task}: executor run over the budget of level ${m[1]} (${b.turns} turns, ${b.unitsM} M units): ${t.pilot.calls} turns, ${Mu(t.pilot.units)} M units so far; write the cause in result and list it in the final report`;
  } catch (e) { return `ALERT ${task}: budget not compared (${e.message})`; }
}
module.exports = { write, budgetAlert };

if (require.main === module) {
  const args = process.argv.slice(2);
  if (!args.length) { console.error('usage: task-cost.js <task-NNN>... | --all'); process.exit(1); }
  const data = JSON.parse(execSync('node vault/scripts/agent-usage.js --json --snapshot --agent-debug', { cwd: R, maxBuffer: 1 << 28 }).toString());
  const ids = args.includes('--all') ? data.tasks.filter(t => t.pilotAgents || t.planning.planners.length).map(t => t.task) : args;
  for (const f of write(data, ids)) console.log(f);
}
