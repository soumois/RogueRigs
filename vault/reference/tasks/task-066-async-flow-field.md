---
id: task-066-async-flow-field
description: "Global task: Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load."
parent: tasks-index
kind: task
status: in-progress
---
# Flow field on a worker thread

- request: "Testons d’abord le thread séparé, tous les ordinateurs ont cette possibilité maintenant." (after task-063: goal rebuild 33–38 frames late under a normal player load) (owner, 2026-10-07)
- goal: Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless, BudgetAt67uu included, without loosened limits; measurement reported; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-066-01-async-flow-field
- result: open

## Subtasks
1. task-066-01-async-flow-field — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-066-01-async-flow-field]] — step 1, cpp.
