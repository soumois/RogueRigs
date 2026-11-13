---
id: task-534-02-doc
description: "Doc: Authorise the script in _navigation and name it where the planner reads its tools."
parent: task-534-descent-script
kind: task
status: in-progress
---
# Script de descente : gloses d'un index et de ses sous-index en un appel: doc

- goal: Authorise the script in _navigation and name it where the planner reads its tools.
- agent: doc
- depends_on: task-534-01-descend-script
- inputs: _navigation (vault/_navigation.md), task-spec (vault/reference/agents/task-spec.md), agent-planner (vault/reference/agents/agent-planner.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md)
- expected_output: see done_when
- done_when: A5 and A6 hold; the _navigation line keeps "never search the vault" intact; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T18:26:16Z
- waiting_for: none
- result: open

## Relations
- [[task-534-descent-script]] — parent global task.
