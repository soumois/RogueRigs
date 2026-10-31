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
- projectile: projectile; each bullet pierces 4 robots (base, like DRGS Deepcore GK2; owner 2026-10-05); piercing is an upgrade stat ([[upgrades]]).
- effect_on_hit: damage; critical hits possible.
- blocked_by_rock: yes
- stats:
  - fire_rate 1.25 bursts/s, burst_count 3, burst_duty 0.2 (owner values 2026-10-02; bullets 0.08 s apart, 0.64 s pause);
  - damage 10 per bullet, crit chance 5 %, range 2000 uu (10 cells), spread ±2°, projectile speed 3000 uu/s, magazine 30 bullets, reload 1.5 s (provisional, owner 2026-10-01);
  - crit damage: ×1.5 base.
- burst timing: see [[sys-auto-fire]].
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool (yellow rarity): 4-direction fire.
- vfx_sfx: white-gold shots ([[color-code]]); muzzle, impact and sound open.
- look and asset: the rifle held by the miner in `vault/assets/rigs-lineup.jpg`, white + teal + rust salvaged plates, dark gunmetal grip, barrel and muzzle; source image = a four-view turnaround (left, right, front, back) the owner generates in ChatGPT from the prompt in art-prompts-burst-rifle, then Meshy (provisional, owner 2026-10-30, task-387); game mesh from the owner Meshy "Aqua Wasteland Carbine" (2026-10-30, tasks 394 and 395, files in `burst-rifle-mesh`): overall length 60 cm stock to muzzle, muzzle toward +X, top +Z, pivot at the pistol grip hand hold, 9,892 triangles as delivered, no decimation, four 2048 maps re-baked (provisional, owner 2026-10-30, task-394); held in the miner right hand at socket `WeaponSocket_R`, left hand on the handguard socket `LeftHandGrip` by IK; shots leave from the rifle Muzzle, its real position height included, flying flat parallel to the aim; their collision is a vertical capsule from 20 uu above the floor up to the Muzzle height, so they stop on rock blocks and hit robots without touching the floor; a rock or robot between the miner and the Muzzle is checked first (provisional, owner 2026-10-31, task-412; tasks 400 and 401 kept floor + 50 uu in front of the miner); the Muzzle position stays the owner's to set by hand (owner 2026-10-31, task-412 Q6); in the hand the mesh sits at relative location (0, 0, 0) and uniform scale 1.25 on the socket, pistol grip in the right hand; the rifle keeps the place the owner chose in task-404 because the right hand moved down onto the grip in the animation (provisional, owner 2026-10-31, task-409); it shows 75 cm long in game, the mesh asset stays 60 cm (task-404); the rifle follows the right hand in the animations: a firing animation that moves the hand moves the rifle with it (provisional, owner 2026-10-31, task-410); the right hand offset moves the hand on the rifle, the rifle keeping its place on screen (provisional, owner 2026-10-31, task-411).
- milestone: M1

- Player projectiles (rifle bullets, shotgun pellets, turret bullets) collide with a 14 uu sphere, larger than they look (owner 2026-10-06 fourth test; was 12, before that 10); robot shots keep 10 uu. Shots fired by the miner's weapons from the rifle Muzzle keep the 14 uu radius but stretch vertically into the capsule of the shot origin line above (provisional, owner 2026-10-31, task-412); turret bullets and robot shots unchanged.

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[burst-rifle-mesh]] — the Meshy mesh of this weapon: files, measures.
