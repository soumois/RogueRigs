---
id: sys-weapons-shotgun
description: "Shotgun pellet model (task-024-02): damage spec per pellet, spread, FireOnce. Open to change shotgun behaviour."
parent: sys-weapons
kind: doc
---
# Weapons — shotgun

- Shotgun pellets (task-024-02, `ARRShotgun::FireOnce`): one `MakeDamageSpec()` per shot, then `PelletCount` pellets of `ProjectileClass` spawned through `SpawnShot` (-> `ARRProjectile::SpawnProjectile`) at one `GetShotPlacement()` read per shot (task-412-01, e1e65788: the hand weapon Muzzle, capsule, each pellet with its own miner-to-Muzzle sweep, [[sys-weapons]]), flat, at yaw offsets spread evenly from −`SpreadDeg` to +`SpreadDeg` around the aim (`GetPelletYawOffset(i)`, no random), speed `ProjectileSpeed`, lifetime `Range / ProjectileSpeed`, `MaxHits` = `GetPierceCount()`, knockback = `KnockbackForce` per robot hit along the pellet's flight ([[sys-projectiles]], [[sys-enemies]]). Each pellet hit rolls its own critical ([[sys-damage]]); pellets stop on rock and damage barrels like rifle bullets. Then `GetShotCount()`, `GetLastPelletsSpawned()` and the BlueprintImplementableEvent `OnShotFired(Origin, Direction, FanSpreadDeg)` for the muzzle flash (`FanSpreadDeg` = `SpreadDeg`; UHT forbids a parameter shadowing the member). The legacy `OnWaveFired` and `GetWaveClearDistance` stubs were removed in task-024-06 ([[bp-shotgun]] no longer uses them; it loads and fires in `Integration.ShotgunPelletsPierceRobots`).
- First drawn frame (task-413-01, 148676a4): each pellet goes through `SpawnShot`, so it is held one frame like a rifle bullet ([[sys-projectiles]]). Not verified in game for the shotgun.

## Relations
- [[sys-weapons]] — parent hub: summary and verdict.
- [[sys-projectiles]] — referenced above.
- [[sys-enemies]] — referenced above.
- [[sys-damage]] — referenced above.
- [[bp-shotgun]] — referenced above.
