---
id: task-538-02-doc
description: "Doc: crew nodes describe the new closing behaviour of task-next.js"
parent: task-538-task-next-close-guard
kind: task
status: todo
---
# task-next.js refuse avant d'écrire: doc

- goal: crew nodes describe the new closing behaviour of task-next.js
- agent: doc
- depends_on: task-538-01-guard
- inputs: orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md), execution-protocol (vault/reference/agents/execution-protocol.md)
- expected_output: see done_when
- done_when: A7 holds: the closing steps say that a refused call changes nothing and that a rerun finishes a late failure; the two trap entries are rewritten or removed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-538-task-next-close-guard]] — parent global task.
