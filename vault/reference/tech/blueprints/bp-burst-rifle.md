---
id: bp-burst-rifle
description: "BP_BurstRifle, Blueprint of ARRWeapon for the burst rifle: cube mesh, muzzle position, projectile class. Open to see its defaults."
parent: blueprints-index
kind: doc
---
# BP_BurstRifle

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle
- parent_class: ARRWeapon (/Script/RogueRigs.RRWeapon)
- purpose: the burst rifle weapon actor; visuals and references only
- components: inherited Root, Muzzle (relative location 40, 0, 0), auto-fire, ASC; added RifleMesh (cube 60x10x10 uu, NoCollision) under the root
- exposed_defaults: StatsRowName = BurstRifle; ProjectileClass = BP_Projectile_C; other C++ defaults kept
- events: none
- logic_in_bp: none
- assets: engine cube mesh, MI_GB_Shot
- owner_agent: agent-bp
- verified: compiled and saved; PIE behaviour check left to task-007-05.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-weapons]] — the C++ class it derives from.
