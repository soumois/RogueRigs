---
id: bp-projectile
description: "BP_Projectile, Blueprint of ARRProjectile, the rifle bullet: small sphere with the green M_PlayerLaserShot and the green impact burst. Open for its defaults."
parent: blueprints-player-index
kind: doc
---
# BP_Projectile

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_Projectile
- parent_class: ARRProjectile (/Script/RogueRigs.RRProjectile)
- purpose: projectile fired by BP_BurstRifle; mesh only, collision and shot placement are C++ ([[sys-projectiles]])
- components: inherited root CollisionCapsule (default 14 x 14 uu, half height resized per shot to the Muzzle placement, task-412-01 commit e1e65788; the earlier "radius 10" wording was stale) and ProjectileMovement; added ShotMesh (sphere radius 7.5 uu, attached to CollisionCapsule at relative (0, 0, 0), scale 0.15). Observed discrepancy (task-412-02 read-back, awaiting owner decision): ShotMesh collision reads QueryAndPhysics, BlockAllDynamic, pre-existing since task-007-04, not NoCollision as this node claimed; C++ `DisableVisualCollision` sets it to NoCollision at spawn, so the stored value has no runtime effect
- exposed_defaults: ShotMesh OverrideMaterials M_PlayerLaserShot ([[bp-player-laser-shot-material]], task-464-01, commit 03a10507; was MI_GB_Shot white-gold); ImpactSparkSystem NS_ShotImpactBurstGreen ([[bp-shot-impact-burst]], task-465-01, commit ece043ab; was NS_ShotImpactSparkGreen, task-464-01, and the C++ default NS_ShotImpactSpark before); other C++ defaults kept
- events: none
- logic_in_bp: none
- assets: engine sphere mesh, M_PlayerLaserShot (green), NS_ShotImpactBurstGreen
- owner_agent: agent-bp
- verified: compiled and saved; PIE behaviour check left to task-007-05. task-412-02: recompiled, components exactly CollisionCapsule, ShotMesh, ProjectileMovement (editor read-back, no asset change, no commit); the stored ShotMesh parent name on disk may still say CollisionSphere, resolved to the root at load.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-projectiles]] — the C++ class it derives from.
- [[bp-player-laser-shot-material]] — its green shot material.
- [[bp-shot-impact-burst]] — its green impact burst.
- [[bp-shot-impact-spark]] — its former impact sparks.
