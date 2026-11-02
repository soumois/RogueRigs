---
id: enemy-chaser
description: "Chaser: small yellow biped that runs at the miner and attacks in melee. The basic horde unit."
parent: enemies-index
kind: spec
status: approved
---
# Chaser

- role: basic horde unit; runs at the miner and attacks in melee.
- locomotion: biped
- rig: rigid parts, one bone per part
- parts (provisional, owner 2026-10-13): 10 rigid parts: head and torso as ONE block (red eye and logo plate on its front, as on the validated sheet vault/assets/chaser-turnaround.png), pelvis, 2 upper arms, 2 forearms with hand, 2 thighs, 2 lower legs with foot (the first list said 13 by a counting error: its items made 11, and the sheet shows head and torso as one block); the same part meshes feed the death effect ([[enemy-death]])
- model_source (provisional, owner 2026-10-14): the game mesh is the Meshy chaser (generated from four views of the validated sheet, optimised by Meshy to 3115 triangles, one object, one painted 2048 px base colour texture) with two texture corrections (dark shoulder-to-torso joints, company emblem on the hood top); it replaces the Blender part-by-part model, whose files stay in git (tag backup-chaser-monolithic and SM_Chaser_parts.fbx). The triangle budget of 3000 is exceeded by 115 (3.8 %): to confirm. The mesh is cut into the 10 rigid parts above for the animation
- size: small; 0.7 × the miner (provisional, owner 2026-10-02)
- animations (provisional, owner 2026-10-13): two clips: run (loop) and attack (wind-up `WindUp`, then the strike); death is the Niagara parts effect, hits are a flash. Technique: baked animation texture (one transform per part and per frame, played by the material), no skeletal mesh at run time; look-alike pipeline in [[asset-pipeline]]
- animations_status (provisional, owner 2026-10-14): both clips are in the game, played by the material from the baked texture (run: 16 frames at 30 fps times RunRate 0.7, loop 0.76 s; attack: 15 frames, 0.5 s, wind-up `WindUp`), with a random phase per robot; foot sliding and the cost at 100 to 400 robots are open ([[handover-2026-10-14-chaser-meshy]])
- behavior: moves straight toward the miner; no line-of-sight requirement.
- telegraph: wind-up `WindUp` in `EnemyStats.csv` row `Chaser` (the robot stops and flashes) before each hit (owner 2026-10-02, was 0.25 s)
- colors: bright company yellow; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-02): `EnemyStats.csv` row `Chaser`, columns `MaxHealth`, `MoveSpeed`, `Damage`, `AttackRate`, `AttackRange`, `XPValue`, `KnockbackResistance` (speed history: owner 2026-10-06 every robot moves at the same speed; earlier values 151, 182, 202, 288, 360, 450, each a slowdown asked by the owner)
- grey box (M1): yellow capsule with a red eye
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`; design sheet `chaser-turnaround.png` (crops chaser-front, chaser-side, chaser-back); Meshy input views `chaser-meshy-front.png`, `-left`, `-back`, `-right`; in game `chaser-ingame-run.png`, `chaser-ingame-attack.png`

## Chaser v2 trial (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288)
- A chaser v2 from the owner's Meshy mesh of `chaser-turnaround-v2.webp` (5120 triangles kept: the 3000 budget is lifted for v2), 123 cm, 10 parts, run and attack clips identical to the chaser v1 clips (v1 knee curve, no cap; provisional, owner 2026-10-23, the v1 animation looked better; was a run with the knee at most 15 degrees), its own debris pieces, and a shield chaser v2 carrying the same riot shield, whose shield run and shield bash clips are identical to the shield chaser v1 clips (provisional, owner 2026-10-23). Shield chaser v2 (provisional, owner 2026-10-23): forearm held forward and the shield carried about 30 cm further forward than now (estimate), so the feet have room. Both are new assets (BP_ChaserV2, BP_ShieldChaserV2) and the spawner points to them; every v1 asset, file and Blueprint stays intact so v1 can be restored by pointing the spawner back ([[robots-meshy-raw-2026-10-22]]).

## Shield variant (provisional, owner 2026-10-05)
- A chaser with a shield that absorbs damage before health (`EnemyStats.csv` row `ShieldChaser`, columns `Shield`, `XPValue`); translucent pale blue-white shell that breaks; the look changes to a physical riot shield carried on the left forearm, with a run clip holding it in front and a shield bash clip (owner 2026-10-17, provisional; not built, the game still shows the shell, [[art-prompts-shield]]); mixed into chaser packs (25 %) after 1:00. Inspired by the DRGS red swarmers.
- Same hp, speed and damage as the chaser (rows `Chaser` and `ShieldChaser` agree on `MaxHealth`, `MoveSpeed`, `Damage`); the shield never regenerates; a hit larger than the remaining shield carries the rest into health. From 1:00, chaser packs hold 2–4 shield chasers among 8–16 chasers (25 % on average); lone chasers, the heavy escort and hordes stay plain chasers (provisional, task-028-01).

## Relations
- [[enemies-index]] — parent index.
- [[enemy-death]] — death effect.
- [[chaser-meshy-pipeline]] — how the chaser mesh and its animation are built.
- [[art-prompts-shield]] — the riot shield of the shield variant: images, prompts, raw mesh.
- [[art-prompts-robots-meshy]] — v2 turnaround sheet (cleaner) and its Meshy prompts; mesh not made yet.
