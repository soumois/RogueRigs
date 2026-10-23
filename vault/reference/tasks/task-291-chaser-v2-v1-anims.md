---
id: task-291-chaser-v2-v1-anims
description: "Global task: Chaser v2 run and attack, and shield chaser v2 shield run and bash, become exactly the v1 clips on the v2 parts."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser v2 and shield chaser v2 play the v1 clips

- request: "L'animation V1 du chasseur et du chasseur avec bouclier était mieux, retranscris ça sur le chasseur V2. Et laisse les autres animations comme ça pour l'instant." (owner, 2026-10-23)
- goal: Chaser v2 clips 0 and 1 = the chaser v1 run and attack (meshy_bake_anim.py v1 curves and keys, knee on the v1 curve up to about 55 degrees, no cap) on the v2 parts; shield chaser v2 clips 0 and 1 = the shield chaser v1 shield run and shield bash (task-220 bake, shield_chaser_bake.py, Art/Meshes/shield-chaser/) on the v2 parts, clips 2 and 3 = the new chaser v2 rows; RunRate 0.7 as v1 unless foot slide clearly favours another value; heavy (task-289 run, knee 15), shooter and kamikaze unchanged. Replaces task-290 (cancelled before any bake).
- acceptance: chaser v2 rows 0..30 and shield chaser v2 rows 0..30 follow the v1 curves (knee peaks reported); shield chaser v2 rows 31..61 equal the new chaser v2 rows 0..30; heavy, shooter, kamikaze and every v1 asset untouched; RunRate in sync on every body and eye instance of both robots; -game capture of chaser v2 and shield chaser v2 next to chaser v1 and shield chaser v1; committed.
- gdd_check: enemy-chaser and enemy-heavy read (2026-10-23): chaser v2 line now says run and attack identical to v1 (no knee cap) and shield clips identical to shield v1; the 35 and 45 degree lines of 26a7a5a2 reverted, heavy back to the 15 degree rule (owner 2026-10-23). No open value.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-291-04-bp-shield-v2-v1-clips
- result: open

## Subtasks
1. task-291-01-bake-chaser-v2-v1-clips — 3d: Bake the chaser v1 clips on chaser v2
2. task-291-02-bake-shield-v2-v1-clips — 3d: Bake the shield chaser v1 clips on shield chaser v2
3. task-291-03-bp-chaser-v2-v1-clips — bp: Chaser v2 v1-clips texture in Unreal
4. task-291-04-bp-shield-v2-v1-clips — bp: Shield chaser v2 v1-clips texture in Unreal
5. task-291-05-capture-v2-v1-clips — cpp: Capture chaser v2 and shield chaser v2

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-291-01-bake-chaser-v2-v1-clips]] — step 01, 3d.
- [[task-291-02-bake-shield-v2-v1-clips]] — step 02, 3d.
- [[task-291-03-bp-chaser-v2-v1-clips]] — step 03, bp.
- [[task-291-04-bp-shield-v2-v1-clips]] — step 04, bp.
- [[task-291-05-capture-v2-v1-clips]] — step 05, cpp.
