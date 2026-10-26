#!/usr/bin/env node
// Token usage of every agent and main session of this project, from the Claude Code transcripts.
// Usage: node vault/scripts/agent-usage.js [--json] [--from <task number>] [--snapshot]
// Sources: ~/.claude/projects/<project>/<session>/subagents/agent-*.jsonl (+ .meta.json: agentType, description) for agents,
// ~/.claude/projects/<project>/<session>.jsonl for main sessions. Each API message is counted once (last entry per message id).
// Machine-local data: nothing is sent anywhere.
// Attribution: an agent whose description starts with a subtask id (task-NNN-SS) is a worker of that subtask; an executor (or a
//   legacy orchestrator launched with "Run global task ...") is the pilot of the first task number in its prompt; a planner (or a
//   legacy orchestrator planning pass) is listed apart as planning: it creates tasks later than its prompt, so no task is guessed.
// Unit: input-token equivalents, weights in cost-rates.json (cache write 1.25, cache read 0.1, output 5). USD only when prices are set there.
// --snapshot merges the records into vault/scripts/usage-snapshot.json (committed): Claude Code deletes old transcripts, the
//   snapshot keeps their numbers. A record found in a transcript wins over the snapshot copy.
const fs = require('fs'), path = require('path'), os = require('os');

const args = process.argv.slice(2);
const asJson = args.includes('--json');
const doSnapshot = args.includes('--snapshot');
const fromIdx = args.indexOf('--from');
const fromTask = fromIdx >= 0 ? parseInt(args[fromIdx + 1], 10) : 0;

const repo = path.resolve(__dirname, '..', '..');
const slug = repo.replace(/[:\\/ ]/g, '-');
const projDir = path.join(os.homedir(), '.claude', 'projects', slug);
const SNAP_FILE = path.join(__dirname, 'usage-snapshot.json');
const RATES = JSON.parse(fs.readFileSync(path.join(__dirname, 'cost-rates.json'), 'utf8'));
const W = RATES.weights;

const modelKey = m => /opus/.test(m || '') ? 'opus' : /sonnet/.test(m || '') ? 'sonnet' : /haiku/.test(m || '') ? 'haiku' : /fable/.test(m || '') ? 'fable' : 'other';
const unitsOf = t => t.input * W.input + t.cacheWrite * W.cacheWrite + t.cacheRead * W.cacheRead + t.output * W.output;
// USD from the per-model split; null as soon as one model has no price.
function usdOf(byModel) {
  let usd = 0;
  for (const [k, t] of Object.entries(byModel)) {
    const p = RATES.usdPerMillionInputTokens[k];
    if (p === null || p === undefined) return null;
    usd += unitsOf(t) * p / 1e6;
  }
  return usd;
}
const zeroTok = () => ({ input: 0, cacheWrite: 0, cacheRead: 0, output: 0 });
const addTok = (a, b) => { for (const k of ['input', 'cacheWrite', 'cacheRead', 'output']) a[k] += b[k] || 0; return a; };

const textOf = c => typeof c === 'string' ? c : (Array.isArray(c) ? c.filter(x => x.type === 'text').map(x => x.text).join(' ') : '');
const isToolResultOnly = c => Array.isArray(c) && c.length > 0 && c.every(x => x.type === 'tool_result');

