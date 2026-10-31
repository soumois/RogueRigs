---
id: task-400-05-shotgun-cube-removal
description: "Bp: remove the grey ShotgunMesh cube from BP_Shotgun; pellets keep the shared shot origin."
parent: task-400-burst-rifle-in-hand
kind: task
status: done
---
# Burst rifle 3D mesh in the miner right hand: bp shotgun cube

- goal: BP_Shotgun: delete the ShotgunMesh component (grey cube 50x14x14 uu), owner Q2 answer "Tu retires le cube, les autres tirs partiront du même endroit"; Muzzle, MuzzleFlash, attachment (capsule offset) and the C++ shot origin unchanged. BP_TurretLauncher has no mesh: nothing to do.
- agent: bp
- depends_on: none
- inputs: bp-shotgun (vault/reference/tech/blueprints/player/bp-shotgun.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), shotgun (vault/reference/gdd/weapons/shotgun.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Blueprints/Weapons/BP_Shotgun`
- expected_output: BP_Shotgun compiled and saved without ShotgunMesh; change summary (component removed, graph references checked, PIE read-back); commit `task-400-05-shotgun-cube-removal: ...`.
- done_when: BP_Shotgun compiles with no ShotgunMesh component and no graph node referencing it; in PIE, after adding BP_Shotgun to the miner (AddWeapon), its pellets spawn at the same point as the rifle bullets (floor + about 50 uu, along the aim); existing shotgun tests still pass; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet+opus
- tokens: input 42, cache write 71760, cache read 478240, output 10331
- usage: turns 21, context start 11k peak 41k, resumes 0, 3 min, units 0.19 M
- runs: 1
- milestone: M2
- started: 2026-10-31T00:13:03Z
- waiting_for: none
- result: ShotgunMesh removed from BP_Shotgun (components now Root, MuzzleFlash, Muzzle, AutoFire, ASC); no graph reference. Headless tests RogueRigs.Shotgun 7, Weapons 12, Player 4 all pass (PelletFan checks pellets at GetShotOrigin, untouched C++). PIE pellet read-back not run, editor closed for tests. Commit 6eae970f

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
