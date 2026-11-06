---
id: task-483-01-budget-alert
description: "Doc: Print a non-blocking alert at closing when the executor run exceeds the budget of its level."
parent: task-483-executor-budget-alert-traces
kind: task
status: in-progress
---
# Budget alert at closing and executor traces: doc

- goal: Print a non-blocking alert at closing when the executor run exceeds the budget of its level.
- agent: doc
- depends_on: none
- inputs: task-482-verification-levels (vault/reference/tasks/task-482-verification-levels.md), agent-executor (vault/reference/agents/agent-executor.md), cost-log (vault/reference/production/cost-log.md), vault/scripts/task-cost.js, vault/scripts/record-cost.js, vault/scripts/task-next.js
- expected_output: see done_when
- done_when: A1 to A5 of the global pass, shown by a run over budget, a run under budget and a global without a level, with test data removed afterwards; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T22:48:40Z
- waiting_for: none
- result: open

## Relations
- [[task-483-executor-budget-alert-traces]] — parent global task.
