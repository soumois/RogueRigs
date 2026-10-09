---
id: task-066-async-flow-field
description: "Global task: flow field rebuilt on a worker thread with a double buffer; robots follow a new path within 1–2 frames under load."
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
- next_step: task-066-02-perf-check
- result: Done. Flow field on a UE::Tasks worker with a double buffer: FRRFlowFieldData works on a walkable-cell snapshot, one job at a time, finished job swapped on the game thread without waiting, openings queued for the next job, rr.FlowField.Async (default 1) with the time-sliced fallback at 0. Headless 67 uu: goal age max 1 frame (was 33–38), game-thread flow cost 0.07 / 0.11 / 0.36 ms avg / p95 / max, worker rebuild 4.6–6.3 ms. Flow-field tests now pace each tick to 1/60 s real time (limits unchanged). Build 0, 195/195. No -game measurement (owner deferred). Commits 131d67a, a68f8b6.

## Subtasks
1. task-066-01-async-flow-field — cpp
2. task-066-02-perf-check — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-066-01-async-flow-field]] — step 1, cpp.
- [[task-066-02-perf-check]] — step 2, cpp.
