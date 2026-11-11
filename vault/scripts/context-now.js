// node vault/scripts/context-now.js <task-NNN | task-NNN-slug> [--file <transcript.jsonl>] [--left]
// Current context of the executor of a global task, in k tokens (task-502, owner 2026-11-11): the last turn of its latest
// executor run (input + cache write + cache read of that turn, the figure agent-usage.js calls contextTokens).
// Source, in order: --file <transcript>; else the newest executor transcript on disk whose prompt names the task first
// (same attribution as agent-usage.js); else the last turn of the newest executor trace of that task in usage-snapshot.json.
// Prints `context: <n>k (<source>)`, or `context: unknown (<why>)`. With --left (the global has subtasks left) it also prints
// the HANDOVER line when the context is over alerts.handoverContextTokens of cost-rates.json (120k, owner Q6 of task-502).
// Env RR_HANDOVER_TOKENS overrides the threshold (tests: force the line on a real transcript). Never throws, always exits 0:
// task-next.js calls handoverLines() after its commit, and a missing measure must not block a closing.
const fs = require('fs'), path = require('path'), os = require('os');
const R = path.resolve(__dirname, '..', '..');
const RATES = JSON.parse(fs.readFileSync(path.join(__dirname, 'cost-rates.json'), 'utf8'));
const k = n => Math.round(n / 1000) + 'k';
const ctxOf = u => (u.input_tokens || 0) + (u.cache_creation_input_tokens || 0) + (u.cache_read_input_tokens || 0);
const textOf = c => typeof c === 'string' ? c : (Array.isArray(c) ? c.filter(x => x.type === 'text').map(x => x.text).join(' ') : '');

function threshold() {
  const env = parseInt(process.env.RR_HANDOVER_TOKENS || '', 10);
  return Number.isFinite(env) && env > 0 ? env : RATES.alerts.handoverContextTokens;
}

// Last turn context and first-prompt task number of one transcript.
function readTranscript(file) {
  let prompt = '', last = null, turns = new Set();
  for (const line of fs.readFileSync(file, 'utf8').split('\n')) {
    if (!line) continue;
    let j; try { j = JSON.parse(line); } catch { continue; }
    const m = j.message;
    if (!prompt && j.type === 'user' && m) prompt = textOf(m.content);
    if (j.type === 'assistant' && m && m.id && m.usage) { last = ctxOf(m.usage); turns.add(m.id); }
  }
  return { task: (prompt.match(/task-(\d{2,3})\b/) || [])[1] || null, tokens: last, turns: turns.size };
}

function projectDirs() {
  const slug = R.replace(/[:\\/ ]/g, '-');
  return [slug, ...(RATES.extraProjectSlugs || [])].map(s => path.join(os.homedir(), '.claude', 'projects', s)).filter(d => fs.existsSync(d));
}

// Newest executor transcript (by modification time) attributed to task number num.
function fromTranscripts(num) {
  const found = [];
  for (const dir of projectDirs()) for (const entry of fs.readdirSync(dir)) {
    const sub = path.join(dir, entry, 'subagents');
    if (!fs.existsSync(sub)) continue;
    for (const f of fs.readdirSync(sub)) {
      if (!f.endsWith('.meta.json')) continue;
      let meta; try { meta = JSON.parse(fs.readFileSync(path.join(sub, f), 'utf8')); } catch { continue; }
      if (meta.agentType !== 'executor') continue;
      const file = path.join(sub, f.replace(/\.meta\.json$/, '.jsonl'));
      if (fs.existsSync(file)) found.push({ file, mtime: fs.statSync(file).mtimeMs });
    }
  }
  for (const c of found.sort((a, b) => b.mtime - a.mtime)) {
    const t = readTranscript(c.file);
    if (t.task && t.task.padStart(3, '0') === num && t.tokens !== null) return { tokens: t.tokens, source: `executor ${path.basename(c.file).replace(/^agent-|\.jsonl$/g, '').slice(0, 8)}, turn ${t.turns}, transcript` };
  }
  return null;
}

// Last turn of the newest executor trace of the task kept in usage-snapshot.json (the transcript may be gone).
function fromSnapshot(num) {
  const snap = path.join(__dirname, 'usage-snapshot.json');
  if (!fs.existsSync(snap)) return null;
  const recs = JSON.parse(fs.readFileSync(snap, 'utf8')).records
    .filter(r => r.type === 'executor' && r.globalTask && String(r.globalTask).padStart(3, '0') === num && r.agentTrace && r.agentTrace.turns && r.agentTrace.turns.length)
    .sort((a, b) => (b.end || '').localeCompare(a.end || ''));
  if (!recs.length) return null;
  const turns = recs[0].agentTrace.turns;
  return { tokens: turns[turns.length - 1].contextTokens, source: `executor ${String(recs[0].id).slice(0, 8)}, turn ${turns.length}, usage trace` };
}

function measure(taskId, file) {
  try {
    if (file) { const t = readTranscript(file); return t.tokens === null ? null : { tokens: t.tokens, source: `${path.basename(file)}, turn ${t.turns}` }; }
    const num = ((String(taskId).match(/task-(\d{2,3})\b/) || [])[1] || '').padStart(3, '0');
    return fromTranscripts(num) || fromSnapshot(num);
  } catch (e) { return { tokens: null, source: e.message }; }
}

// Lines task-next.js prints at a closing: always the context line; the HANDOVER line only over the threshold with subtasks left.
function handoverLines(taskId, left, file) {
  const m = measure(taskId, file);
  if (!m || m.tokens === null) return [`context: unknown (${m ? m.source : 'no executor transcript or trace for ' + taskId})`];
  const out = [`context: ${k(m.tokens)} (${m.source})`];
  const limit = threshold();
  if (left && m.tokens > limit) out.push(`HANDOVER ${taskId}: context ${k(m.tokens)} over ${k(limit)} with subtasks left: write "## Handover" in the global node (max 300 words: done, state, next, pitfalls), commit it by path and stop; the relay launches a fresh executor (execution-protocol step 5.9)`);
  return out;
}
module.exports = { measure, handoverLines };

if (require.main === module) {
  const args = process.argv.slice(2);
  const fi = args.indexOf('--file');
  const file = fi >= 0 ? args[fi + 1] : null;
  const id = args.find((a, i) => !a.startsWith('--') && (fi < 0 || i !== fi + 1));
  if (!id) { console.error('usage: context-now.js <task-NNN | task-NNN-slug> [--file <transcript.jsonl>] [--left]'); process.exit(1); }
  for (const l of handoverLines(id, args.includes('--left'), file)) console.log(l);
}
