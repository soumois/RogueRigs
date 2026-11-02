---
id: burst-rifle
description: "Default weapon: auto-fires projectile bursts toward the aim. Burst timing, starting values, magazine, upgradable stats and effect pool."
parent: weapons-index
kind: spec
status: approved
---
# Burst rifle

Final English name: open (one name for tag, asset and this node; "burst rifle" is the working name).

- fire_mode: auto toward aim, in bursts.
- projectile: projectile; each bullet pierces robots (base Pierce in `WeaponStats.csv` row `BurstRifle`, like DRGS Deepcore GK2; owner 2026-10-05); piercing is an upgrade stat ([[upgrades]]).
- effect_on_hit: damage; critical hits possible.
- blocked_by_rock: yes
- stats:
  - fire rate, burst count and burst duty: `WeaponStats.csv` row `BurstRifle`, columns `FireRate`, `BurstCount`, `BurstDuty` (owner values 2026-10-02);
  - damage per bullet, crit chance, crit damage, range, spread, projectile speed, magazine, reload: `WeaponStats.csv` row `BurstRifle`, columns `Damage`, `CritChance`, `CritDamage`, `Range`, `SpreadDeg`, `ProjectileSpeed`, `MagazineSize`, `ReloadTime` (provisional, owner 2026-10-01).
- burst timing: see [[sys-auto-fire]].
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool (yellow rarity): 4-direction fire.
- vfx_sfx: white-gold shots ([[color-code]]); muzzle flash NS_MuzzleFlash: white-gold, about 0.06 s, bright core about 25 uu wide and a few short sparks thrown forward along the aim, no light, attached to the shown rifle Muzzle, one per shot of every miner weapon (3 per rifle burst, 1 per shotgun volley), none for turret and robot shots (provisional, owner 2026-10-31, task-414); impact and sound open.
- look and asset: the rifle held by the miner in `vault/assets/rigs-lineup.jpg`, white + teal + rust salvaged plates, dark gunmetal grip, barrel and muzzle; source image = a four-view turnaround (left, right, front, back) the owner generates in ChatGPT from the prompt in art-prompts-burst-rifle, then Meshy (provisional, owner 2026-10-30, task-387); game mesh from the owner Meshy "Aqua Wasteland Carbine" (2026-10-30, tasks 394 and 395, files in `burst-rifle-mesh`): overall length 60 cm stock to muzzle, muzzle toward +X, top +Z, pivot at the pistol grip hand hold, 9,892 triangles as delivered, no decimation, four 2048 maps re-baked (provisional, owner 2026-10-30, task-394); held in the miner right hand at socket `WeaponSocket_R`, left hand on the handguard socket `LeftHandGrip` by IK; shots leave from the rifle Muzzle, its real position height included, flying flat parallel to the aim; their collision is a vertical capsule from 20 uu above the floor up to the Muzzle height, so they stop on rock blocks and hit robots without touching the floor; a rock or robot between the miner and the Muzzle is checked first (provisional, owner 2026-10-31, task-412; tasks 400 and 401 kept floor + 50 uu in front of the miner); the Muzzle position stays the owner's to set by hand (owner 2026-10-31, task-412 Q6); the bullet is shown at the Muzzle on its first rendered frame, never ahead of it (provisional, owner 2026-10-31, task-413); in the hand the mesh sits at relative location (0, 0, 0) and uniform scale 1.25 on the socket, pistol grip in the right hand; the rifle keeps the place the owner chose in task-404 because the right hand moved down onto the grip in the animation (provisional, owner 2026-10-31, task-409); it shows 75 cm long in game, the mesh asset stays 60 cm (task-404); the rifle follows the right hand in the animations: a firing animation that moves the hand moves the rifle with it (provisional, owner 2026-10-31, task-410); the right hand offset moves the hand on the rifle, the rifle keeping its place on screen (provisional, owner 2026-10-31, task-411).
- milestone: M1

- Player projectiles (rifle bullets, shotgun pellets, turret bullets) collide with a 14 uu sphere, larger than they look (owner 2026-10-06 fourth test; was 12, before that 10); robot shots keep 10 uu. Shots fired by the miner's weapons from the rifle Muzzle keep the 14 uu radius but stretch vertically into the capsule of the shot origin line above (provisional, owner 2026-10-31, task-412); turret bullets and robot shots unchanged.

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[burst-rifle-mesh]] — the Meshy mesh of this weapon: files, measures.
