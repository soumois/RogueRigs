---
id: task-290-robot-run-knee-cap
description: "Global task: heavy and chaser v2 run knee follows the v1 curve, capped at 35 and 45 degrees instead of 15."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot run knee cap raised toward v1

- request: "Oui, relève le plafond du genou pour se rapprocher du v1" then "ok, lance avec 45° chasseur v2 et 35° gros" (owner, 2026-10-23)
- goal: Heavy and chaser v2 run clips rebaked with the knee following the v1 curve capped at 35 (heavy) and 45 (chaser v2) degrees instead of 15.
- acceptance: heavy slam rows 16..61 and chaser v2 attack rows 16..30 byte-identical; knee peaks reported (heavy at most 35, chaser v2 at most 45 degrees, v1 curve); no visible gap at the knee cut and no part interpenetration; foot slide per cycle reported against task-289 (heavy 245, chaser v2 79, v1 127 cm); RunRate changed only if it clearly improves slide, body and eye instances in sync; task-289 and older textures kept; shield chaser v2 and v1 assets untouched; GDD and pipeline 15 degree rule replaced; Blender side-by-side renders and -game capture compared with Saved/RobotMeshes/run-v1/; committed.
- gdd_check: enemy-heavy and enemy-chaser read (2026-10-23): the walk/run knee "at most 15 degrees" is replaced by the owner decision (heavy 35, chaser v2 45 degrees, provisional, owner 2026-10-23) in both specs; speeds unchanged; kamikaze leg sway and shield chaser v2 unchanged.
- confirmed_by_owner: yes (2026-10-23)
- milestone: M2
- next_step: task-290-01-robot-run-knee-cap
- result: open

## Subtasks
1. task-290-01-robot-run-knee-cap — 3d: Rebake the heavy run, knee cap 35
2. task-290-02-bake-chaser-v2-knee — 3d: Rebake the chaser v2 run, knee cap 45
3. task-290-03-bp-heavy-knee — bp: Heavy anim texture (and RunRate) in Unreal
4. task-290-04-bp-chaser-v2-knee — bp: Chaser v2 anim texture (and RunRate) in Unreal
5. task-290-05-capture-knee — cpp: Capture both robots running

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-290-01-robot-run-knee-cap]] — step 01, 3d.
- [[task-290-02-bake-chaser-v2-knee]] — step 02, 3d.
- [[task-290-03-bp-heavy-knee]] — step 03, bp.
- [[task-290-04-bp-chaser-v2-knee]] — step 04, bp.
- [[task-290-05-capture-knee]] — step 05, cpp.
