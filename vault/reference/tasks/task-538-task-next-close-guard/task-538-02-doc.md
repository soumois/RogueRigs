---
id: task-538-02-doc
description: "Doc: crew nodes describe the new closing behaviour of task-next.js"
parent: task-538-task-next-close-guard
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-538
- usage: included in the executor run of task-538
- runs: 1
- milestone: M2
- started: 2026-11-13T19:11:40Z
- waiting_for: none
- result: Commit 7a21ee73: orchestrator-script-quirks rewrites the two closing trap entries (missing next arguments, run it once) into the new behaviour: refusals before any write with Nothing changed, the dead-link refusal, and rerun after a late failure; orchestrator-runbook (task-next.js command) and execution-protocol step 5.5 state that a refused call changes nothing and that a rerun finishes a late failure. Lint clean. Global acceptance A1 to A6 checked in 01 on a throwaway task in a deleted clone, A7 here.

## Relations
- [[task-538-task-next-close-guard]] — parent global task.
