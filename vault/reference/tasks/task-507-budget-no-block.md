---
id: task-507-budget-no-block
description: "Global task: Réglage (no planner): Hum, j'ai mal lu cette régle, si ça engendre ce genre de comportement, ça n'est pas ..."
parent: tasks-to-do-index
kind: task
status: todo
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
- next_step: task-507-01-budget-no-block
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-507-01-budget-no-block (vault/reference/tasks/task-507-budget-no-block/task-507-01-budget-no-block.md) — executor: Apply the setting change of the request and check it at light size.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-507-01-budget-no-block]] — step 1, executor.
- [[request-families]] — the family path this task follows.
