// node vault/scripts/record-cost.js <task-NNN-SS> <runs>
// Fills model, tokens, usage and runs in the subtask node from agent-usage.js, adds or updates its row in cost-log, refreshes the
// cost-log totals and the usage snapshot (vault/scripts/usage-snapshot.json), and prints an ALERT line for each budget overrun
// (thresholds in cost-rates.json): weighted units over token_ceiling (field written `- token_ceiling: <x> M units`, decimal allowed;
// a legacy integer or `k` value, or `open`, gives no ceiling alert), start context too large, turn limit near or hit, and the
// executor run of the global over the budget of its verification level (task-cost.js budgetAlert: a BUDGET line, the handover
// trigger, or BUDGET STOP at the stop factor, the owner-wait trigger of task-507; the closing itself still completes).
const fs = require('fs'), { execSync } = require('child_process');
const R = require('path').resolve(__dirname, '..', '..').split(require('path').sep).join('/') + '/';
process.chdir(R);
const [id, runs] = process.argv.slice(2);
const data = JSON.parse(execSync('node vault/scripts/agent-usage.js --json --snapshot --agent-debug', { maxBuffer: 1 << 28 }).toString());
const rates = JSON.parse(fs.readFileSync(R + 'vault/scripts/cost-rates.json', 'utf8'));
// A subtask done by the executor itself has no worker of its own: its tokens stay on the executor's run of the global task
// (agent-usage.js role pilot, "Executors" line of the cost-log totals) and its row is written with zeros so that closure still has one.
const globalId = id.split('-').slice(0, 2).join('-');
const pilot = data.tasks.find(t => t.task === globalId && t.pilotAgents > 0);
const worker = data.subtasks.find(x => x.id === id);
if (!worker && !pilot) throw new Error('no usage for ' + id);
const r = worker || { models: pilot.pilotModels.join('+'), input: 0, cacheWrite: 0, cacheRead: 0, output: 0, units: 0, usd: null, calls: 0, firstCtx: 0, peakCtx: 0, resumes: 0, resumeReasons: {}, seconds: 0 };
const model = worker ? r.models.split('+').filter(m => m && m !== '<synthetic>').map(m => m.replace('claude-', '').replace(/-\d.*$/, '')).join('+') : r.models;
const T = R + 'vault/reference/tasks/';
let file;
for (const d of fs.readdirSync(T, { withFileTypes: true })) if (d.isDirectory()) for (const f of fs.readdirSync(T + d.name)) if (f.startsWith(id + '-')) file = T + d.name + '/' + f;
const k = n => Math.round(n / 1000) + 'k';
const Mu = n => (n / 1e6).toFixed(2);
const reasons = Object.entries(r.resumeReasons).map(([a, n]) => `${a} x${n}`).join(', ');
const usage = worker ? `turns ${r.calls}, context start ${k(r.firstCtx)} peak ${k(r.peakCtx)}, resumes ${r.resumes}${reasons ? ' (' + reasons + ')' : ''}, ${Math.round(r.seconds / 60)} min, units ${Mu(r.units)} M${r.usd === null ? '' : ', USD ' + r.usd.toFixed(2)}` : `included in the executor run of ${globalId}`;
const tokensLine = worker ? `- tokens: input ${r.input}, cache write ${r.cacheWrite}, cache read ${r.cacheRead}, output ${r.output}` : `- tokens: included in the executor run of ${globalId}`;
let s = fs.readFileSync(file, 'utf8');
s = s.replace(/^- model: .*$/m, '- model: ' + model).replace(/^- runs: .*$/m, '- runs: ' + runs);
if (/^- usage: .*$/m.test(s)) s = s.replace(/^- tokens: .*$/m, tokensLine).replace(/^- usage: .*$/m, '- usage: ' + usage);
else s = s.replace(/^- tokens: .*$/m, tokensLine + '\n- usage: ' + usage);
// token_ceiling bounds the weighted units (the figure of the usage line), written `<x> M units` (decimal allowed, e.g. 1.2 M units).
// A legacy value without `M units` (integer, `k`) or `open` gives no ceiling alert.
const ceilingM = s.match(/^- token_ceiling: (\d+(?:\.\d+)?|\.\d+) ?M units\b/m);
const ceiling = ceilingM ? parseFloat(ceilingM[1]) : null;
fs.writeFileSync(file, s);

