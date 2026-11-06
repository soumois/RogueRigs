// The agent records of vault/scripts/usage-snapshot.json without their debug traces (plannerTrace, agentTrace), as one JSON array.
// Needed because the board's module cannot read a file over 4 MiB, and the snapshot passed that once agent-usage.js
// started keeping --agent-debug traces.
// Usage: node snapshot_agents.js <usage-snapshot.json>  ->  JSON array on stdout.
const fs = require('fs')

const file = process.argv[2]
if (!file) {
  console.error('usage: node snapshot_agents.js <usage-snapshot.json>')
  process.exit(2)
}
const records = (JSON.parse(fs.readFileSync(file, 'utf8')).records || []).filter(r => r.kind === 'agent')
process.stdout.write(JSON.stringify(records.map(({ plannerTrace, agentTrace, ...r }) => r)))
