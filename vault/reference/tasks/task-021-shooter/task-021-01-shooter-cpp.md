---
id: task-021-01-shooter-cpp
description: "Shooter class: keeps 4 cells away, grid LOS gating, repositioning search, on-screen-only fire, 0.6 s laser telegraph, enemy projectile; spawner entry; tests."
parent: task-021-shooter
kind: task
status: todo
---
# Shooter C++

- goal: the shooter works in C++.
- agent: cpp
- depends_on: task-020-04-bp-check
- inputs: [[enemy-shooter]], [[enemies-index]], [[sys-enemies]], [[sys-enemy-ai]], [[sys-flow-field]], [[sys-grid]], [[sys-projectiles]], [[sys-damage]], [[sys-spawner]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: shooter enemy class on the shared robot base (stats row in the enemy CSV: hp 40, speed 300, damage 12, interval 2 s, telegraph 0.6 s, projectile 1200 uu/s, range 1200, preferred 800 ± 200, xp 2, knockback resistance 0.2); behavior: approach or back off to the preferred ring along the flow field, fire only with grid line of sight (ARRArenaGrid::HasLineOfSight) and while inside the camera view, else search nearby reachable cells with LOS inside the ring and move there; telegraph state with BlueprintImplementableEvents (OnAimStart(Target), OnAimEnd, OnFire) for the laser; enemy projectile that damages only the miner and stops on rock blocks without damaging them; spawner class entry; [[sys-enemies]], [[sys-enemy-ai]] (framework decision: C++ grid search), [[sys-projectiles]] updated. Tests `RogueRigs.Shooter.*`.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-021-shooter]] — parent global task.
