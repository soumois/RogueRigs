---
id: bp-shotgun
description: "BP_Shotgun and BP_ShotgunPellet, Blueprints of ARRShotgun and its pellet: grey-box mesh, muzzle flash on OnShotFired, white-gold pellet mesh."
parent: blueprints-index
kind: doc
---
# BP_Shotgun

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_Shotgun
- parent_class: ARRShotgun (/Script/RogueRigs.RRShotgun)
- purpose: the shotgun weapon actor; mesh and muzzle flash only, the pellets, damage and knockback are C++ ([[sys-weapons]]).
- cleanup (task-024-05, done): the Event OnWaveFired chain (7 runtime cubes, GetWaveClearDistance calls, Delay, destroy loop; 193 nodes) was deleted; Event OnShotFired(Origin, Direction, FanSpreadDeg) shows the MuzzleFlash component for 0.06 s. BP_ShotgunPellet (parent ARRShotgunPellet) holds ShotMesh (sphere r 4 uu, MI_GB_Shot white-gold, NoCollision) and is BP_Shotgun ProjectileClass. Still for the cpp agent: remove the legacy OnWaveFired and GetWaveClearDistance stubs.
- components: inherited Root, Muzzle (relative location 50, 0, 0), auto-fire, ASC; added ShotgunMesh (cube 50x14x14 uu at 25, 0, 0, NoCollision, MI_GB_Shot) and MuzzleFlash (sphere r 20 at 55, 0, 0, MI_GB_Shot, hidden, NoCollision) under the root.
- exposed_defaults: StatsRowName = Shotgun (C++ default); ProjectileClass = BP_ShotgunPellet_C; other C++ defaults kept.
- events: OnShotFired(Origin, Direction, FanSpreadDeg): MuzzleFlash visible, Delay 0.06, hidden. No other graph logic.
- logic_in_bp: visual effect only (the flash)
- assets: engine Cube and Sphere meshes, MI_GB_Shot
- owner_agent: agent-bp
- verified: compiled, saved. PIE (task-024-05) after taking the shotgun from a new-weapon card: ShotCount 4, LastPelletsSpawned 7, ProjectileClass BP_ShotgunPellet_C; no pellet-related log error. Flash and pellet look not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-weapons]] — the C++ class it derives from and its events.
- [[shotgun]] — the weapon design it implements.
- [[bp-miner]] — lists it in StartingWeapons.
- [[bp-burst-rifle]] — the sibling weapon Blueprint.
- [[bp-turret]] — the turret Blueprints, same grey-box style.
