---
id: task-413-bullet-shown-at-muzzle
description: "Global task: the rifle bullet visually starts ahead of the Muzzle; measure the cause in game, then draw its first frame at the Muzzle."
parent: tasks-to-do-index
kind: task
status: todo
---
# Bullet shown at the muzzle on its first frame

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "@C:\Users\soumo\Videos\2026-10-31 14-51-08.mkv — en 00:03 et 00:04, tu peux voir que la balle démarre visuellement plus loin que le muzzle (j'ai vérifié il est au bon endroit). Aussi, il n'y a pas de particule pour le tir." This task covers the first point (bullet starts visually further than the Muzzle); the second point (no shot particle) is task-414-muzzle-flash. Context relayed: tasks 412-01..03 just made shots leave from the hand weapon Muzzle (capsule collision, miner-to-Muzzle sweep). Owner answer 2026-10-31 (relayed by the main session, verbatim): "ok, tout par défaut" = the plan and the defaults of Q1, Q2 accepted.
- goal: In game the burst rifle bullet is first drawn at the barrel tip (the Muzzle the owner placed), not one or more bullet lengths ahead of it, with its speed, range, collision, sweep and hits unchanged. Choices per the Questions.
- acceptance: C++ builds; a -game run measures, for rifle bullets, the Muzzle world location at fire time, the bullet location after the Q3 sweep and its ShotMesh location on the first rendered frame, before and after the fix (numbers written in the cpp result); after the fix the first rendered ShotMesh location is within 10 uu of the Muzzle (horizontal distance) for every bullet of a burst; speed 3000 uu/s, range, capsule sizing, miner-to-Muzzle sweep, rock stop and robot hits unchanged; shotgun pellets get the same first-frame behaviour through the shared path; turret bullets and robot shots unchanged; only existing test expectations the change breaks updated, no new test; no .uasset touched (BP_BurstRifle and ABP_Miner stay the owner uncommitted edits); doc nodes updated; gdd burst-rifle line updated (edit on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (approved; shots leave from the rifle Muzzle, flat, parallel to the aim, projectile speed 3000 uu/s provisional; Muzzle placed by the owner, task-412 Q6), [[weapons-index]] (shown-weapon principle). No conflict: the request restores what task-412 meant (bullets visibly leave the barrel tip). Open values needed: none. Milestone M2. Video read 2026-10-31 (00:03 to 00:04, frames at 10 fps cropped on the miner): every rifle bullet (short white-gold streak) first appears about one to two bullet lengths ahead of the rifle tip, then flies flat toward the cursor; no flash at the barrel. Tech read: [[sys-projectiles]] (spawned on the miner axis at Muzzle height, `SweepTo(Muzzle)`, then UProjectileMovementComponent at 3000 uu/s, no sub-stepping), [[sys-weapons]], [[bp-projectile]] (ShotMesh sphere at relative (0, 0, 0) of the capsule), [[bp-burst-rifle]] (Muzzle (52.25, 0, 0), runtime counter-move of task-411); code read: RRWeapon.cpp `SpawnShot`. Likely cause (not verified, must be measured in a real run first per the owner lessons): the bullet moves one frame before it is first drawn (3000 uu/s = 50 uu per frame at 60 fps, 100 uu at 30 fps in PIE), plus motion blur stretching the streak; other candidates the measure must rule out: Muzzle read one frame late against the hand animation, ShotMesh offset. Owner working tree: BP_BurstRifle.uasset and ABP_Miner.uasset modified, not committed; no step stages them.
- confirmed_by_owner: yes (2026-10-31, "ok, tout par défaut")
- milestone: M2
- next_step: task-413-01-bullet-first-frame-at-muzzle
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-31 (owner "ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-31.
- Q1 Fix: Default: the cpp subtask first measures in a -game run (Muzzle at fire time, bullet after the sweep, bullet on its first drawn frame) to find the real cause, then fixes it so the bullet is drawn at the Muzzle on its first frame (likely: the bullet already travels one frame, 50 uu at 60 fps, before it is first drawn; the fix holds its first movement step so it appears at the Muzzle and then flies), speed, range and collision unchanged; if the measure shows another cause, that cause is fixed and named in the result. Or: measure and report only, no fix before you decide.
- Q2 Visual check: Default: no capture subtask (as in task-412 Q5); the cpp result gives the measured distances before and after, you check in PIE. Or: a capture subtask (C1: on frames, the bullet first appears at the barrel tip).

## Subtasks
1. task-413-01-bullet-first-frame-at-muzzle (vault/reference/tasks/task-413-bullet-shown-at-muzzle/task-413-01-bullet-first-frame-at-muzzle.md) — cpp: measure in a -game run where the rifle bullet is first drawn versus the Muzzle, find the cause, fix it so the first drawn frame is at the Muzzle
2. task-413-02-doc-bullet-first-frame (vault/reference/tasks/task-413-bullet-shown-at-muzzle/task-413-02-doc-bullet-first-frame.md) — doc: sys-projectiles, sys-weapons (first drawn frame at the Muzzle, cause and fix, debug log or cvar)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-413-01-bullet-first-frame-at-muzzle]] — step 1, cpp.
- [[task-413-02-doc-bullet-first-frame]] — step 2, doc.
- [[task-412-shots-from-rifle-muzzle]] — task that made shots leave from the Muzzle; this one fixes where the bullet is first drawn.
- [[task-414-muzzle-flash]] — second point of the same owner message (shot particle).
- [[burst-rifle]] — gdd shot origin line this task extends.
- [[sys-projectiles]] — projectile spawn, sweep and movement this task changes.
