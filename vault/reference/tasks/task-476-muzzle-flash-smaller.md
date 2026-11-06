---
id: task-476-muzzle-flash-smaller
description: "Global task: The green burst rifle muzzle flash keeps its look but is shown smaller; turret and shotgun flashes unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Player muzzle flash a little smaller

- request: Owner 2026-11-06 (verbatim): "* Le muzzle du joueur tir est très bien, mais il est un peu trop grand. * La partie du bas de l'écran est mieux, mais on s'est éloigné de la proposition de l'art. Repart de la proposition et je laisse 6 tours de correction ( c'était 5 avant ) pour arriver au même résultat. ( image 1 ) * Image 2 on voit des lignes noir sur la surface des murs, il faudrait retiré ça et avoir plus quelque chose comme l'image 3. Il a aussi l'emissif doit aussi être sur l'edge sur la partie du dessus. ( 4 tours de correction max )". Image 1 ("target", 1229 x 190): a crop of the bottom of the retained HUD mockup Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp (= Art/Source/hud-unified/20261105-152627-hud-unified-1.png), same picture; labelled temporary (duplicate crop), kept only in Saved/Scratch/hud-477/owner-target-crop-2026-11-06-temporary.png. Image 2 kept (owner image): Art/Source/cartoon-look/reference/owner-2026-11-06-wall-black-lines.png (low view of the walls: purple wall faces with dark wavy horizontal lines and visible vertical facet cuts; orange glow along the top lip; an orange editor selection outline at the wall foot). Image 3 kept (owner image): Art/Source/cartoon-look/reference/owner-2026-11-06-wall-section-target.png (diagram "Coupe de principe": top surface with a continuous texture, glowing emissive edge "bord lumineux", wall "maille unique (sans découpe)" with soft vertical dark streaks). Split into three global tasks, one per owner point: task-476 muzzle flash, task-477 bottom HUD, task-478 walls. This task: "Le muzzle du joueur tir est très bien, mais il est un peu trop grand.". Owner answer 2026-11-06 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1 accepted.
- goal: The green muzzle flash of the player rifle keeps the look the owner likes and is a little smaller on screen.
- acceptance: A1 in a -game run the burst rifle green muzzle flash is about 30 % smaller on screen than now (Q1), core, spikes, forward streak and shards scaled together; A2 its look is unchanged: same tones, shape, timing and fade into shards (style of Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp), still attached to the rifle Muzzle, one per shot; A3 per Q1 the turret flash keeps its current on-screen size and the shotgun white-gold flash is unchanged; A4 no gameplay value changes; A5 compiles, no new runtime error in PIE; A6 a -game capture compares the rifle flash before and after (same zoom) and is sent to the owner; the report gives the token cost per subtask and in total.
- gdd_check: Read burst-rifle (vfx_sfx: NS_MuzzleFlash, core about 25 uu, turret flash same effect at scale 0.6 since task-464), color-code (player laser row), task-464-player-shot-colour (the flash being shrunk). No conflict: look only, rules unchanged. Not a GDD value: the amount (Q1). Edit to burst-rifle applies on confirmation. No code read.
- confirmed_by_owner: yes (2026-11-06, "Ok, tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-06T19:26:29Z
- waiting_for: none
- executor_tokens: input 146, cache write 174363, cache read 8040835, output 30273
- executor_usage: runs 1, turns 69, context start 17k peak 186k, resumes 3, 8 min, units 1.17 M
- planning_tokens: input 11, cache write 29692, cache read 298315, output 7823
- planning_usage: af36ec5b "Plan owner request: muzzle, HUD, walls" 1/3 (tasks 476, 477, 478), units 0.11 M
- result: Done, size provisional until the owner sees it. 476-01 (af6e7912): rifle NS_MuzzleFlash every sprite size and the spark speed x0.7 (core 35-42 uu, shape 119-140, streak 11.2 x 210-252), look, colours and timing unchanged; turret keeps its size through NS_MuzzleFlashTurret, a copy at the task-464 sizes (sprite size ignores component scale); shotgun untouched. 476-02: before/after -game slomo frames, sheets Saved/Captures/task476_compare_crop.png and task476_compare_turret2.png: rifle star smaller, same look; turret and shotgun flashes unchanged. 476-03 (beaa3d6f): bp-muzzle-flash, bp-turret, sys-weapons. No gameplay value or C++ changed. Not rerun: RogueRigs.Weapons.MuzzleFlashAsset (asset-only change).

## Questions
Decided 2026-11-06 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 Taille : Default: seul le flash vert du fusil du mineur rétrécit, d'environ 30 % (forme, couleurs et durée inchangées) ; le flash des tourelles garde sa taille actuelle à l'écran et le flash blanc-or du fusil à pompe ne change pas ; une capture avant/après t'est envoyée et tu ajustes ensuite si besoin. Autres : un autre pourcentage, ou les tourelles aussi.

## Subtasks
1. task-476-01-rifle-flash-size (vault/reference/tasks/task-476-muzzle-flash-smaller/task-476-01-rifle-flash-size.md) — bp: The burst rifle green muzzle flash is shown about 30 % smaller, look unchanged; turret and shotgun flashes keep their size.
2. task-476-02-capture (vault/reference/tasks/task-476-muzzle-flash-smaller/task-476-02-capture.md) — capture: Before/after capture of the rifle flash, with a turret firing.
3. task-476-03-doc (vault/reference/tasks/task-476-muzzle-flash-smaller/task-476-03-doc.md) — doc: Docs record the new flash size.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-476-01-rifle-flash-size]] — step 1, bp.
- [[task-476-02-capture]] — step 2, capture.
- [[task-476-03-doc]] — step 3, doc.
