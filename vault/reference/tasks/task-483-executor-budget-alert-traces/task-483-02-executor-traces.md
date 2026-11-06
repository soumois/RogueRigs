---
id: task-483-02-executor-traces
description: "Doc: Keep executor traces in agent-usage.js under the size limit, with usage-agents.json still free of them."
parent: task-483-executor-budget-alert-traces
kind: task
status: in-progress
---
# Budget alert at closing and executor traces: doc

- goal: Keep executor traces in agent-usage.js under the size limit, with usage-agents.json still free of them.
- agent: doc
- depends_on: task-483-01-budget-alert
- inputs: vault/scripts/agent-usage.js, vault/scripts/usage-snapshot.json, vault/scripts/usage-agents.json
- expected_output: see done_when
- done_when: A6 to A8 of the global pass on a real executor run, with the two sizes written in result; the crew-board mod still reads usage-agents.json; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T22:54:19Z
- waiting_for: none
- result: open

## Relations
- [[task-483-executor-budget-alert-traces]] — parent global task.
