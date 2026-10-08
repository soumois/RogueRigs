---
id: task-056-bonus-chest
description: "Global task: Bonus chest: one per level at 60 % alert, 8 s hack, XP, ore and a lucky upgrade choice."
parent: tasks-index
kind: task
status: in-progress
---
# Bonus chest

- request: "Prévois le coffre bonus" (owner, 2026-10-07)
- goal: Bonus chest: one per level at 60 % alert, 8 s hack, XP, ore and a lucky upgrade choice.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-056-02-bp-bonus-chest
- result: open

## Subtasks
1. task-056-01-bonus-chest — cpp
2. task-056-02-bp-bonus-chest — bp

## Relations
- [[tasks-index]] — parent index.
- [[task-056-01-bonus-chest]] — step 1, cpp.
- [[task-056-02-bp-bonus-chest]] — step 2, bp.
