---
id: bp-burst-rifle
description: "BP_BurstRifle, Blueprint of ARRWeapon for the burst rifle: cube mesh, muzzle position, projectile class. Open to see its defaults."
parent: blueprints-player-index
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
- verified: compiled and saved. Headless (task-007-05): timing, magazine, projectile stop on rock and dummy damage pass when spawned alone. Superseded by task-008-03: BP_Miner now uses no socket (capsule offset). Before, at `hand_r` the socket was pitched about 84°, so the rifle +X and the Muzzle offset point up: muzzle 130 uu above the floor, above the 100 uu blocks; shots would fly over rock (fix open: socket, attach rotation or Muzzle offset).

## Relations
- [[blueprints-index]] — parent index.
- [[sys-weapons]] — the C++ class it derives from.
- [[sys-automation-tests]] — RogueRigs.Weapons.* tests using it, incl. the failing muzzle height check.
