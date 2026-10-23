---
id: task-289-robot-run-v1-motion
description: "Global task: heavy and chaser v2 run clips rebaked with the chaser v1 run motion, light knee bend kept, RunRate reset against foot slide."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot run with the chaser v1 motion

- request: "Les courses du gros robot et du chasseur sont trop rigide, essaye de reprendre l'animation qu'avait le chasseur V1" (owner, 2026-10-23)
- goal: The heavy and chaser v2 run clips (clip 0) reproduce the chaser v1 run (meshy_bake_anim.py walk_style sine: thigh 35, arm 30, elbow 40, lean 8, twist 5, ground-fit bob, 16 frames at 30 fps), adapted to each robot's own pivots, knee flexion at most 15 degrees; RunRate in the body and eye instances set so the feet slide as little as possible at the robot's speed (provisional, owner 2026-10-23).
- acceptance: heavy slam rows 16..61 and chaser v2 attack rows 16..30 unchanged; v1 assets and the shield chaser v2 untouched; -game capture of both robots running compared with Saved/RobotMeshes/walk/ and a chaser v1 run; Blender side-by-side renders v1 vs new; committed.
- gdd_check: enemy-heavy and enemy-chaser read (2026-10-23): walk loop "with a light knee bend (at most 15 degrees)" kept; speeds heavy 153, chaser 180 uu/s unchanged; only the look of the run changes (rendering, not a rule). Shield chaser v2 (not named by the owner) has its own T_ShieldChaserV2Anim (copy of the old chaser v2 rows): left as is.
- confirmed_by_owner: yes (2026-10-23, direct order, run to the end)
- milestone: M2
- next_step: task-289-01-bake-heavy-run
- result: open

## Subtasks
1. task-289-01-bake-heavy-run — 3d: Rebake the heavy run with the v1 motion
2. task-289-02-bake-chaser-v2-run — 3d: Rebake the chaser v2 run with the v1 motion
3. task-289-03-bp-heavy-run — bp: Heavy anim texture and RunRate in Unreal
4. task-289-04-bp-chaser-v2-run — bp: Chaser v2 anim texture and RunRate in Unreal
5. task-289-05-capture-run — cpp: Capture both robots running

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-289-01-bake-heavy-run]] — step 01, 3d.
- [[task-289-02-bake-chaser-v2-run]] — step 02, 3d.
- [[task-289-03-bp-heavy-run]] — step 03, bp.
- [[task-289-04-bp-chaser-v2-run]] — step 04, bp.
- [[task-289-05-capture-run]] — step 05, cpp.
