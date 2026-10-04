#!/usr/bin/env node
// Token usage per subtask, read from the Claude Code transcripts of this project.
// Usage: node vault/scripts/agent-usage.js [--json] [--from <task number>]
// Source: ~/.claude/projects/<project>/<session>/subagents/agent-*.jsonl (+ .meta.json whose
// "description" starts with the subtask id) and <session>.jsonl for the main (Orchestrator) session.
// Each API message is counted once (last entry per message id). Machine-local data: nothing is sent anywhere.
const fs = require('fs'), path = require('path'), os = require('os');

const args = process.argv.slice(2);
const asJson = args.includes('--json');
const fromIdx = args.indexOf('--from');
const fromTask = fromIdx >= 0 ? parseInt(args[fromIdx + 1], 10) : 0;

const repo = path.resolve(__dirname, '..', '..');
const slug = repo.replace(/[:\\/ ]/g, '-');
const projDir = path.join(os.homedir(), '.claude', 'projects', slug);
if (!fs.existsSync(projDir)) { console.error('No transcripts at ' + projDir); process.exit(1); }

function usageOf(file) {
  const byId = new Map();
  for (const line of fs.readFileSync(file, 'utf8').split('\n')) {
    if (!line) continue;
    let j; try { j = JSON.parse(line); } catch { continue; }
    const m = j.message;
    if (!m || !m.usage || !m.id) continue;
    byId.set(m.id, { model: m.model, u: m.usage });
  }
  const t = { input: 0, cacheWrite: 0, cacheRead: 0, output: 0, calls: byId.size, models: {} };
  for (const { model, u } of byId.values()) {
    t.input += u.input_tokens || 0;
    t.cacheWrite += u.cache_creation_input_tokens || 0;
    t.cacheRead += u.cache_read_input_tokens || 0;
    t.output += u.output_tokens || 0;
    if (model && model !== '<synthetic>') t.models[model] = (t.models[model] || 0) + 1;
  }
  return t;
}
function add(a, b) {
  for (const k of ['input', 'cacheWrite', 'cacheRead', 'output', 'calls']) a[k] += b[k];
  for (const [m, n] of Object.entries(b.models)) a.models[m] = (a.models[m] || 0) + n;
}
const zero = () => ({ input: 0, cacheWrite: 0, cacheRead: 0, output: 0, calls: 0, models: {}, runs: 0, agents: new Set() });

const subtasks = {}, orchestrator = zero(), unmatched = zero();
for (const session of fs.readdirSync(projDir)) {
  const sDir = path.join(projDir, session);
  if (session.endsWith('.jsonl')) { add(orchestrator, usageOf(sDir)); orchestrator.runs++; continue; }
  const subDir = path.join(sDir, 'subagents');
  if (!fs.existsSync(subDir)) continue;
  for (const f of fs.readdirSync(subDir)) {
    if (!f.endsWith('.jsonl')) continue;
    const metaFile = path.join(subDir, f.replace(/\.jsonl$/, '.meta.json'));
    let meta = {}; try { meta = JSON.parse(fs.readFileSync(metaFile, 'utf8')); } catch {}
    const id = ((meta.description || '').match(/task-\d+-\d+/) || [])[0];
    const u = usageOf(path.join(subDir, f));
    if (!id) { add(unmatched, u); unmatched.runs++; continue; }
    if (parseInt(id.split('-')[1], 10) < fromTask) continue;
    const s = subtasks[id] || (subtasks[id] = zero());
    add(s, u); s.runs++; s.agents.add(meta.agentType || '?');
  }
}
const rows = Object.entries(subtasks).sort(([a], [b]) => a.localeCompare(b, 'en', { numeric: true }))
  .map(([id, s]) => ({ id, agent: [...s.agents].join('+'), models: Object.keys(s.models).join('+'), runs: s.runs, calls: s.calls,
    input: s.input, cacheWrite: s.cacheWrite, cacheRead: s.cacheRead, output: s.output }));
const strip = o => ({ ...o, agents: undefined, models: Object.keys(o.models).join('+') });
if (asJson) { console.log(JSON.stringify({ subtasks: rows, orchestrator: strip(orchestrator), unmatched: strip(unmatched) }, null, 1)); process.exit(0); }
const k = n => (n / 1000).toFixed(0) + 'k';
console.log('| Subtask | Agent | Model | Runs | Input | Cache write | Cache read | Output |');
console.log('|---|---|---|---|---|---|---|---|');
for (const r of rows) console.log(`| ${r.id} | ${r.agent} | ${r.models} | ${r.runs} | ${k(r.input)} | ${k(r.cacheWrite)} | ${k(r.cacheRead)} | ${k(r.output)} |`);
console.log(`\nOrchestrator (all main sessions): input ${k(orchestrator.input)}, cache write ${k(orchestrator.cacheWrite)}, cache read ${k(orchestrator.cacheRead)}, output ${k(orchestrator.output)}`);
console.log(`Agents without a task id: ${unmatched.runs} runs, output ${k(unmatched.output)}`);
