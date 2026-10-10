---
id: task-071-robot-light-movement
description: "Global task: robots move with a light custom movement instead of CharacterMovement; game thread measured before and after. On hold."
parent: tasks-done-index
kind: task
status: done
---
# Robot light movement

- request: "je pense qu'on devrait optimiser le mouvement des robots pour essayer de gagner de la performance" (after task-068-01: CharacterMovement 4.9 ms of 12.5 ms game thread at 250 real-game robots) (owner, 2026-10-10)
- goal: Robots move with a light custom movement instead of CharacterMovement, same behaviour in play, game thread measured before and after on the real-game stress.
- acceptance: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; before/after numbers recorded in stress-grid-cells; robots still reach the miner around rock in the real-game stress (kills and damage to the miner logged); docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10), launched 2026-10-10
- milestone: M2
- next_step: none
- result: Done. Light kinematic robot movement behind rr.Enemy.LightMovement (default on): game thread -2.4 to -4.4 ms at 250 real-game robots, movement 5.56 -> 4.39 ms per frame (Insights), spikes 140-188 -> 75-100; S0 400 game 9.12 -> 7.84. Build 0, 203/204 tests (Integration.WeaponCardEquipsShotgun already failing, card text change). Commits b28e4bf, 918f154.

## Subtasks
1. task-071-01-robot-light-movement — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-071-01-robot-light-movement]] — step 1, cpp.
