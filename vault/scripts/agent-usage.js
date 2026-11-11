#!/usr/bin/env node
// Token usage of every agent and main session of this project, from the Claude Code transcripts.
// Usage: node vault/scripts/agent-usage.js [--json] [--from <task number>] [--snapshot] [--agent-debug] [--agent-detail <agent-id|session-id>]
// Sources: ~/.claude/projects/<project>/<session>/subagents/agent-*.jsonl (+ .meta.json: agentType, description) for agents,
// ~/.claude/projects/<project>/<session>.jsonl for main sessions. Each API message is counted once (last entry per message id).
// Detailed traces retain the preceding user/tool-result timestamp as a request trigger, the assistant response timestamp, and the interval between triggers.
// Machine-local data: nothing is sent anywhere.
// Attribution: an agent whose description starts with a subtask id (task-NNN-SS) is a worker of that subtask; an executor (or a
//   legacy orchestrator launched with "Run global task ...") is the pilot of the first task number in its prompt; a planner (or a
//   legacy orchestrator planning pass) is listed apart as planning: it creates tasks later than its prompt, so no task is guessed.
// Unit: input-token equivalents, weights in cost-rates.json (cache write 1.25, cache read 0.1, output 5). USD only when prices are set there.
// --snapshot merges the records into vault/scripts/usage-snapshot.json (committed): Claude Code deletes old transcripts, the
//   snapshot keeps their numbers. A record found in a transcript wins over the snapshot copy.
// --agent-debug keeps a trace per turn for planners, executors, bp and cpp (sizes, tools and paths, never content), saved in the
//   snapshot so --agent-detail still shows it once the transcript is gone. Over snapshotMaxBytes (cost-rates.json) the oldest
//   executor traces are removed, their numbers stay. usage-agents.json never holds a trace.
const fs = require('fs'), path = require('path'), os = require('os');

const args = process.argv.slice(2);
const asJson = args.includes('--json');
const doSnapshot = args.includes('--snapshot');
const agentDebug = args.includes('--agent-debug') || args.includes('--planner-debug') || args.includes('--agent-detail') || args.includes('--planner-detail');
const fromIdx = args.indexOf('--from');
const fromTask = fromIdx >= 0 ? parseInt(args[fromIdx + 1], 10) : 0;
const plannerDetailIdx = args.indexOf('--planner-detail');
const plannerDetailId = plannerDetailIdx >= 0 ? args[plannerDetailIdx + 1] : null;
const agentDetailIdx = args.indexOf('--agent-detail');
const agentDetailId = agentDetailIdx >= 0 ? args[agentDetailIdx + 1] : null;

const repo = path.resolve(__dirname, '..', '..');
const slug = repo.replace(/[:\\/ ]/g, '-');
const SNAP_FILE = path.join(__dirname, 'usage-snapshot.json');
const RATES = JSON.parse(fs.readFileSync(path.join(__dirname, 'cost-rates.json'), 'utf8'));
// Transcript folders: the one of the repo path, plus the sessions opened from another folder with the repo added as a
// working directory (the desktop app keys them by the opening folder, e.g. C--Users-soumo-Claude-code).
// Extra slugs: cost-rates.json "extraProjectSlugs" and the env var RR_USAGE_EXTRA_SLUGS (comma separated).
const extraSlugs = [].concat(RATES.extraProjectSlugs || [], (process.env.RR_USAGE_EXTRA_SLUGS || '').split(',')).map(s => s.trim()).filter(Boolean);
const projDirs = [slug, ...extraSlugs].filter((s, i, all) => all.indexOf(s) === i)
  .map(s => path.join(os.homedir(), '.claude', 'projects', s)).filter(d => fs.existsSync(d));
const projDir = projDirs.length ? projDirs.join(', ') : path.join(os.homedir(), '.claude', 'projects', slug);
const W = RATES.weights;

