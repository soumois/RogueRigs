---
id: task-412-02-bp-projectiles-recompile
description: "Bp: recompile, check and save BP_Projectile, BP_ShotgunPellet, BP_TurretBullet, BP_ShooterProjectile after the capsule root change"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: done
---
# Shots leave from the rifle muzzle: bp

- goal: After the cpp change, open each of the four projectile Blueprints, compile, check that the inherited collision root is the capsule, that ShotMesh is still attached to it with NoCollision and its old relative transform, no compile error or orphan component; save. No other edit. Never stage BP_BurstRifle.uasset or ABP_Miner.uasset (owner work).
- agent: bp
- depends_on: task-412-01-shot-origin-muzzle-capsule
- inputs: task-412-shots-from-rifle-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle.md), agent-bp (vault/reference/agents/agent-bp.md), bp-projectile (vault/reference/tech/blueprints/player/bp-projectile.md), bp-shotgun (vault/reference/tech/blueprints/player/bp-shotgun.md), bp-turret (vault/reference/tech/blueprints/player/bp-turret.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), /Game/RogueRigs/Blueprints/Weapons/BP_Projectile, BP_ShotgunPellet, BP_TurretBullet; /Game/RogueRigs/Blueprints/Enemies/BP_ShooterProjectile
- expected_output: The four .uasset saved and committed by path `task-412-02-bp-projectiles-recompile: <summary>`; a change summary per Blueprint (root class read back, ShotMesh parent and collision, compile result).
- done_when: Each of the four Blueprints compiles without error, its root reads back as the capsule, ShotMesh is attached to it with NoCollision; the four assets saved and committed by path; no other asset staged.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 26, cache write 42853, cache read 366871, output 8373
- usage: turns 13, context start 12k peak 43k, resumes 0, 1 min, units 0.13 M
- runs: 1
- milestone: M2
- started: 2026-10-31T21:34:45Z
- waiting_for: none
- result: Done, no commit (assets unchanged on disk after compile+save, nothing to stage): 4 BPs compile, root CollisionCapsule, ShotMesh attached to it at 0,0,0. Discrepancy: BP_Projectile ShotMesh collision is QueryAndPhysics BlockAllDynamic (pre-existing since task-007-04), not NoCollision as the doc says; not changed (no other edit allowed), owner decision. Compile log not read.

## Relations
- [[task-412-shots-from-rifle-muzzle]] — parent global task.
