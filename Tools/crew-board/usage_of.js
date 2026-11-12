// What the crew board needs from one agent transcript (a .jsonl under ~/.claude/projects/<project>/<session>/subagents):
// the tokens it has used so far (each API message counted once, the last entry of its id wins, as vault/scripts/agent-usage.js
// does), its last tool, the head of its first prompt (to find the task it works on) and, for a planner, the global tasks whose
// node it created, and whether a tool call is still waiting for its result (a long build or capture: the agent is not idle).
// Also run on a main session transcript (<project>/<session>.jsonl) for the relay's tokens. Needed because the board's module cannot read a file over 4 MiB, and an executor's transcript passes that.
// Usage: node usage_of.js <transcript.jsonl>  ->  one JSON line on stdout.
const fs = require('fs')

const file = process.argv[2]
if (!file) {
  console.error('usage: node usage_of.js <transcript.jsonl>')
  process.exit(2)
}

const byId = new Map()
const created = new Set()
let tool = ''
const open = new Set()
let head = ''
const models = new Set()
let first = 0
let last = 0

for (const line of fs.readFileSync(file, 'utf8').split('\n')) {
  if (!line) continue
  let j
  try {
    j = JSON.parse(line)
  } catch {
    continue
  }
  const m = j.message
  if (!m) continue
  const at = Date.parse(j.timestamp || '')
  if (at) {
    first = first || at
    last = at
  }
  if (!head && j.type === 'user') head = (typeof m.content === 'string' ? m.content : JSON.stringify(m.content)).slice(0, 8000)
  if (j.type === 'assistant' && m.usage) {
    byId.set(m.id || j.uuid, m.usage)
    if (m.model) models.add(m.model)
    const tools = (Array.isArray(m.content) ? m.content : []).filter(b => b.type === 'tool_use')
    if (tools.length > 0) tool = tools[tools.length - 1].name
    for (const t of tools) open.add(t.id)
  }
  if (j.type === 'user' && Array.isArray(m.content)) {
    for (const b of m.content) {
      if (b.type !== 'tool_result') continue
      open.delete(b.tool_use_id)
      const text = typeof b.content === 'string' ? b.content : Array.isArray(b.content) ? b.content.map(x => x.text || '').join('\n') : ''
      // task-create.js prints "created task-NNN-slug" for each global task it writes
      for (const x of text.matchAll(/^created task-(\d{3})-[a-z0-9-]+/gm)) created.add(x[1])
      for (const x of text.matchAll(/File created successfully at: [^\n"]*?reference[\\/]+tasks[\\/]+task-(\d{3})-[a-z0-9-]+\.md/g)) created.add(x[1])
    }
  }
}

const tokens = { input: 0, cacheWrite: 0, cacheRead: 0, output: 0 }
for (const u of byId.values()) {
  tokens.input += u.input_tokens || 0
  tokens.cacheWrite += u.cache_creation_input_tokens || 0
  tokens.cacheRead += u.cache_read_input_tokens || 0
  tokens.output += u.output_tokens || 0
}
console.log(JSON.stringify({ tokens, tool, head, created: [...created], models: [...models], seconds: Math.round((last - first) / 1000), pending: open.size > 0 }))
