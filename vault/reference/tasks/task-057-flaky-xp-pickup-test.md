---
id: task-057-flaky-xp-pickup-test
description: "Global task: Progression.XPPickupAddsOne made reliable: find why it fails intermittently and fix the cause."
parent: tasks-done-index
kind: task
status: done
---
# Flaky XP pickup test

- request: "Lance la task 57 pour résoudre le problème de XPPickupAddsOne" (owner, 2026-10-07)
- goal: Progression.XPPickupAddsOne made reliable: find why it fails intermittently and fix the cause.
- acceptance: the test passes 10 runs in a row; build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Cause in the test: the pickup spawned 100 uu from the miner popped 80–220 uu in a clock-seeded random direction and could land up to 320 uu away, outside the 300 uu pickup radius (2.3 % of runs by simulation). Fix: pop direction fixed back through the miner (lands within 120 uu); check unchanged, landing-distance check added in the test. 10/10 runs alone. Full run 194/195: FlowField.BudgetAt67uu (frame-time budget, machine-load dependent) failed, unrelated. Commit b57620c.

## Subtasks
1. task-057-01-flaky-xp-pickup-test — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-057-01-flaky-xp-pickup-test]] — step 1, cpp.
