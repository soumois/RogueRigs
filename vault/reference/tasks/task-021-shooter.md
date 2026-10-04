---
id: task-021-shooter
description: "Global task: ranged shooter robot with line of sight, repositioning, red targeting laser, dodgeable shots stopped by rock; joins the spawner mix."
parent: tasks-index
kind: task
status: in-progress
---
# Shooter robot

- request: "Tu peux lancer 1 et 2. Aussi, peut être faire un check up ( et refactoring si nécessaire) du code c++ pour voir si le code est assez généralisé et lisible pour un être humain" (1 = packaging fix, 2 = shooter); values and plan confirmed "Oui" (2026-10-04).
- goal: shooters keep about 4 cells away, reposition when rock blocks the line, show a 0.6 s red laser and fire dodgeable red shots that stop on rock; they make up 15 % of spawns.
- acceptance: (1) build exits 0; all tests pass, new ones included; (2) distance keeping, LOS gating, repositioning, on-screen-only fire, telegraph timing, shots stopped by rock and harmless to robots (tests); (3) spawner mix 70/15/15 (test); (4) visible and readable in PIE; (5) docs sys-enemies, sys-enemy-ai, new Blueprint doc; lint clean; committed.
- gdd_check: Read [[enemy-shooter]], [[enemies-index]], [[color-code]], [[readability-rules]], [[destruction-rules]], [[xp-level-up]]. Open values set by the owner (2026-10-04, provisional), AI framework: C++ grid search, no EQS.
- confirmed_by_owner: yes (2026-10-04, all subtasks pre-approved)
- milestone: M2
- next_step: task-021-02-bp-shooter
- result: open

## Subtasks
1. task-021-01-shooter-cpp — cpp: shooter behavior, laser telegraph, enemy projectile, spawner entry, tests
2. task-021-02-bp-shooter — bp: BP_Shooter grey-box, laser and shot visuals, spawner weights, PIE
3. task-021-03-tests-integration — cpp: integration tests with the final Blueprints

## Relations
- [[tasks-index]] — parent index.
- [[task-021-01-shooter-cpp]] — step 1, cpp.
- [[task-021-02-bp-shooter]] — step 2, bp.
- [[task-021-03-tests-integration]] — step 3, cpp.