const modelKey = m => /opus/.test(m || '') ? 'opus' : /sonnet/.test(m || '') ? 'sonnet' : /haiku/.test(m || '') ? 'haiku' : /fable/.test(m || '') ? 'fable' : 'other';
const unitsOf = t => t.input * W.input + t.cacheWrite * W.cacheWrite + t.cacheRead * W.cacheRead + t.output * W.output;
function operationOfTool(name) {
  const n = String(name || '').toLowerCase();
  if (n === 'read' || /read_(file|text|resource)|readfile|open_file|view_file/.test(n)) return 'read';
  if (n === 'grep' || n === 'glob' || /search|list_(files|directory)/.test(n)) return 'lookup';
  if (n === 'write' || n === 'edit' || /write_file|edit_file/.test(n)) return 'write';
  return 'other';
}
function taskIdsIn(value) {
  return [...String(value || '').matchAll(/\b(task-\d{3}(?:-\d{2})?(?:-[a-z0-9]+(?:-[a-z0-9]+)*)?)(?=[^a-z0-9-]|$)/ig)]
    .map(m => m[1]).filter((id, i, all) => all.indexOf(id) === i);
}
function taskPathKind(p) {
  const s = String(p || '').replace(/\\/g, '/');
  const prefix = s.includes('vault/reference/tasks/') ? 'vault/reference/tasks/' : s.startsWith('reference/tasks/') ? 'reference/tasks/' : null;
  if (!prefix) return null;
  const rest = s.slice(s.indexOf(prefix) + prefix.length);
  return { kind: /^tasks-(?:to-do|done)-index\.md$/.test(rest) ? 'task-index' : 'task-node' };
}
const TASK_WORKFLOWS = {
  'task-create': 'create-tasks', 'task-start': 'start-task', 'task-status': 'update-task-status',
  'task-next': 'close-or-advance-task', 'task-confirm': 'confirm-task', 'record-cost': 'record-task-cost',
};
function taskWorkflows(name, input) {
  if (!/^(bash|shell|powershell)$/i.test(String(name || ''))) return [];
  const command = [input && input.command, input && input.cmd].filter(x => typeof x === 'string').join('\n');
  const out = [];
  for (const m of command.matchAll(/\b(task-create|task-start|task-status|task-next|task-confirm|record-cost)\.js\b/ig)) {
    const script = m[1].toLowerCase();
    const tail = command.slice(m.index + m[0].length).split(/[;\r\n|&]/, 1)[0];
    out.push({ script, action: TASK_WORKFLOWS[script], taskRefCount: taskIdsIn(tail).length });
  }
  return out;
}
function toolPath(input) {
  const raw = input && (input.file_path || input.filePath || input.path || input.filename);
  if (typeof raw !== 'string' || !raw.trim()) return null;
  const absolute = path.resolve(path.isAbsolute(raw) ? raw : path.join(repo, raw));
  const relative = path.relative(repo, absolute);
  if (!relative || relative === '..' || relative.startsWith('..' + path.sep) || path.isAbsolute(relative)) return '[external]';
  const safe = relative.replace(/\\/g, '/');
  return /(^|\/)(?:\.env(?:\.|$)|[^/]*(?:secret|credential|password|token)[^/]*)$/i.test(safe) ? '[redacted-path]' : safe;
}
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
const roughTokensOfChars = n => Math.ceil((n || 0) / 4);
function textMetrics(content) {
  const text = textOf(content);
  return {
    chars: text.length,
    roughTokens: roughTokensOfChars(text.length),
    lines: text ? text.split(/\r?\n/).length : 0,
    blockTypes: Array.isArray(content) ? [...new Set(content.map(x => x.type || 'unknown'))] : (typeof content === 'string' ? ['text'] : []),
  };
}

