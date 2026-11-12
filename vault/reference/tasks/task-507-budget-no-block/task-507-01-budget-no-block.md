---
id: task-507-01-budget-no-block
description: "Executor: Apply the setting change of the request and check it at light size."
parent: task-507-budget-no-block
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-507
- usage: included in the executor run of task-507
- runs: 1
- milestone: M2
- started: 2026-11-12T04:43:01Z
- waiting_for: none
- result: Budget overrun no longer waits for the owner: task-cost.js budgetAlert now compares the executor runs with the figures of the previous closing (previousUsage, read by record-cost.js before the executor_usage line is rewritten); the closing that crosses the budget prints a BUDGET handover request (fresh executor with subtasks left), later closings an information line, and only the closing that crosses alerts.budgetStopFactor = 5 (cost-rates.json) times the budget prints BUDGET STOP (handover + wait-answers). Every BUDGET line asks for the overrun and its cause in result and the final report. budget_grants removed from task-status.js and the docs. Rule text updated in execution-protocol 5.10, verification-levels, agent-executor, orchestrator-runbook, cost-log, .claude/agents/executor.md, .claude/commands/task.md. Light check: budgetAlert run on 7 synthetic cases (under, crossing 1x by turns and by units, still over, crossing 5x, 5x and 1x in one closing, past 5x) gives the expected line each time; previousUsage reads task-505 (210 turns, 3.70 M). Commit b19288c2.

## Relations
- [[task-507-budget-no-block]] — parent global task.
