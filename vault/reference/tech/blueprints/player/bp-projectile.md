---
id: bp-projectile
description: "BP_Projectile, Blueprint of ARRProjectile: small sphere mesh with MI_GB_Shot. Open to see its defaults."
parent: blueprints-player-index
kind: doc
---
# BP_Projectile

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_Projectile
- parent_class: ARRProjectile (/Script/RogueRigs.RRProjectile)
- purpose: projectile fired by BP_BurstRifle; visuals only
- components: inherited CollisionSphere (radius 10) and movement; added ShotMesh (sphere radius 7.5 uu, NoCollision)
- exposed_defaults: C++ defaults kept; ShotMesh material MI_GB_Shot
- events: none
- logic_in_bp: none
- assets: engine sphere mesh, MI_GB_Shot (white-gold)
- owner_agent: agent-bp
- verified: compiled and saved; PIE behaviour check left to task-007-05.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-projectiles]] — the C++ class it derives from.