const nodeId = file.split('/').pop().replace('.md', '');
const C = R + 'vault/reference/production/cost-log.md';
// LF only from here on: every pattern below expects \n, and a CRLF working copy (core.autocrlf) would silently add no row.
let c = fs.readFileSync(C, 'utf8').replace(/\r\n/g, '\n');
const OLD_HEAD = '| Subtask | Model | Runs | Input | Cache write | Cache read | Output |\n|---|---|---|---|---|---|---|';
const NEW_HEAD = '| Subtask | Model | Runs | Input | Cache write | Cache read | Output | Units (M) | Turns | Resumes |\n|---|---|---|---|---|---|---|---|---|---|';
const kv = x => parseFloat(x.replace(/[^\d.]/g, '')) * (/M/.test(x) ? 1e6 : 1000);
const units = (i, w, rd, o) => i + w * rates.weights.cacheWrite + rd * rates.weights.cacheRead + o * rates.weights.output;
if (c.includes(OLD_HEAD)) { // one-time migration: old rows get an approximate Units value from their rounded columns
  c = c.replace(OLD_HEAD, NEW_HEAD).replace(/^(\| \[\[task-[^\]]+\]\] \|[^\n]*\| (\d+k) \| (\d+k) \| (\d+k) \| (\d+k) \|)$/gm, (m, a, i, w, rd, o) => `${a} ~${Mu(units(kv(i), kv(w), kv(rd), kv(o)))} | — | — |`);
}
const row = `| [[${nodeId}]] | ${model} | ${runs} | ${k(r.input)} | ${k(r.cacheWrite)} | ${k(r.cacheRead)} | ${k(r.output)} | ${Mu(r.units)} | ${r.calls} | ${r.resumes} |`;
const rowRe = new RegExp('^\\| \\[\\[' + nodeId + '\\]\\].*$', 'm');
if (rowRe.test(c)) c = c.replace(rowRe, row);
else c = c.replace(/(\n## Per subtask\n[\s\S]*?)(\n\n## Totals)/, (m, a, b) => a + '\n' + row + b);

// Totals are rebuilt from the rows above (rounded to k) and from the snapshot totals (exact), never edited by hand.
const tot = { all: [0, 0, 0, 0, 0] }, byModel = {};
const section = (c.match(/## Per subtask\n([\s\S]*?)\n\n## Totals/) || [, ''])[1];
for (const line of section.match(/^\| \[\[task-[^\]]+\]\] \|.*$/gm) || []) {
  const p = line.split('|').map(x => x.trim());
  const nums = [kv(p[4]), kv(p[5]), kv(p[6]), kv(p[7]), parseFloat(p[8].replace('~', '')) * 1e6];
  for (const key of ['all', p[2].includes('+') ? 'mixed' : p[2]]) { const t = key === 'all' ? tot.all : (byModel[key] = byModel[key] || [0, 0, 0, 0, 0]); nums.forEach((n, i) => { t[i] += n; }); }
}
const fmt = t => `| ${Mu(t[4])} | ${(t[0] / 1000).toFixed(0)}k | ${Mu(t[1])} M | ${Mu(t[2])} M | ${(t[3] / 1000).toFixed(0)}k |`;
const fmtX = x => `| ${Mu(x.units)} | ${k(x.input)} | ${Mu(x.cacheWrite)} M | ${Mu(x.cacheRead)} M | ${k(x.output)} |`;
const totals = ['## Totals', 'Rebuilt by `record-cost.js` at each closing; do not edit. Units are input-token equivalents (weights in `vault/scripts/cost-rates.json`). Subtask rows are rounded to k; the lines below them come from the usage snapshot (exact).', '',
  '| Scope | Units (M) | Input | Cache write | Cache read | Output |', '|---|---|---|---|---|---|',
  `| All subtask rows above | ${fmt(tot.all).slice(2)}`,
  ...Object.entries(byModel).sort().map(([m, t]) => `| model ${m} | ${fmt(t).slice(2)}`),
  `| Executors and legacy orchestrators that ran a task (pilot) | ${fmtX(data.totals.pilots).slice(2)}`,
  `| Planning runs (planners and legacy planning passes) | ${fmtX(data.totals.planning).slice(2)}`,
  `| Agents without a subtask id (other) | ${fmtX(data.totals.other).slice(2)}`,
  `| Main sessions (all of them, also chat outside tasks) | ${fmtX(data.totals.sessions).slice(2)}`].join('\n');
c = c.replace(/## Totals\n[\s\S]*?(?=\n## Lessons for estimates)/, totals + '\n');
fs.writeFileSync(C, c);

console.log(id, model, k(r.cacheWrite), k(r.cacheRead), k(r.output), 'units ' + Mu(r.units) + ' M,', r.calls + ' turns');
const alerts = [];
const unitsM = parseFloat(Mu(r.units)); // same rounded figure as the usage line
if (ceiling && unitsM > ceiling * rates.alerts.ceilingRatio) alerts.push(`units ${Mu(r.units)} M is over token_ceiling ${ceilingM[1]} M (${(unitsM / ceiling).toFixed(2)}x): the subtask was too big, split the next similar one`);
if (r.firstCtx > rates.alerts.startContextTokens) alerts.push(`context at the first turn is ${k(r.firstCtx)} (over ${k(rates.alerts.startContextTokens)}): check the agent's tool list and what it loads at start`);
if (r.resumeReasons['turn-limit']) alerts.push('the agent hit its turn limit and was resumed: split the subtask, the whole context is re-read on every extra turn');
else if (r.calls >= rates.alerts.turns) alerts.push(`${r.calls} turns (alert from ${rates.alerts.turns}): close to the turn limit`);
if (r.resumes > 0 && !r.resumeReasons['turn-limit']) alerts.push(`resumed ${r.resumes} time(s) (${reasons}): write the cause in the subtask result`);
for (const a of alerts) console.log('ALERT ' + id + ': ' + a);
// The executor run and the planner share live on the global node (the subtasks done by the executor say "included in the executor run").
const taskCost = require('./task-cost');
const prevUsage = taskCost.previousUsage(globalId); // figures of the previous closing, before write() replaces them
taskCost.write(data, [globalId]);
// The executor run so far against the budget of the level of its global (task-483); over it, a BUDGET line: handover when this
// closing crosses the budget, BUDGET STOP when it crosses the stop factor (task-507).
const budget = taskCost.budgetAlert(data, globalId, prevUsage);
if (budget) console.log(budget);
