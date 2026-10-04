---
id: task-021-01-shooter-cpp
description: "Shooter class: keeps 4 cells away, grid LOS gating, repositioning search, on-screen-only fire, 0.6 s laser telegraph, enemy projectile; spawner entry; tests."
parent: task-021-shooter
kind: task
status: done
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
- model: opus
- tokens: input 100, cache write 200155, cache read 6584113, output 86555
- runs: 1
- milestone: M2
- result: ARRShooter on ARREnemy (Approach, Hold, Reposition, Aim, Dead): fires with grid LOS, within 1200 uu and on screen (reuses ARREnemySpawner::ComputeCameraViewRectangle), 0.6 s aim then shot every 2 s, aim cancelled on loss of sight; holds 800 ± 200 uu; repositions via GatherReachableCells within 6 cells every 0.5 s. MuzzlePoint and C++-driven LaserBeam; OnAimStart, OnAimEnd, OnFire; getters GetAimOrigin, GetAimTargetPoint, GetLaserLength. ARREnemyProjectile hits only the miner, no terrain damage. ARREnemy::MakeDamageSpec. EnemyStats.csv Shooter row + PreferredDistance, Tolerance, ProjectileSpeed; kamikaze ArmRange/FuseTime moved to CSV (R8, unchanged values). 6 new Shooter tests (mix 1395/298/307 of 2000); build 0, 109/109. Docs sys-enemies-shooter (new), sys-enemies, kamikaze, base, sys-enemy-ai (C++ grid search, no EQS), sys-projectiles, sys-spawner. Commits c0feb9d, 8524f23.

## Relations
- [[task-021-shooter]] — parent global task.
