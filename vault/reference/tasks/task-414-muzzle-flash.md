---
id: task-414-muzzle-flash
description: "Global task: a short white-gold Niagara muzzle flash at the shown rifle Muzzle on every miner shot; today shots have no particle."
parent: tasks-done-index
kind: task
status: done
---
# Muzzle flash on every miner shot

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "@C:\Users\soumo\Videos\2026-10-31 14-51-08.mkv — en 00:03 et 00:04, tu peux voir que la balle démarre visuellement plus loin que le muzzle (j'ai vérifié il est au bon endroit). Aussi, il n'y a pas de particule pour le tir." This task covers the second point ("il n'y a pas de particule pour le tir"); the first point is task-413-bullet-shown-at-muzzle. Reading: a muzzle flash at the barrel when firing; impact sparks were not asked and are not planned. Owner answer 2026-10-31 (relayed by the main session, verbatim): "ok, tout par défaut" = the plan and the defaults of Q3, Q4, Q5, Q6 accepted.
- goal: Every shot of the miner shows a short bright white-gold flash at the barrel tip of the rifle in his hand, following the hand; choices per the Questions.
- acceptance: NS_MuzzleFlash exists in Content/RogueRigs/FX/ (white-gold per [[color-code]], look per Q4, one-shot, auto-destroys) and compiles; C++ builds; at each rifle bullet and each shotgun volley fired by the miner, one flash spawns attached to the shown hand weapon Muzzle (Q3), loaded by C++ default path so no Blueprint is edited (Q5), skipped where Niagara does not render (-nullrhi); turret bullets and robot shots spawn none; existing tests pass, no new test; BP_BurstRifle and ABP_Miner untouched; doc nodes created or updated; gdd burst-rifle vfx_sfx line updated (edit on confirmation); lint clean; committed by path. Visual check per Q6.
- gdd_check: Read 2026-10-31: [[art-style-guide]] A8 ("Player fire: white-gold bolts, bright muzzle flash, small impact sparks on the hit target"), [[color-code]] (player shots white-gold), [[burst-rifle]] (vfx_sfx: "muzzle, impact and sound open": the flash look is an open value, proposed in Q4 and written on confirmation), [[shotgun]] (vfx_sfx: "white-gold pellets and muzzle flash"), [[weapons-index]] (every other player weapon shoots from the shown rifle Muzzle, task-412). No conflict; open value: burst-rifle muzzle look (Q4). Impact sparks (A8) not requested: not planned. Milestone M2. Video 2026-10-31: no flash or particle at the barrel on any shot. Tech read: Content/RogueRigs/FX/ holds no muzzle flash; the Niagara module is already a dependency; existing pattern: RRDrone.cpp loads NS_DroneThruster and NS_DigImpact by `ConstructorHelpers::FObjectFinder` path, skips Niagara without rendering; [[bp-shooter-charge]] is a similar effect at a cannon tip. Owner working tree: BP_BurstRifle.uasset and ABP_Miner.uasset modified, not committed (Q5 keeps them untouched).
- confirmed_by_owner: yes (2026-10-31, "ok, tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-10-31T22:17:02Z
- waiting_for: none
- executor_tokens: input 32, cache write 37388, cache read 457579, output 6961
- executor_usage: runs 1, turns 13, context start 18k peak 49k, resumes 0, 12 min, units 0.13 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. bp-muzzle-flash created; sys-weapons, sys-weapons-shotgun, bp-burst-rifle updated; lint clean (8a21d3d2)

## Questions
Decided 2026-10-31 (owner "ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-31.
- Q3 Which shots: Default: one flash per shot of every miner weapon, at the shown rifle Muzzle and attached to it so it follows the hand (3 per rifle burst, 1 per shotgun volley); turret bullets and robot shots none. Or: rifle bullets only.
- Q4 Look (open in the gdd): Default: per art rule A8 and the colour code, white-gold, very short (about 0.06 s), a bright core about 25 uu wide plus a few short sparks thrown forward along the aim, no light. Or: tell another look (size, duration, a light flash, smoke).
- Q5 Wiring: Default: NS_MuzzleFlash in Content/RogueRigs/FX/, loaded by the C++ weapon by default path (as the drone thruster), so BP_BurstRifle, with your uncommitted edit, is not opened. Or: a property set on BP_BurstRifle (the bp agent opens and saves your uncommitted asset).
- Q6 Visual check: Default: no capture subtask, you check in PIE (a 0.06 s flash is hard to catch on captured frames). Or: a capture subtask (C1: a flash at the barrel tip on a shot frame).

## Subtasks
1. task-414-01-ns-muzzle-flash (vault/reference/tasks/task-414-muzzle-flash/task-414-01-ns-muzzle-flash.md) — bp: create NS_MuzzleFlash (one-shot white-gold core and forward sparks, and its sprite material if needed) in Content/RogueRigs/FX/
2. task-414-02-spawn-muzzle-flash (vault/reference/tasks/task-414-muzzle-flash/task-414-02-spawn-muzzle-flash.md) — cpp: ARRWeapon spawns NS_MuzzleFlash attached to the shown hand weapon Muzzle on every miner shot (rifle bullet, shotgun volley)
3. task-414-03-doc-muzzle-flash (vault/reference/tasks/task-414-muzzle-flash/task-414-03-doc-muzzle-flash.md) — doc: create bp-muzzle-flash (fx index); update sys-weapons, sys-weapons-shotgun, bp-burst-rifle

## Relations
- [[tasks-done-index]] — parent index.
- [[task-414-01-ns-muzzle-flash]] — step 1, bp.
- [[task-414-02-spawn-muzzle-flash]] — step 2, cpp.
- [[task-414-03-doc-muzzle-flash]] — step 3, doc.
- [[task-413-bullet-shown-at-muzzle]] — first point of the same owner message (bullet start).
- [[burst-rifle]] — gdd vfx_sfx line this task fills.
- [[art-style-guide]] — rule A8 for player fire effects.
- [[sys-weapons]] — fire path that spawns the flash.
