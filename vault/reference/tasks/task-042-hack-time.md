---
id: task-042-hack-time
description: "Global task: Console hack time 20 → 16 s."
parent: tasks-index
kind: task
status: in-progress
---
# Hack time −20 %

- request: "Réduire de 20% le temps de piratage des commandes." (owner, 2026-10-07)
- goal: Console hack time 20 → 16 s.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-042-01-hack-time
- result: open

## Subtasks
1. task-042-01-hack-time — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-042-01-hack-time]] — step 1, cpp.
