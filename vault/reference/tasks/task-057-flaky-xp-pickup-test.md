---
id: task-057-flaky-xp-pickup-test
description: "Global task: Progression.XPPickupAddsOne made reliable: find why it fails intermittently and fix the cause."
parent: tasks-index
kind: task
status: in-progress
---
# Flaky XP pickup test

- request: "Lance la task 57 pour résoudre le problème de XPPickupAddsOne" (owner, 2026-10-07)
- goal: Progression.XPPickupAddsOne made reliable: find why it fails intermittently and fix the cause.
- acceptance: the test passes 10 runs in a row; build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-057-01-flaky-xp-pickup-test
- result: open

## Subtasks
1. task-057-01-flaky-xp-pickup-test — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-057-01-flaky-xp-pickup-test]] — step 1, cpp.
