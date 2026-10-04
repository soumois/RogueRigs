---
id: task-021-02-bp-shooter
description: "BP_Shooter grey-box (acid green, elbow cannon), red laser on aim, red shot visual, spawner weights 70/15/15, PIE check."
parent: task-021-shooter
kind: task
status: done
---
# Shooter Blueprint

- goal: the shooter is visible and readable in PIE.
- agent: bp
- depends_on: task-021-01-shooter-cpp
- inputs: [[enemy-shooter]], [[color-code]], [[readability-rules]], [[bp-chaser]], [[bp-enemy-spawner]], [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]].
- expected_output: `BP_Shooter` from the C++ class: acid-green body, elbow arm cannon, red eyes, yellow logo plate (rigid primitives); laser beam shown between OnAimStart and OnAimEnd; red projectile Blueprint or material; `BP_EnemySpawner` weights chaser 70 / kamikaze 15 / shooter 15; new doc `bp-shooter`, [[bp-enemy-spawner]] updated. PIE with the cheat property route: shooters appear, keep distance, laser then shot, shots stop on rock; no log error.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: sonnet
- tokens: input 154, cache write 138006, cache read 6637132, output 39197
- runs: 2
- milestone: M2
- result: BP_Shooter (acid-green primitives MI_GB_Shooter, elbow cannon, red eyes, yellow plate; MuzzlePoint 58 uu above floor; LaserBeam cube MI_GB_Laser; XP pickup; projectile BP_ShooterProjectile red sphere), BP_EnemySpawner weights 70/15/15 (placed L_ArenaM1 instance showed 85/15, reset to inherit, map unchanged on disk: verify in 021-03). PIE: 25 shooters spawned, held 900–1000 uu, LaserBeam visible while aiming then off, shots spawned and hit near the miner; rock stop not observed (C++ tests cover it); no game log error. Docs bp-shooter (new), bp-enemy-spawner, blueprints-index. Commits 71651c5, 73498b3.

## Relations
- [[task-021-shooter]] — parent global task.
