---
id: task-009-chaser
description: "Global task: the chaser robot with player health, i-frames, death and game over, grid flow-field movement, enemy base, spawner, HUD and tests."
parent: tasks-index
kind: task
status: todo
---
# Chaser robot

- request: "Implémente le robot chaser, mais découpe bien les tâches, car cela inclut les points de vie et le fait d'en perdre, la mort, le déplacement des robots vers le joueur (généralise bien le code de déplacement des robots) et les frames d'invulnérabilité du joueur (plus toutes autres tâches auxquelles je n'aurais pas pensé)."
- goal: in PIE, chasers spawn outside the view, follow the grid flow field to the miner around rocks, wind up and hit for 10; the miner blinks 0.25 s invulnerable, loses health on a HUD bar, and at 0 the game is over and the level restarts after 3 s; chasers die after 3 bullets, shrink, and drop an XP cube.
- acceptance: (1) build exits 0; all `RogueRigs.*` headless tests pass, new ones included; (2) flow field: direction from any reachable cell leads to the miner, updated when a block is destroyed or the miner changes cell; (3) generic movement component used by the chaser, provider swappable; (4) chaser values from `EnemyStats.csv` row `Chaser` match [[enemy-chaser]]; (5) i-frames 0.25 s and single death proven by tests; (6) 50 chasers headless: average frame time reported; (7) docs `sys-enemy-ai`, `sys-damage`, `sys-player`, new `sys-flow-field`, `sys-enemies`, `sys-spawner`, `sys-hud`, Blueprint docs; lint clean; every subtask committed; (8) owner plays it.
- gdd_check: read [[enemies-index]], [[enemy-chaser]], [[enemy-death]], [[sys-enemy-ai]], [[sys-damage]], [[sys-gas-core]], [[miner]], [[run-structure]], [[difficulty]], [[xp-level-up]], [[perf-risks]]. Conflict: owner asked for navmesh, [[sys-enemy-ai]] chose a grid flow field → owner chose the flow field behind a generic movement component (2026-10-02). Open values set by the owner (2026-10-02): chaser stats, wind-up 0.25 s, size 0.7, i-frames 0.25 s, M1 death flow, spawn rule, M1 robot death, C++ state machine. Out: difficulty scaling (open), XP system (next task; the cube is inert), kamikaze, death debris (M3).
- confirmed_by_owner: no
- milestone: M1
- next_step: task-009-01-player-damage
- result: open

## Subtasks
1. task-009-01-player-damage — cpp: flat damage GE, i-frames, single death, enemy attribute set.
2. task-009-02-game-over-hud — cpp: game-over flow, restart, HUD base class fed by C++.
3. task-009-03-flow-field — cpp: grid BFS flow field, path-provider interface, generic enemy movement component.
4. task-009-04-enemy-chaser — cpp: `ARREnemy` base, `ARRChaser` state machine, CSV, death and XP drop.
5. task-009-05-spawner — cpp: spawner outside the camera view on reachable cells.
6. task-009-06-bp-chaser — bp: chaser, XP cube, spawner, miner blink, HUD widget, map.
7. task-009-07-tests-perf — cpp: integration tests and a 50-chaser timing run.

## Relations
- [[tasks-index]] — parent index.
- [[task-009-01-player-damage]] — step 1, cpp.
- [[task-009-02-game-over-hud]] — step 2, cpp.
- [[task-009-03-flow-field]] — step 3, cpp.
- [[task-009-04-enemy-chaser]] — step 4, cpp.
- [[task-009-05-spawner]] — step 5, cpp.
- [[task-009-06-bp-chaser]] — step 6, bp.
- [[task-009-07-tests-perf]] — step 7, cpp.
