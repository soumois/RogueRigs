---
id: task-066-async-flow-field
description: "Global task: flow field rebuilt on a worker thread with a double buffer; robots follow a new path within 1–2 frames under load."
parent: tasks-done-index
kind: task
status: done
---
# Flow field on a worker thread

- request: "Testons d’abord le thread séparé, tous les ordinateurs ont cette possibilité maintenant." (after task-063: goal rebuild 33–38 frames late under a normal player load) (owner, 2026-10-07)
- goal: Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless, BudgetAt67uu included, without loosened limits; measurement reported; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. In game at 67 uu on the owner's normal profile (no 60 Hz pacing this time, idle CPU 0–23 %): all scenarios within 16.6 / 25 ms (p99 at most 14.05 ms); worker flow-field cost on the game thread about 0.03–0.05 ms avg, 0.27 ms max while digging; Async 1 vs 0 within noise (0.2–0.6 ms); one 57.9 ms frame in dig 400 not from the game thread; goal age not logged by the stress harness. With task-066-01: headless goal age 1 frame. Commit 39c10db.

## Subtasks
1. task-066-01-async-flow-field — cpp
2. task-066-02-perf-check — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-066-01-async-flow-field]] — step 1, cpp.
- [[task-066-02-perf-check]] — step 2, cpp.
