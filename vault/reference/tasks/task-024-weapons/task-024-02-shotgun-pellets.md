---
id: task-024-02-shotgun-pellets
description: "Replace the shotgun cone wave with 7 projectile pellets toward the aim: spread, pierce 3, knockback per pellet, rock stop, crit per pellet; tests."
parent: task-024-weapons
kind: task
status: done
---
# Shotgun pellets

- goal: the shotgun fires pellets like DRGS but toward the aim.
- agent: cpp
- depends_on: task-024-01-rifle-pierce
- inputs: [[shotgun]], [[sys-weapons]], [[sys-projectiles]], [[sys-damage]], [[sys-enemies]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: ARRShotgun fires 7 pellets per shot from the shared projectile path (SpawnProjectile), spread ±15° around the aim, range 700 uu, 6 damage each, pierce 3, knockback 250 uu/s per pellet hit scaled by resistance, magazine 2, 1 shot per 0.5 s, reload 2.5 s, one crit roll per pellet hit; OnWaveFired replaced by OnShotFired(Origin, Direction, Spread) for the muzzle flash; the old cone code removed; WeaponStats.csv updated; docs sys-weapons, bp-shotgun note for the bp agent. Tests `RogueRigs.Shotgun.*` rewritten (pellet count and spread, pierce, rock stop, knockback, crits).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: opus
- tokens: input 94, cache write 184319, cache read 5599297, output 78585
- runs: 1
- milestone: M2
- result: ARRShotgunPellet (native default ProjectileClass); ARRShotgun fires 7 pellets in an even ±15° fan via SpawnProjectile with MaxHits = pierce and per-pellet knockback (ARRProjectile KnockbackSpeed; ApplyKnockback now accumulates in a frame); OnShotFired(Origin, Direction, FanSpreadDeg); legacy OnWaveFired/GetWaveClearDistance stubs kept until BP_Shotgun is cleaned (024-05), removal in 024-06. WeaponStats.csv: PelletCount column (ConeAngleDeg removed); shotgun 6 dmg, 2 shots/s, mag 2, reload 2.5, range 700, spread 15, knockback 250, pierce 3, pellet speed 3000 (provisional). Pellets now damage barrels. Shotgun tests rewritten; build 0, 127/127. Docs sys-weapons, sys-projectiles, sys-damage, sys-enemies-base, sys-automation-tests, bp-shotgun. Commits c7aa8f4, aab6089.

## Relations
- [[task-024-weapons]] — parent global task.
