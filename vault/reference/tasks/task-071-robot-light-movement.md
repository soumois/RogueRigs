---
id: task-071-robot-light-movement
description: "Global task: robots move with a light custom movement instead of CharacterMovement; game thread measured before and after. On hold."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot light movement

- request: "je pense qu'on devrait optimiser le mouvement des robots pour essayer de gagner de la performance" (after task-068-01: CharacterMovement 4.9 ms of 12.5 ms game thread at 250 real-game robots) (owner, 2026-10-10)
- goal: Robots move with a light custom movement instead of CharacterMovement, same behaviour in play, game thread measured before and after on the real-game stress.
- acceptance: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; before/after numbers recorded in stress-grid-cells; robots still reach the miner around rock in the real-game stress (kills and damage to the miner logged); docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10), launched 2026-10-10
- milestone: M2
- next_step: task-071-01-robot-light-movement
- result: open

## Subtasks
1. task-071-01-robot-light-movement — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-071-01-robot-light-movement]] — step 1, cpp.
