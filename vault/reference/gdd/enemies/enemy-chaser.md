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
- animations (provisional, owner 2026-10-13): two clips: run (loop) and attack (0.1 s wind-up, then the strike); death is the Niagara parts effect, hits are a flash. Technique: baked animation texture (one transform per part and per frame, played by the material), no skeletal mesh at run time; look-alike pipeline in [[asset-pipeline]]
- animations_status (provisional, owner 2026-10-14): both clips are in the game, played by the material from the baked texture (run: 16 frames at 30 fps times RunRate 0.7, loop 0.76 s; attack: 15 frames, 0.5 s, wind-up 0.1 s), with a random phase per robot; foot sliding and the cost at 100 to 400 robots are open ([[handover-2026-10-14-chaser-meshy]])
- behavior: moves straight toward the miner; no line-of-sight requirement.
- telegraph: 0.1 s wind-up (the robot stops and flashes) before each hit (owner 2026-10-02, was 0.25 s)
- colors: bright company yellow; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-02): hp 30, speed 180 uu/s (owner 2026-10-06: every robot moves at the same speed, 180; before that 151, was 182; owner 2026-10-06 third test: 10 % slower again, was 202; owner 2026-10-06 second test: 30 % slower so the miner has time to dig, was 288; 20 % slower, owner 2026-10-06, was 360; 20 % slower, owner 2026-10-02, was 450), damage 10, attack_rate 1/s, range 120 uu, xp_value 1, knockback_resistance 0
- grey box (M1): yellow capsule with a red eye
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`; design sheet `chaser-turnaround.png` (crops chaser-front, chaser-side, chaser-back); Meshy input views `chaser-meshy-front.png`, `-left`, `-back`, `-right`; in game `chaser-ingame-run.png`, `chaser-ingame-attack.png`

## Shield variant (provisional, owner 2026-10-05)
- A chaser with a 30-point shield that absorbs damage before health; translucent pale blue-white shell that breaks; xp 2; the look changes to a physical riot shield carried on the left forearm, with a run clip holding it in front and a shield bash clip (owner 2026-10-17, provisional; not built, the game still shows the shell, [[art-prompts-shield]]); mixed into chaser packs (25 %) after 1:00. Inspired by the DRGS red swarmers.
- Same hp, speed and damage as the chaser; the shield never regenerates; a hit larger than the remaining shield carries the rest into health. From 1:00, chaser packs hold 2–4 shield chasers among 8–16 chasers (25 % on average); lone chasers, the heavy escort and hordes stay plain chasers (provisional, task-028-01).

## Relations
- [[enemies-index]] — parent index.
- [[enemy-death]] — death effect.
- [[chaser-meshy-pipeline]] — how the chaser mesh and its animation are built.
- [[art-prompts-shield]] — the riot shield of the shield variant: images, prompts, raw mesh.
