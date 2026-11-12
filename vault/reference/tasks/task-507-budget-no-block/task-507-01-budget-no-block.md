---
id: task-507-01-budget-no-block
description: "Executor: Apply the setting change of the request and check it at light size."
parent: task-507-budget-no-block
kind: task
status: in-progress
---
# Réglage: Hum, j'ai mal lu cette régle, si ça engendre ce genre de comportement, ça n'est pas ...: executor

- goal: Apply the setting change of the request and check it at light size.
- agent: executor
- depends_on: none
- inputs: request-families (vault/reference/agents/request-families.md), verification-levels (vault/reference/agents/verification-levels.md)
- expected_output: see done_when
- done_when: A1 to A3 of the global pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-12T04:43:01Z
- waiting_for: none
- result: open

## Relations
- [[task-507-budget-no-block]] — parent global task.
