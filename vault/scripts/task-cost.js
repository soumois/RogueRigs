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

// Executor figures the previous closing wrote on the global node (executor_usage), read before write() replaces them, so that
// budgetAlert sees which threshold this closing crosses. { turns: 0, unitsM: 0 } when the line is absent or none.
function previousUsage(id) {
  const num = id.split('-')[1];
  const f = fs.readdirSync(T).find(x => new RegExp('^task-' + num + '-.*\\.md$').test(x));
  const m = f && fs.readFileSync(path.join(T, f), 'utf8').match(/^- executor_usage: runs \d+, turns (\d+),.*units (\d*\.?\d+) M\s*$/m);
  return m ? { turns: +m[1], unitsM: +m[2] } : { turns: 0, unitsM: 0 };
}

// BUDGET lines (owner 2026-11-12, task-507, replacing the owner-wait of task-502): the executor runs of a global, as far as they
// have gone, against the turns and units of its level's budget. Over the budget nothing waits for the owner: the closing that
// crosses it prints a handover line (a fresh executor goes on when subtasks are left), later closings an information line; only
// the closing that crosses alerts.budgetStopFactor times the budget (cost-rates.json, 5, owner "On pourrait limiter à 5 fois le
// budget") prints BUDGET STOP, the owner-wait trigger. prev: previousUsage() of the same global. Every line asks for the overrun
// and its cause in result. null under the budget, and for a global without a level or without an executor run. Never throws:
// the closing itself still completes.
function budgetAlert(data, id, prev = { turns: 0, unitsM: 0 }) {
  const num = id.split('-')[1], task = 'task-' + num;
  try {
    const t = data.tasks.find(x => x.task === task);
    const f = fs.readdirSync(T).find(x => new RegExp('^task-' + num + '-.*\\.md$').test(x));
    if (!t || !t.pilotAgents || !f) return null;
    const s = fs.readFileSync(path.join(T, f), 'utf8');
    const m = s.match(/^- verification_level: ([a-z]+)\s*$/m);
    if (!m || m[1] === 'open') return null;
    const b = budgetOf(m[1]);
    if (!b) return `ALERT ${task}: level ${m[1]} has no budget in verification-levels, executor run not compared`;
    const stop = JSON.parse(fs.readFileSync(path.join(R, 'vault', 'scripts', 'cost-rates.json'), 'utf8')).alerts.budgetStopFactor;
    const now = { turns: t.pilot.calls, unitsM: parseFloat(Mu(t.pilot.units)) }; // same rounded figure as the executor_usage line
    const ratio = x => Math.max(x.turns / b.turns, x.unitsM / b.unitsM);
    if (ratio(now) <= 1) return null;
    const fig = `${now.turns} turns, ${Mu(t.pilot.units)} M units so far, ${ratio(now).toFixed(1)}x the budget of level ${m[1]} (${b.turns} turns, ${b.unitsM} M units)`;
    const cause = 'write the overrun and its cause in result and in the final report';
    if (ratio(now) > stop && ratio(prev) <= stop) return `BUDGET STOP ${task}: executor runs over ${stop}x the budget: ${fig}; with subtasks left write ## Handover, set the next subtask and the global wait-answers with done, remaining and an estimate, and go on only on the owner word (execution-protocol step 5.10); ${cause}`;
    if (ratio(prev) <= 1) return `BUDGET ${task}: executor runs over the budget at this closing: ${fig}; with subtasks left write ## Handover and stop with HANDOVER, a fresh executor goes on, no owner wait (execution-protocol step 5.10); ${cause}`;
    return `BUDGET ${task}: executor runs still over the budget: ${fig}; no handover, no owner wait ${ratio(now) > stop ? `(the ${stop}x stop was passed at an earlier closing)` : `below ${stop}x`}; ${cause}`;
  } catch (e) { return `ALERT ${task}: budget not compared (${e.message})`; }
}
module.exports = { write, budgetAlert, previousUsage };

if (require.main === module) {
  const args = process.argv.slice(2);
  if (!args.length) { console.error('usage: task-cost.js <task-NNN>... | --all'); process.exit(1); }
  const data = JSON.parse(execSync('node vault/scripts/agent-usage.js --json --snapshot --agent-debug', { cwd: R, maxBuffer: 1 << 28 }).toString());
  const ids = args.includes('--all') ? data.tasks.filter(t => t.pilotAgents || t.planning.planners.length).map(t => t.task) : args;
  for (const f of write(data, ids)) console.log(f);
}
