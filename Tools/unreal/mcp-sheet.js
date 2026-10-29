#!/usr/bin/env node
// Builds the compact Unreal MCP tool sheet read by the bp agent instead of list_toolsets / describe_toolset.
// Usage: node Tools/unreal/mcp-sheet.js [--from 332] [--to 357] [--out Tools/unreal/mcp-tool-sheet.md]
// Source: the Claude Code transcripts (~/.claude/projects/<project>/<session>/subagents/agent-*.jsonl + .meta.json), the
//   location vault/scripts/agent-usage.js reads. A bp agent (meta agentType "bp") whose description starts with a subtask id
//   task-NNN-SS, NNN in [from, to], contributes the tools it called through mcp__unreal__call_tool. Argument shapes come from
//   the describe_toolset answers (inline, or persisted under <session>/tool-results/); when a toolset was not described in the
//   selected runs, any other transcript of the project that described it is used. No editor, no MCP call, machine-local data.
// Rerun after an engine or plugin update (shapes may change), and commit the regenerated sheet.
const fs = require('fs'), path = require('path'), os = require('os');

const args = process.argv.slice(2);
const argOf = (name, def) => { const i = args.indexOf(name); return i >= 0 ? args[i + 1] : def; };
const fromTask = parseInt(argOf('--from', '332'), 10);
const toTask = parseInt(argOf('--to', '357'), 10);
const repo = path.resolve(__dirname, '..', '..');
const outFile = path.resolve(repo, argOf('--out', 'Tools/unreal/mcp-tool-sheet.md'));
const slug = repo.replace(/[:\\/ ]/g, '-');
const projDir = path.join(os.homedir(), '.claude', 'projects', slug);
if (!fs.existsSync(projDir)) { console.error('No transcripts at ' + projDir); process.exit(1); }

const textOf = c => typeof c === 'string' ? c : (Array.isArray(c) ? c.filter(x => x && x.type === 'text').map(x => x.text).join('\n') : '');

// Parses a describe_toolset answer (inline text or persisted file) into { name, tools: [{ name, inputSchema }] }.
function parseDescribe(raw) {
  const persisted = raw.match(/saved to:?\s+(\S+?\.(?:json|txt))\b/);
  if (persisted) {
    if (!fs.existsSync(persisted[1])) return null;
    raw = fs.readFileSync(persisted[1], 'utf8');
    try { const arr = JSON.parse(raw); if (Array.isArray(arr)) raw = textOf(arr); } catch { /* raw text file */ }
  }
  try { const j = JSON.parse(raw); return j && Array.isArray(j.tools) ? j : null; } catch { return null; }
}

// Reads one transcript: MCP tool calls (deduplicated by tool_use id) and toolset descriptions.
function scan(file) {
  const uses = new Map(), results = new Map();
  for (const line of fs.readFileSync(file, 'utf8').split('\n')) {
    if (!line) continue;
    let j; try { j = JSON.parse(line); } catch { continue; }
    const m = j.message;
    if (!m || !Array.isArray(m.content)) continue;
    for (const b of m.content) {
      if (b.type === 'tool_use' && /^mcp__unreal__/.test(b.name)) uses.set(b.id, { name: b.name, input: b.input || {} });
      if (b.type === 'tool_result') results.set(b.tool_use_id, { text: textOf(b.content), isError: !!b.is_error });
    }
  }
  const calls = [], describes = [];
  for (const [id, u] of uses) {
    const r = results.get(id);
    if (u.name === 'mcp__unreal__call_tool') calls.push({ toolset: u.input.toolset_name, tool: u.input.tool_name, argKeys: Object.keys(u.input.arguments || {}), ok: r ? !r.isError : false });
    if (u.name === 'mcp__unreal__describe_toolset' && r && !r.isError) { const d = parseDescribe(r.text); if (d) describes.push(d); }
  }
  return { calls, describes };
}

