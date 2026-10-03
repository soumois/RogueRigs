---
id: bp-shotgun
description: "BP_Shotgun, Blueprint of ARRShotgun: grey-box mesh, muzzle position and the white-gold fan flash drawn on OnWaveFired; open for the VFX."
parent: blueprints-index
kind: doc
---
# BP_Shotgun

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_Shotgun
- parent_class: ARRShotgun (/Script/RogueRigs.RRShotgun)
- purpose: the shotgun weapon actor; mesh and wave flash only, the wave, damage and knockback are C++ ([[sys-weapons]]).
- components: inherited Root, Muzzle (relative location 50, 0, 0), auto-fire, ASC; added ShotgunMesh (cube 50x14x14 uu at 25, 0, 0, NoCollision, MI_GB_Shot) under the root.
- exposed_defaults: StatsRowName = Shotgun (C++ default); ProjectileClass unset (the shotgun fires no projectile); other C++ defaults kept.
- events: OnWaveFired(Origin, Direction, HalfAngleDeg, WaveRange, ClippedLength) draws a fan of 7 runtime StaticMeshComponents (engine Cube, scale (len/100, len/500, 0.02), no collision, no shadow, MI_GB_Shot white-gold), one per ray at yaw offsets -30, -20, -10, 0, 10, 20, 30 deg from the aim. Each ray length is `GetWaveClearDistance(offset)` (so the fan is cut by rock); each is placed at Origin + ray direction * length / 2 with `SetWorldTransform`, then all are destroyed after a 0.15 s Delay. Offsets are fixed for the 60 deg cone (not read from HalfAngleDeg); no fade, no sound hook.
- logic_in_bp: visual effect only (the flash)
- assets: engine Cube mesh, MI_GB_Shot
- owner_agent: agent-bp
- verified: compiled, saved. PIE on L_ArenaM1 (task-015-02): waves fire about every 1.25 s (WaveCount 14 at 18 s), `LastWave` origin (8100, 8100, 50), half angle 30, range 700, ClippedLength 700 in the open, hitCount 1 once a robot was in the cone; log shows robots killed with instigator BP_Shotgun_C_0, no runtime log error. Not verified: the flash seen on screen (no screenshot taken), the rock clipping in PIE, a measured knockback.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-weapons]] — the C++ class it derives from and the OnWaveFired event.
- [[shotgun]] — the weapon design it implements.
- [[bp-miner]] — lists it in StartingWeapons.
- [[bp-burst-rifle]] — the sibling weapon Blueprint.