function scan(file, meta, kind, session, id) {
  const byId = new Map();
  let prompt = '', seenAssistant = false, start = '', end = '';
  const resumeReasons = {};
  const mentions = {};
  let handback = '';
  for (const line of fs.readFileSync(file, 'utf8').split('\n')) {
    if (!line) continue;
    let j; try { j = JSON.parse(line); } catch { continue; }
    if (j.timestamp) { start = start || j.timestamp; end = j.timestamp; }
    const m = j.message;
    if (j.type === 'user' && m) {
      const t = textOf(m.content);
      if (!prompt && t) prompt = t;
      else if (seenAssistant && t && !isToolResultOnly(m.content) && !/^\s*(\[SYSTEM NOTIFICATION|<system-reminder>|<task-notification)/.test(t)) {
        const reason = /turn limit|from where you stopped/i.test(t) ? 'turn-limit' : /rate limit|cut off/i.test(t) ? 'rate-limit' : 'message';
        resumeReasons[reason] = (resumeReasons[reason] || 0) + 1;
      }
    }
    if (j.type === 'assistant' && m && m.id) {
      seenAssistant = true;
      if (m.usage) byId.set(m.id, { model: m.model, u: m.usage });
      for (const b of m.content || []) {
        if (b.type !== 'tool_use') continue;
        const s = JSON.stringify(b.input || {});
        if (b.name === 'SubagentHandback') handback += ' ' + (b.input && b.input.message || '');
        if (kind === 'session') for (const x of s.matchAll(/task-(\d{3})\b/g)) mentions[x[1]] = (mentions[x[1]] || 0) + 1;
      }
    }
  }
  const t = zeroTok(), byModel = {}, models = {};
  let firstCtx = null, peakCtx = 0;
  for (const { model, u } of byId.values()) {
    const tok = { input: u.input_tokens || 0, cacheWrite: u.cache_creation_input_tokens || 0, cacheRead: u.cache_read_input_tokens || 0, output: u.output_tokens || 0 };
    addTok(t, tok);
    if (model && model !== '<synthetic>') { addTok(byModel[modelKey(model)] || (byModel[modelKey(model)] = zeroTok()), tok); models[model] = (models[model] || 0) + 1; }
    const ctx = tok.input + tok.cacheWrite + tok.cacheRead;
    if (firstCtx === null) firstCtx = ctx;
    if (ctx > peakCtx) peakCtx = ctx;
  }
  const desc = (meta && meta.description) || '';
  const type = (meta && meta.agentType) || (kind === 'session' ? 'main' : '?');
  const sub = (desc.match(/^task-\d+-\d+/) || [])[0] || null;
  let role = 'session';
  if (kind === 'agent') {
    if (sub) role = 'worker';
    else if (type === 'planner') role = 'planning';
    else if (type === 'executor') role = 'pilot';
    else if (type === 'orchestrator') role = /Run global task|resume from its vault node/i.test(prompt) ? 'pilot' : 'planning';
    else role = 'other';
  }
  const rec = {
    kind, id, session, type, role, desc, sub, start, end,
    seconds: start && end ? Math.round((Date.parse(end) - Date.parse(start)) / 1000) : 0,
    calls: byId.size, ...t, models: Object.keys(models), byModel, firstCtx: firstCtx || 0, peakCtx,
    resumes: Object.values(resumeReasons).reduce((a, b) => a + b, 0), resumeReasons,
  };
  if (role === 'pilot') rec.globalTask = ((prompt.match(/task-(\d{2,3})\b/) || [])[1]) || null;
  if (role === 'planning' || role === 'other') rec.promptStart = prompt.replace(/\s+/g, ' ').slice(0, 110);
  if (role === 'planning') rec.handbackTasks = [...new Set([...handback.matchAll(/task-(\d{3})\b/g)].map(x => x[1]))].slice(0, 12);
  if (kind === 'session') rec.tasksMentioned = Object.entries(mentions).filter(([, n]) => n >= 2).sort((a, b) => b[1] - a[1]).slice(0, 3).map(([k]) => k);
  return rec;
}

// 1. scan the transcripts still on disk
const scanned = [];
if (fs.existsSync(projDir)) {
  for (const entry of fs.readdirSync(projDir)) {
    const p = path.join(projDir, entry);
    if (entry.endsWith('.jsonl')) { scanned.push(scan(p, null, 'session', entry.slice(0, 8), entry.replace(/\.jsonl$/, ''))); continue; }
    const subDir = path.join(p, 'subagents');
    if (!fs.existsSync(subDir)) continue;
    for (const f of fs.readdirSync(subDir)) {
      if (!f.endsWith('.jsonl')) continue;
      let meta = {}; try { meta = JSON.parse(fs.readFileSync(path.join(subDir, f.replace(/\.jsonl$/, '.meta.json')), 'utf8')); } catch {}
      scanned.push(scan(path.join(subDir, f), meta, 'agent', entry.slice(0, 8), f.replace(/^agent-|\.jsonl$/g, '')));
    }
  }
} else if (!fs.existsSync(SNAP_FILE)) { console.error('No transcripts at ' + projDir + ' and no snapshot'); process.exit(1); }

// 2. merge with the snapshot: transcripts win, vanished transcripts keep their saved numbers
const records = new Map();
if (fs.existsSync(SNAP_FILE)) for (const r of JSON.parse(fs.readFileSync(SNAP_FILE, 'utf8')).records) records.set(r.kind + ':' + r.id, r);
for (const r of scanned) records.set(r.kind + ':' + r.id, r);
const all = [...records.values()].sort((a, b) => (a.start || '').localeCompare(b.start || ''));
if (doSnapshot) {
  fs.writeFileSync(SNAP_FILE, '{"version":1,"updated":"' + new Date().toISOString() + '","records":[\n' + all.map(r => JSON.stringify(r)).join(',\n') + '\n]}\n');
}

// 3. derive the reports
const tokOf = r => ({ input: r.input, cacheWrite: r.cacheWrite, cacheRead: r.cacheRead, output: r.output });
const taskNum = id => parseInt(id.split('-')[1], 10);
const sumRecs = recs => {
  const t = zeroTok(), byModel = {};
  let calls = 0, resumes = 0, seconds = 0;
  for (const r of recs) {
    addTok(t, r); calls += r.calls; resumes += r.resumes; seconds += r.seconds;
    for (const [k, v] of Object.entries(r.byModel)) addTok(byModel[k] || (byModel[k] = zeroTok()), v);
  }
  return { ...t, calls, resumes, seconds, byModel, units: unitsOf(t), usd: usdOf(byModel) };
};

const workers = {};
for (const r of all) if (r.role === 'worker') (workers[r.sub] = workers[r.sub] || []).push(r);
const subtasks = Object.entries(workers).filter(([id]) => taskNum(id) >= fromTask)
  .sort(([a], [b]) => a.localeCompare(b, 'en', { numeric: true })).map(([id, recs]) => {
    const s = sumRecs(recs);
    const reasons = {};
    for (const r of recs) for (const [k, v] of Object.entries(r.resumeReasons)) reasons[k] = (reasons[k] || 0) + v;
    return { id, agent: [...new Set(recs.map(r => r.type))].join('+'), models: [...new Set(recs.flatMap(r => r.models))].join('+'), runs: recs.length,
      resumes: s.resumes, resumeReasons: reasons, calls: s.calls, firstCtx: recs[0].firstCtx, peakCtx: Math.max(...recs.map(r => r.peakCtx)), seconds: s.seconds,
      input: s.input, cacheWrite: s.cacheWrite, cacheRead: s.cacheRead, output: s.output, units: s.units, usd: s.usd };
  });

const pilots = all.filter(r => r.role === 'pilot');
const taskIds = new Set([...subtasks.map(s => s.id.split('-')[1]), ...pilots.filter(p => p.globalTask && +p.globalTask >= fromTask).map(p => p.globalTask)]);
const tasks = [...taskIds].sort().map(n => {
  const w = sumRecs(all.filter(r => r.role === 'worker' && r.sub.split('-')[1] === n));
  const p = sumRecs(pilots.filter(r => r.globalTask === n));
  return { task: 'task-' + n, workersUnits: w.units, pilotUnits: p.units, pilotAgents: pilots.filter(r => r.globalTask === n).length, totalUnits: w.units + p.units,
    usd: w.usd !== null && p.usd !== null ? w.usd + p.usd : null };
});
const planning = all.filter(r => r.role === 'planning').map(r => ({ id: r.id, start: r.start, calls: r.calls, firstCtx: r.firstCtx, units: unitsOf(r), promptStart: r.promptStart, handbackTasks: r.handbackTasks }));
const sessions = all.filter(r => r.kind === 'session').map(r => ({ id: r.session, start: r.start, end: r.end, calls: r.calls, firstCtx: r.firstCtx, peakCtx: r.peakCtx, units: unitsOf(r), tasksMentioned: r.tasksMentioned }));
const other = sumRecs(all.filter(r => r.role === 'other'));
const totals = {
  workers: sumRecs(all.filter(r => r.role === 'worker')), pilots: sumRecs(pilots), planning: sumRecs(all.filter(r => r.role === 'planning')),
  other, sessions: sumRecs(all.filter(r => r.kind === 'session')),
};
for (const k of Object.keys(totals)) { delete totals[k].byModel; }

if (asJson) { console.log(JSON.stringify({ weights: W, subtasks, tasks, planning, sessions, totals, snapshot: SNAP_FILE.replace(repo, '').replace(/\\/g, '/'), records: all.length }, null, 1)); process.exit(0); }

const k = n => (n / 1000).toFixed(0) + 'k';
const M = n => (n / 1e6).toFixed(2);
console.log('| Subtask | Agent | Model | Runs | Resumes | Turns | Ctx start/peak | Input | Cache write | Cache read | Output | Units (M) |');
console.log('|---|---|---|---|---|---|---|---|---|---|---|---|');
for (const r of subtasks) console.log(`| ${r.id} | ${r.agent} | ${r.models.replace(/claude-/g, '')} | ${r.runs} | ${r.resumes}${Object.keys(r.resumeReasons).length ? ' (' + Object.keys(r.resumeReasons).join(',') + ')' : ''} | ${r.calls} | ${k(r.firstCtx)}/${k(r.peakCtx)} | ${k(r.input)} | ${k(r.cacheWrite)} | ${k(r.cacheRead)} | ${k(r.output)} | ${M(r.units)} |`);
console.log('\n| Global task | Workers (M) | Pilot (M) | Total (M) |');
console.log('|---|---|---|---|');
for (const t of tasks) console.log(`| ${t.task} | ${M(t.workersUnits)} | ${M(t.pilotUnits)}${t.pilotAgents ? ` (${t.pilotAgents})` : ''} | ${M(t.totalUnits)} |`);
console.log('\nPlanning runs (not attributed to a task):');
for (const p of planning) console.log(`  ${p.start.slice(0, 16)} ${M(p.units)} M, ${p.calls} turns, start ctx ${k(p.firstCtx)}${p.handbackTasks && p.handbackTasks.length ? ', tasks named in its report: ' + p.handbackTasks.join(' ') : ''}`);
console.log('\nMain sessions:');
for (const s of sessions) console.log(`  ${s.id} ${s.start.slice(0, 10)}..${s.end.slice(0, 10)} ${M(s.units)} M, ${s.calls} turns, peak ctx ${k(s.peakCtx)}${s.tasksMentioned && s.tasksMentioned.length ? ', tasks: ' + s.tasksMentioned.join(' ') : ''}`);
const T = totals;
console.log(`\nTotals (M units): workers ${M(T.workers.units)}, pilots ${M(T.pilots.units)}, planning ${M(T.planning.units)}, other agents ${M(T.other.units)}, main sessions ${M(T.sessions.units)}. Records: ${all.length}${doSnapshot ? ' (snapshot written)' : ''}.`);
if (T.workers.usd !== null) console.log(`USD: workers ${T.workers.usd.toFixed(2)}, pilots ${T.pilots.usd === null ? 'n/a' : T.pilots.usd.toFixed(2)}`);