// 1. walk the transcripts
const used = new Map();       // "toolset|tool" -> { toolset, tool, count, ok, argKeys:Set }
const schemas = new Map();    // toolset -> Map(bare tool -> inputSchema), first found in the selected runs wins
const fallback = new Map();   // same, from any other transcript
let runs = 0;
for (const session of fs.readdirSync(projDir)) {
  const subDir = path.join(projDir, session, 'subagents');
  if (!fs.existsSync(subDir)) continue;
  for (const f of fs.readdirSync(subDir)) {
    if (!f.endsWith('.jsonl')) continue;
    let meta = {}; try { meta = JSON.parse(fs.readFileSync(path.join(subDir, f.replace(/\.jsonl$/, '.meta.json')), 'utf8')); } catch {}
    const num = parseInt(((meta.description || '').match(/^task-(\d+)-\d+/) || [])[1], 10);
    const selected = meta.agentType === 'bp' && num >= fromTask && num <= toTask;
    const { calls, describes } = scan(path.join(subDir, f));
    const target = selected ? schemas : fallback;
    for (const d of describes) {
      const map = target.get(d.name) || new Map();
      for (const t of d.tools) { const bare = t.name.split('.').pop(); if (!map.has(bare)) map.set(bare, t.inputSchema || {}); }
      target.set(d.name, map);
    }
    if (!selected) continue;
    runs++;
    for (const c of calls) {
      if (!c.toolset || !c.tool) continue;
      const key = c.toolset + '|' + c.tool;
      const u = used.get(key) || { toolset: c.toolset, tool: c.tool, count: 0, ok: 0, argKeys: new Set() };
      u.count++; if (c.ok) { u.ok++; c.argKeys.forEach(k => u.argKeys.add(k)); }
      used.set(key, u);
    }
  }
}
const schemaOf = (toolset, tool) => (schemas.get(toolset) && schemas.get(toolset).get(tool)) || (fallback.get(toolset) && fallback.get(toolset).get(tool)) || null;

// 2. compact type notation: ref(Class) for {refPath}, T[] for arrays, {a,b} for small structs
function typeOf(s) {
  if (!s) return '?';
  if (s.type === 'object' && s.properties && s.properties.refPath) {
    const title = String(s.title || '');
    return 'ref(' + (/^Class@/.test(title) ? 'class:' : '') + title.split(/[./]/).pop() + ')';
  }
  if (s.type === 'array') return typeOf(s.items) + '[]';
  if (s.enum) return s.enum.slice(0, 6).join('|') + (s.enum.length > 6 ? '|...' : '');
  if (s.type === 'object' && s.properties) {
    const keys = Object.keys(s.properties);
    return '{' + keys.slice(0, 5).join(',') + (keys.length > 5 ? ',...' : '') + '}';
  }
  return Array.isArray(s.type) ? s.type.join('|') : (s.type || 'any');
}

// 3. write the sheet
const groups = new Map();
const failed = [...used.values()].filter(u => u.ok === 0).map(u => u.toolset.split('.').pop() + '.' + u.tool);
for (const u of [...used.values()].filter(u => u.ok > 0).sort((a, b) => a.toolset.localeCompare(b.toolset) || a.tool.localeCompare(b.tool))) {
  (groups.get(u.toolset) || groups.set(u.toolset, []).get(u.toolset)).push(u);
}
const notFound = [];
const lines = [
  '# Unreal MCP tool sheet (generated)',
  '',
  `Generated ${new Date().toISOString().slice(0, 10)} by \`node Tools/unreal/mcp-sheet.js\` from ${runs} bp runs of tasks ${fromTask}-${toTask}; do not edit by hand, rerun after an engine or plugin update.`,
  'Call: `mcp__unreal__call_tool` with `toolset_name` (full name below), `tool_name` (bare name), `arguments` (object). A `ref(X)` argument is `{"refPath": "/Game/Path/Asset.Asset"}`; a widget is `<WBP refPath>:WidgetTree.<Name>`, a slot `<widget refPath>.<SlotName>`. `values` of set_properties is a JSON string.',
  'Format: `tool(required: type, ...) [optional, ...] xN` (N = calls in those runs). Use describe_toolset only for a tool missing here.',
];
for (const [toolset, tools] of groups) {
  lines.push('', '## ' + toolset);
  for (const u of tools) {
    const s = schemaOf(toolset, u.tool);
    let sig;
    if (s) {
      const props = s.properties || {}, req = s.required || [];
      const opt = Object.keys(props).filter(k => !req.includes(k));
      sig = `${u.tool}(${req.map(k => k + ': ' + typeOf(props[k])).join(', ')})` + (opt.length ? ` [${opt.join(', ')}]` : '');
    } else {
      notFound.push(toolset + '.' + u.tool);
      sig = `${u.tool}(shape not found; keys of successful calls: ${[...u.argKeys].join(', ') || 'none'})`;
    }
    lines.push(`- ${sig} x${u.count}`);
  }
}
if (failed.length) lines.push('', 'Never succeeded in those runs (wrong name, do not call): ' + failed.map(x => '`' + x + '`').join(', ') + '.');
const text = lines.join('\n') + '\n';
fs.writeFileSync(outFile, text);
console.log(`${outFile.replace(repo + path.sep, '').replace(/\\/g, '/')}: ${runs} bp runs, ${used.size - failed.length} tools listed, ${text.length} chars (~${Math.round(text.length / 4)} tokens)` +
  (notFound.length ? `; shape not found: ${notFound.join(', ')}` : '; every shape found'));
