---
id: task-532-02-doc
description: "Doc: Update the vault nodes that describe lint.js usage with the default output and --all."
parent: task-532-lint-quiet-output
kind: task
status: todo
---
# lint.js : sortie courte par défaut: doc

- goal: Update the vault nodes that describe lint.js usage with the default output and --all.
- agent: doc
- depends_on: task-532-01-lint-default-output
- inputs: orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md)
- expected_output: see done_when
- done_when: A7 holds: every vault node that describes lint.js output or flags states the default and --all; lint clean.
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
- [[task-532-lint-quiet-output]] — parent global task.
