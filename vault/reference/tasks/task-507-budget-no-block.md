---
id: task-507-budget-no-block
description: "Global task: Réglage (no planner): Hum, j'ai mal lu cette régle, si ça engendre ce genre de comportement, ça n'est pas ..."
parent: tasks-done-index
kind: task
status: done
---
# Réglage: Hum, j'ai mal lu cette régle, si ça engendre ce genre de comportement, ça n'est pas ...

- request: Owner 2026-11-12, verbatim: "Hum, j'ai mal lu cette régle, si ça engendre ce genre de comportement, ça n'est pas possible. je lance souvent des grosses tâches durant la nuit, ça ne peut pas bloquer pour des choses comme ça. On pourrait limiter à 5 fois le budget". Family réglage, picked or forced at the entry; node written by task-reglage.js before the executor run, no planner.
- goal: The setting the owner names is changed on the existing system, nothing else.
- acceptance: A1 the change named in the request is in place, checked at light size (verification-levels); A2 no new asset, no new system, no gdd value decided by the executor; A3 lint clean, committed.
- gdd_check: Not done: réglage family, no planner (request-families). The executor stops before editing on a new asset or system, an open value or a design question, and reports for a relaunch through the planner (agent-executor wrong_family).
- verification_level: light
- family: reglage
- confirmed_by_owner: yes (2026-11-12, the request itself, réglage family)
- milestone: M2
- next_step: none
- started: 2026-11-12T04:43:01Z
- waiting_for: none
- executor_tokens: input 52, cache write 67868, cache read 1496349, output 19158
- executor_usage: runs 1, turns 26, context start 16k peak 80k, resumes 0, 3 min, units 0.33 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Budget overrun no longer waits for the owner: task-cost.js budgetAlert now compares the executor runs with the figures of the previous closing (previousUsage, read by record-cost.js before the executor_usage line is rewritten); the closing that crosses the budget prints a BUDGET handover request (fresh executor with subtasks left), later closings an information line, and only the closing that crosses alerts.budgetStopFactor = 5 (cost-rates.json) times the budget prints BUDGET STOP (handover + wait-answers). Every BUDGET line asks for the overrun and its cause in result and the final report. budget_grants removed from task-status.js and the docs. Rule text updated in execution-protocol 5.10, verification-levels, agent-executor, orchestrator-runbook, cost-log, .claude/agents/executor.md, .claude/commands/task.md. Light check: budgetAlert run on 7 synthetic cases (under, crossing 1x by turns and by units, still over, crossing 5x, 5x and 1x in one closing, past 5x) gives the expected line each time; previousUsage reads task-505 (210 turns, 3.70 M). Commit b19288c2.

## Subtasks
1. task-507-01-budget-no-block (vault/reference/tasks/task-507-budget-no-block/task-507-01-budget-no-block.md) — executor: Apply the setting change of the request and check it at light size.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-507-01-budget-no-block]] — step 1, executor.
- [[request-families]] — the family path this task follows.
