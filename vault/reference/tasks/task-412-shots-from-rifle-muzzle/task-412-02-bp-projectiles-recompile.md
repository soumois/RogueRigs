---
id: task-412-02-bp-projectiles-recompile
description: "Bp: recompile, check and save BP_Projectile, BP_ShotgunPellet, BP_TurretBullet, BP_ShooterProjectile after the capsule root change"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T21:34:45Z
- waiting_for: none
- result: open

## Relations
- [[task-412-shots-from-rifle-muzzle]] — parent global task.