function scan(file, meta, kind, session, id) {
  const agentType = (meta && meta.agentType) || '';
  const captureDebug = agentDebug && kind === 'agent' && ['planner', 'orchestrator', 'executor', 'bp', 'cpp'].includes(agentType);
  const byId = new Map();
  const toolUsesById = new Map();
  const toolResultsById = new Map();
  let pendingUserMessages = [];
  let prompt = '', seenAssistant = false, start = '', end = '', pendingTriggerTimestamp = '';
  const resumeReasons = {};
  const mentions = {};
  let handback = '';
  for (const line of fs.readFileSync(file, 'utf8').split('\n')) {
    if (!line) continue;
    let j; try { j = JSON.parse(line); } catch { continue; }
    if (j.timestamp) { start = start || j.timestamp; end = j.timestamp; }
    const m = j.message;
    if (j.type === 'user' && m) {
      if (j.timestamp) pendingTriggerTimestamp = j.timestamp;
      for (const b of (Array.isArray(m.content) ? m.content : [])) {
        if (b.type !== 'tool_result' || !b.tool_use_id) continue;
        const resultText = textOf(b.content);
        const taskFiles = ((resultText.match(/^files:\s*(.*)$/m) || [])[1] || '').split(/\s+/)
          .filter(p => /^vault\/reference\/tasks\/task-\d{3}/.test(p));
        toolResultsById.set(b.tool_use_id, {
          status: b.is_error === true ? 'error' : 'ok',
          createdGlobals: [...resultText.matchAll(/^created task-\d{3}-[a-z0-9-]+/gm)].length,
          createdTaskFiles: taskFiles.length,
          ...(captureDebug ? { debug: textMetrics(b.content) } : {}),
        });
      }
      const t = textOf(m.content);
      if (captureDebug && t && !isToolResultOnly(m.content) && !/^\s*(\[SYSTEM NOTIFICATION|<system-reminder>|<task-notification)/.test(t)) {
        pendingUserMessages.push(textMetrics(m.content));
      }
      if (!prompt && t) prompt = t;
      else if (seenAssistant && t && !isToolResultOnly(m.content) && !/^\s*(\[SYSTEM NOTIFICATION|<system-reminder>|<task-notification)/.test(t)) {
        const reason = /turn limit|from where you stopped/i.test(t) ? 'turn-limit' : /rate limit|cut off/i.test(t) ? 'rate-limit' : 'message';
        resumeReasons[reason] = (resumeReasons[reason] || 0) + 1;
      }
    }
    if (j.type === 'assistant' && m && m.id) {
      seenAssistant = true;
      if (m.usage) {
        const previous = byId.get(m.id);
        byId.set(m.id, {
          model: m.model,
          u: m.usage,
          triggerTimestamp: (previous && previous.triggerTimestamp) || pendingTriggerTimestamp || null,
          responseTimestamp: j.timestamp || (previous && previous.responseTimestamp) || null,
          userMessages: previous ? previous.userMessages : pendingUserMessages,
        });
        if (!previous) { pendingUserMessages = []; pendingTriggerTimestamp = ''; }
      }
      const tools = toolUsesById.get(m.id) || new Map();
      for (const [index, b] of (m.content || []).entries()) {
        if (b.type !== 'tool_use') continue;
        const input = b.input || {};
        const key = b.id || `${m.id}:${index}:${b.name}`;
        const previous = tools.get(key);
        const use = { name: b.name || 'unknown', operation: operationOfTool(b.name), path: toolPath(input) };
        if (captureDebug) {
          const serializedInput = JSON.stringify(input);
          const payloadFields = Object.entries(input).filter(([field, value]) => typeof value === 'string' && /(prompt|description|message|instructions|task)/i.test(field))
            .map(([field, value]) => ({ field, chars: value.length, roughTokens: roughTokensOfChars(value.length) }));
          const targetAgentType = input.subagent_type || input.agent_type || null;
          use.debugInput = {
            chars: serializedInput.length,
            roughTokens: roughTokensOfChars(serializedInput.length),
            fields: Object.keys(input).slice(0, 16),
            payloadFields,
            ...(typeof targetAgentType === 'string' && /^[a-z0-9_-]{1,40}$/i.test(targetAgentType) ? { targetAgentType } : {}),
          };
        }
        if (!use.path && previous && previous.path) use.path = previous.path;
        const taskPath = taskPathKind(use.path);
        if (taskPath) {
          use.taskScope = taskPath.kind;
          use.path = `vault/reference/tasks/[${taskPath.kind === 'task-index' ? 'task index' : 'task node'}]`;
        }
        const workflows = taskWorkflows(b.name, input);
        if (workflows.length) {
          use.workflows = workflows;
        }
        tools.set(key, use);
        const s = JSON.stringify(input);
        if (b.name === 'SubagentHandback') handback += ' ' + (b.input && b.input.message || '');
        if (kind === 'session') for (const x of s.matchAll(/task-(\d{3})\b/g)) mentions[x[1]] = (mentions[x[1]] || 0) + 1;
      }
      if (tools.size) toolUsesById.set(m.id, tools);
    }
  }
  const t = zeroTok(), byModel = {}, models = {};
  let firstCtx = null, peakCtx = 0;
  const turnDetails = [], toolCounts = {}, readPaths = {}, lookupPaths = {}, writePaths = {};
  const taskReadPaths = {}, taskWritePaths = {}, taskWorkflowCounts = {}, taskWorkflowOutcomes = {};
  for (const [messageId, { model, u, userMessages = [], triggerTimestamp = null, responseTimestamp = null }] of byId) {
    const tok = { input: u.input_tokens || 0, cacheWrite: u.cache_creation_input_tokens || 0, cacheRead: u.cache_read_input_tokens || 0, output: u.output_tokens || 0 };
    addTok(t, tok);
    if (model && model !== '<synthetic>') { addTok(byModel[modelKey(model)] || (byModel[modelKey(model)] = zeroTok()), tok); models[model] = (models[model] || 0) + 1; }
    const ctx = tok.input + tok.cacheWrite + tok.cacheRead;
    if (firstCtx === null) firstCtx = ctx;
    if (ctx > peakCtx) peakCtx = ctx;
    const tools = [...(toolUsesById.get(messageId) || new Map()).entries()].map(([toolId, tool]) => {
      const result = toolResultsById.get(toolId);
      if (result) {
        tool.result = result.status;
        if (result.debug) tool.debugResult = result.debug;
        if (tool.workflows && tool.workflows.some(w => w.script === 'task-create')) {
          for (const w of tool.workflows) if (w.script === 'task-create') {
            w.createdGlobals = result.createdGlobals;
            w.createdTaskFiles = result.createdTaskFiles;
          }
        }
      }
      return tool;
    });
    for (const tool of tools) {
      toolCounts[tool.name] = (toolCounts[tool.name] || 0) + 1;
      if (tool.path && tool.operation === 'read') readPaths[tool.path] = (readPaths[tool.path] || 0) + 1;
      if (tool.path && tool.operation === 'lookup') lookupPaths[tool.path] = (lookupPaths[tool.path] || 0) + 1;
      if (tool.path && tool.operation === 'write') writePaths[tool.path] = (writePaths[tool.path] || 0) + 1;
      if (tool.taskScope && tool.path && tool.operation === 'read') taskReadPaths[tool.path] = (taskReadPaths[tool.path] || 0) + 1;
      if (tool.taskScope && tool.path && tool.operation === 'write') taskWritePaths[tool.path] = (taskWritePaths[tool.path] || 0) + 1;
      for (const w of tool.workflows || []) {
        taskWorkflowCounts[w.script] = (taskWorkflowCounts[w.script] || 0) + 1;
        const outcome = tool.result || 'unknown';
        const outcomes = taskWorkflowOutcomes[w.script] || (taskWorkflowOutcomes[w.script] = { ok: 0, error: 0, unknown: 0 });
        outcomes[outcome] = (outcomes[outcome] || 0) + 1;
      }
    }
    const turn = { model: modelKey(model), triggerTimestamp, responseTimestamp, triggerGapSeconds: null, ...tok, units: unitsOf(tok), usd: usdOf({ [modelKey(model)]: tok }), contextTokens: ctx, tools };
    if (captureDebug) {
      const toolInputChars = tools.reduce((n, tool) => n + (tool.debugInput && tool.debugInput.chars || 0), 0);
      const toolResultChars = tools.reduce((n, tool) => n + (tool.debugResult && tool.debugResult.chars || 0), 0);
      turn.debug = {
        userMessages,
        userChars: userMessages.reduce((n, message) => n + message.chars, 0),
        userRoughTokens: roughTokensOfChars(userMessages.reduce((n, message) => n + message.chars, 0)),
        toolInputChars,
        toolInputRoughTokens: roughTokensOfChars(toolInputChars),
        toolResultChars,
        toolResultRoughTokens: roughTokensOfChars(toolResultChars),
      };
    }
    turnDetails.push(turn);
  }
  for (let i = 0; i < turnDetails.length; i++) {
    const current = turnDetails[i].triggerTimestamp ? Date.parse(turnDetails[i].triggerTimestamp) : NaN;
    const previous = i && turnDetails[i - 1].triggerTimestamp ? Date.parse(turnDetails[i - 1].triggerTimestamp) : NaN;
    if (Number.isFinite(current) && Number.isFinite(previous)) turnDetails[i].triggerGapSeconds = Math.round((current - previous) / 1000);
    if (i < turnDetails.length - 1) turnDetails[i].nextContextDelta = turnDetails[i + 1].contextTokens - turnDetails[i].contextTokens;
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
  const trace = {
    ...(captureDebug ? { debugLevel: 'deep', debugEstimate: 'text characters / 4; approximate and not billed-token attribution' } : {}),
    toolCounts, readPaths, lookupPaths, writePaths,
    taskActivity: { taskReadPaths, taskWritePaths, taskWorkflowCounts, taskWorkflowOutcomes },
    turns: turnDetails,
  };
  if (role === 'planning') rec.plannerTrace = trace;
  if (['executor', 'bp', 'cpp'].includes(type) && captureDebug) rec.agentTrace = trace;
  if (kind === 'session') rec.tasksMentioned = Object.entries(mentions).filter(([, n]) => n >= 2).sort((a, b) => b[1] - a[1]).slice(0, 3).map(([k]) => k);
  return rec;
}

// 1. scan the transcripts still on disk
const scanned = [];
if (projDirs.length) {
  for (const dir of projDirs) for (const entry of fs.readdirSync(dir)) {
    const p = path.join(dir, entry);
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
for (const r of scanned) {
  const key = r.kind + ':' + r.id, saved = records.get(key);
  // A scan without --agent-debug builds no trace: it must not erase one already saved.
  if (saved && !agentDebug) for (const f of ['plannerTrace', 'agentTrace']) if (saved[f] && !r[f]) r[f] = saved[f];
  records.set(key, r);
}
const all = [...records.values()].sort((a, b) => (a.start || '').localeCompare(b.start || ''));
if (doSnapshot) {
  // Size limit of the committed snapshot (snapshotMaxBytes in cost-rates.json): over it, the traces of the oldest executors go,
  // their numbers stay. A trace rebuilt from a transcript still on disk is removed again here at each run.
  const lines = all.map(r => JSON.stringify(r));
  const head = '{"version":5,"updated":"' + new Date().toISOString() + '","records":[\n', tail = '\n]}\n';
  let size = Buffer.byteLength(head + tail) + lines.reduce((n, l) => n + Buffer.byteLength(l) + 2, 0);
  for (let i = 0; i < all.length && size > RATES.snapshotMaxBytes; i++) {
    if (all[i].type !== 'executor' || !all[i].agentTrace) continue;
    delete all[i].agentTrace;
    const line = JSON.stringify(all[i]);
    size -= Buffer.byteLength(lines[i]) - Buffer.byteLength(line);
    lines[i] = line;
  }
  if (size > RATES.snapshotMaxBytes) console.error(`ALERT usage-snapshot.json: ${size} bytes is still over snapshotMaxBytes ${RATES.snapshotMaxBytes} with no executor trace left`);
  fs.writeFileSync(SNAP_FILE, head + lines.join(',\n') + tail);
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
// A planner's cost is shared equally by the global tasks it created: the `globals` of the Saved/Planner/*.spec.js files it wrote
// (writePaths of its --agent-debug trace). A planner with no trace or no spec (legacy, edits only) is attributed to no task.
const specCache = {};
const specTasks = rel => {
  if (!(rel in specCache)) {
    try {
      const file = path.join(repo, rel); delete require.cache[require.resolve(file)];
      const s = require(file); const g = Array.isArray(s) ? s : (s.globals || []);
      specCache[rel] = g.map(x => String(Array.isArray(x) ? x[0] : x.num)).filter(n => /^\d+$/.test(n)).map(n => n.padStart(3, '0'));
    } catch { specCache[rel] = []; }
  }
  return specCache[rel];
};
const plannerShares = {}; // task number -> [{ rec, share, tasks }]
for (const r of all.filter(x => x.role === 'planning')) {
  const created = [...new Set(Object.keys((r.plannerTrace && r.plannerTrace.writePaths) || {}).filter(p => /^Saved\/Planner\/.*\.spec\.js$/.test(p)).flatMap(specTasks))].sort();
  for (const n of created) (plannerShares[n] = plannerShares[n] || []).push({ rec: r, share: 1 / created.length, tasks: created });
  r.createdTasks = created;
}
if (doSnapshot) {
  // The crew-board mod reads at most 4 MiB per file and the snapshot passed that with the debug traces: it reads this
  // compact copy instead (agent records without traces, planners with the tasks they created; git-ignored, rebuilt with
  // every snapshot).
  fs.writeFileSync(path.join(__dirname, 'usage-agents.json'), '[\n' + all.filter(r => r.kind === 'agent').map(({ plannerTrace, agentTrace, ...r }) => JSON.stringify(r)).join(',\n') + '\n]\n');
}
const taskIds = new Set([...subtasks.map(s => s.id.split('-')[1]), ...pilots.filter(p => p.globalTask && +p.globalTask >= fromTask).map(p => p.globalTask),
  ...Object.keys(plannerShares).filter(n => +n >= fromTask)]);
const tokFields = s => ({ input: s.input, cacheWrite: s.cacheWrite, cacheRead: s.cacheRead, output: s.output, units: s.units });
const tasks = [...taskIds].sort().map(n => {
  const w = sumRecs(all.filter(r => r.role === 'worker' && r.sub.split('-')[1] === n));
  const pr = pilots.filter(r => r.globalTask === n);
  const p = sumRecs(pr);
  const pl = zeroTok();
  for (const { rec, share } of plannerShares[n] || []) for (const k of Object.keys(pl)) pl[k] += rec[k] * share;
  const planningUnits = unitsOf(pl);
  return { task: 'task-' + n, workersUnits: w.units, pilotUnits: p.units, pilotAgents: pr.length, pilotModels: Object.keys(p.byModel), planningUnits, totalUnits: w.units + p.units + planningUnits,
    usd: w.usd !== null && p.usd !== null ? w.usd + p.usd : null,
    pilot: { ...tokFields(p), calls: p.calls, resumes: p.resumes, seconds: p.seconds, firstCtx: pr.length ? pr[0].firstCtx : 0, peakCtx: pr.length ? Math.max(...pr.map(r => r.peakCtx)) : 0 },
    planning: { ...tokFields({ ...pl, units: planningUnits }), planners: (plannerShares[n] || []).map(({ rec, share, tasks }) => ({ id: rec.id, description: rec.desc, share, tasks })) } };
});
const planning = all.filter(r => r.role === 'planning').map(r => ({ id: r.id, session: r.session, start: r.start, description: r.desc, models: r.models, calls: r.calls, resumes: r.resumes, resumeReasons: r.resumeReasons, firstCtx: r.firstCtx, peakCtx: r.peakCtx, input: r.input, cacheWrite: r.cacheWrite, cacheRead: r.cacheRead, output: r.output, units: unitsOf(r), promptStart: r.promptStart, handbackTasks: r.handbackTasks, trace: r.plannerTrace || null }));
const sessions = all.filter(r => r.kind === 'session').map(r => ({ id: r.session, start: r.start, end: r.end, calls: r.calls, firstCtx: r.firstCtx, peakCtx: r.peakCtx, units: unitsOf(r), tasksMentioned: r.tasksMentioned }));
const other = sumRecs(all.filter(r => r.role === 'other'));
const totals = {
  workers: sumRecs(all.filter(r => r.role === 'worker')), pilots: sumRecs(pilots), planning: sumRecs(all.filter(r => r.role === 'planning')),
  other, sessions: sumRecs(all.filter(r => r.kind === 'session')),
};
for (const k of Object.keys(totals)) { delete totals[k].byModel; }

if (plannerDetailId || agentDetailId) {
  const requestedId = agentDetailId || plannerDetailId;
  const matches = r => (agentDetailId ? Boolean(r.plannerTrace || r.agentTrace) : r.role === 'planning') && (r.id === requestedId || r.session === requestedId);
  const rec = all.find(r => matches(r) && r.id === requestedId) || all.find(matches);
  if (!rec) { console.error(`No ${agentDetailId ? 'agent trace' : 'planner record'} matches ${requestedId}: unknown id, type without traces, transcript gone before its trace was saved, or trace removed by the snapshot size limit`); process.exit(2); }
  const trace = rec.plannerTrace || rec.agentTrace;
  if (!trace) { console.error('This saved record has no detailed trace; refresh the snapshot while its transcript is still available.'); process.exit(2); }
  const detail = { id: rec.id, session: rec.session, start: rec.start, description: rec.desc, type: rec.type, role: rec.role, model: rec.models, calls: rec.calls, resumes: rec.resumes, input: rec.input, cacheWrite: rec.cacheWrite, cacheRead: rec.cacheRead, output: rec.output, units: unitsOf(rec), ...(rec.plannerTrace ? { plannerTrace: trace } : { agentTrace: trace }) };
  if (asJson) console.log(JSON.stringify(detail, null, 2));
  else {
    const kTok = n => Math.abs(n) >= 1000 ? (n / 1000).toFixed(1) + 'k' : String(n);
    const MUnits = n => (n / 1e6).toFixed(2);
    console.log(`${rec.role === 'planning' ? 'Planner' : `${rec.type} agent`} detail: ${rec.desc} (${rec.id}, session ${rec.session})`);
    console.log(`Model ${rec.models.join('+')}; ${rec.calls} API calls; ${rec.resumes} resumes; ${MUnits(unitsOf(rec))} M units.`);
    console.log('Per-call tokens describe the whole context. Trigger UTC is the latest preceding user/tool-result transcript timestamp; response UTC is the assistant usage timestamp. Trigger gaps approximate request spacing, not the exact API start time. Paths and task workflows show activity, not tokens assigned to individual files. Context delta includes all intervening messages and tool results, not just the listed action.');
    if (trace.debugLevel === 'deep') console.log(`Debug detail: exact API units per call; message/tool text sizes estimated at ${trace.debugEstimate}. No message, command or file content is stored.`);
    const deep = trace.debugLevel === 'deep';
    console.log(deep
      ? '| Turn | Trigger UTC | Response UTC | Δ trigger s | Model | Input | Cache write | Cache read | Output | Units M | USD | Prompt ctx | Δ ctx next | User ≈tok | Tool in ≈tok | Tool out ≈tok | Tools and actions |'
      : '| Turn | Trigger UTC | Response UTC | Δ trigger s | Model | Input | Cache write | Cache read | Prompt ctx | Δ ctx next | Output | Tools, paths and task actions |');
    console.log(deep
      ? '|---:|---|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|'
      : '|---:|---|---|---:|---|---:|---:|---:|---:|---:|---:|---|');
    for (const [i, turn] of trace.turns.entries()) {
      const activity = turn.tools.map(tool => {
        const workflows = (tool.workflows || []).map(w => {
          const refs = w.taskRefCount ? `, ${w.taskRefCount} task ref(s)` : '';
          const created = w.createdGlobals ? `, ${w.createdGlobals} global(s), ${w.createdTaskFiles} task file(s) created` : '';
          return `${w.script} → ${w.action}${refs}${created}`;
        }).join(', ');
        const result = tool.result ? ` (${tool.result})` : '';
        const inputSize = tool.debugInput ? `input≈${kTok(tool.debugInput.roughTokens)} tok/${tool.debugInput.chars} ch` : '';
        const payloadSizes = tool.debugInput && tool.debugInput.payloadFields.length
          ? tool.debugInput.payloadFields.map(x => `${x.field}≈${kTok(x.roughTokens)} tok`).join(', ') : '';
        const targetAgent = tool.debugInput && tool.debugInput.targetAgentType ? `agent=${tool.debugInput.targetAgentType}` : '';
        const outputSize = tool.debugResult ? `result≈${kTok(tool.debugResult.roughTokens)} tok/${tool.debugResult.chars} ch${tool.debugResult.blockTypes.length ? ' [' + tool.debugResult.blockTypes.join('+') + ']' : ''}` : '';
        const sizes = [inputSize, payloadSizes, targetAgent, outputSize].filter(Boolean).join('; ');
        return `${tool.name}${tool.operation !== 'other' ? ` ${tool.operation}` : ''}${tool.path ? ': ' + tool.path : ''}${tool.taskScope ? ` [${tool.taskScope}]` : ''}${workflows ? ': ' + workflows : ''}${sizes ? ` {${sizes}}` : ''}${result}`;
      }).join('<br>') || '—';
      const delta = turn.nextContextDelta === undefined ? '—' : `${turn.nextContextDelta >= 0 ? '+' : ''}${kTok(turn.nextContextDelta)}`;
      const trigger = turn.triggerTimestamp || '—';
      const response = turn.responseTimestamp || '—';
      const triggerGap = turn.triggerGapSeconds === null ? '—' : turn.triggerGapSeconds;
      if (deep) {
        const usd = turn.usd === null ? '—' : turn.usd.toFixed(4);
        const d = turn.debug || {};
        console.log(`| ${i + 1} | ${trigger} | ${response} | ${triggerGap} | ${turn.model} | ${kTok(turn.input)} | ${kTok(turn.cacheWrite)} | ${kTok(turn.cacheRead)} | ${kTok(turn.output)} | ${MUnits(turn.units)} | ${usd} | ${kTok(turn.contextTokens)} | ${delta} | ≈${kTok(d.userRoughTokens || 0)} | ≈${kTok(d.toolInputRoughTokens || 0)} | ≈${kTok(d.toolResultRoughTokens || 0)} | ${activity} |`);
      } else console.log(`| ${i + 1} | ${trigger} | ${response} | ${triggerGap} | ${turn.model} | ${kTok(turn.input)} | ${kTok(turn.cacheWrite)} | ${kTok(turn.cacheRead)} | ${kTok(turn.contextTokens)} | ${delta} | ${kTok(turn.output)} | ${activity} |`);
    }
    const paths = (entries, label) => {
      const rows = Object.entries(entries || {}).sort((a, b) => b[1] - a[1]);
      if (rows.length) console.log(`\n${label}: ${rows.map(([p, n]) => `${p} (${n})`).join('; ')}`);
    };
    paths(trace.readPaths, 'Files read');
    paths(trace.lookupPaths, 'Lookup scopes');
    paths(trace.writePaths, 'Files written by direct tools');
    const taskActivity = trace.taskActivity || {};
    paths(taskActivity.taskReadPaths, 'Task files read by direct tools');
    paths(taskActivity.taskWritePaths, 'Task files written by direct tools');
    const taskCounts = taskActivity.taskWorkflowCounts || {};
    if (Object.keys(taskCounts).length) console.log('\nTask workflow invocations (outcome shown per call): ' + Object.entries(taskCounts).map(([name, n]) => `${name} ${n}`).join(', '));
    const taskOutcomes = taskActivity.taskWorkflowOutcomes || {};
    if (Object.keys(taskOutcomes).length) console.log('Task workflow outcomes: ' + Object.entries(taskOutcomes).map(([name, x]) => `${name} (${x.ok} ok, ${x.error} errors, ${x.unknown} unknown)`).join(', '));
    console.log('\nTool totals: ' + (Object.entries(trace.toolCounts).map(([name, n]) => `${name} ${n}`).join(', ') || 'none'));
  }
  process.exit(0);
}

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
console.log('| Agent ID | Date | Description | Model | Calls | Resumes | Input | Cache write | Cache read | Output | Units (M) | Tool activity |');
console.log('|---|---|---|---|---:|---:|---:|---:|---:|---:|---:|---|');
for (const p of planning) {
  const taskActivity = p.trace && p.trace.taskActivity;
  const taskWorkflows = taskActivity && Object.entries(taskActivity.taskWorkflowCounts || {}).map(([name, n]) => `${name} ${n}`).join(', ');
  const activity = p.trace ? [Object.entries(p.trace.toolCounts).map(([name, n]) => `${name} ${n}`).join(', ') || 'no tools',
    taskActivity ? `task file reads ${Object.values(taskActivity.taskReadPaths || {}).reduce((a, n) => a + n, 0)}, direct writes ${Object.values(taskActivity.taskWritePaths || {}).reduce((a, n) => a + n, 0)}` : '',
    taskWorkflows ? `task workflows: ${taskWorkflows}` : ''].filter(Boolean).join('; ') : 'legacy: no trace';
  const desc = (p.description || '').replace(/\|/g, '/');
  console.log(`| ${p.id} | ${p.start.slice(0, 10)} | ${desc} | ${p.models.join('+').replace(/claude-/g, '')} | ${p.calls} | ${p.resumes} | ${k(p.input)} | ${k(p.cacheWrite)} | ${k(p.cacheRead)} | ${k(p.output)} | ${M(p.units)} | ${activity}; ${p.trace ? Object.keys(p.trace.readPaths).length + ' files, ' + Object.keys(p.trace.lookupPaths).length + ' lookup scopes' : ''} |`);
}
console.log('\nMain sessions:');
for (const s of sessions) console.log(`  ${s.id} ${s.start.slice(0, 10)}..${s.end.slice(0, 10)} ${M(s.units)} M, ${s.calls} turns, peak ctx ${k(s.peakCtx)}${s.tasksMentioned && s.tasksMentioned.length ? ', tasks: ' + s.tasksMentioned.join(' ') : ''}`);
const T = totals;
console.log(`\nTotals (M units): workers ${M(T.workers.units)}, pilots ${M(T.pilots.units)}, planning ${M(T.planning.units)}, other agents ${M(T.other.units)}, main sessions ${M(T.sessions.units)}. Records: ${all.length}${doSnapshot ? ' (snapshot written)' : ''}.`);
if (T.workers.usd !== null) console.log(`USD: workers ${T.workers.usd.toFixed(2)}, pilots ${T.pilots.usd === null ? 'n/a' : T.pilots.usd.toFixed(2)}`);
