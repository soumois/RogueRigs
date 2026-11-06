---
id: task-483-01-budget-alert
description: "Doc: Print a non-blocking alert at closing when the executor run exceeds the budget of its level."
parent: task-483-executor-budget-alert-traces
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-483
- usage: included in the executor run of task-483
- runs: 1
- milestone: M2
- started: 2026-11-06T22:48:40Z
- waiting_for: none
- result: Non-blocking ALERT printed by record-cost.js at each closing (task-cost.js budgetAlert), comparing the executor run so far to the budget of the level, read from the table of verification-levels (one edit changes it). Shown on a throwaway vault copy with task-next.js, real commits: over on turns only, on units only, on a task with 2 subtasks (alert at both closings) and with 1; at exactly the budget (90 turns, 1.50 M) no alert; no level with the same figures no alert; budget edited in the table to 5 turns: alert at 5. All exit 0 and same four files changed in the closing commit with and without alert. Copy deleted, so no test data left. agent-executor cost_alerts, runbook and cost-log updated. A global with an unknown level gets an ALERT saying it was not compared; never blocks.

## Relations
- [[task-483-executor-budget-alert-traces]] — parent global task.
