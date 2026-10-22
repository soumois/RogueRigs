---
id: task-285-heavy-slam-anim
description: "Global task: Animate the heavy slam: fists raised during the telegraph, then knees bent and body leaning forward so the fists touch the ground."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heavy slam animation

- request: "Pour le gros, pour taper au sol, comme comptes tu fais ( Je suppose qu'il faudrait qu'il plie ses genoux et se penche un peu vers l'avant pour que ses points touches de le sol. Pour la préparation, tu peux juste lever ses points vers le haut )." (owner, 2026-10-22)
- goal: Animate the heavy slam: fists raised during the telegraph, then knees bent and body leaning forward so the fists touch the ground.
- acceptance: every subtask done and its checks passed; a -game capture shows the heavy fists raised in the telegraph and on the ground at the slam; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Defaults accepted by the owner (provisional, owner 2026-10-22): slam timing (default: telegraph 0.6 s unchanged, fists raised over 0.45 s, strike over the last 0.15 s, contact on the slam damage frame, recover 0.5 s); BodyMesh tilt 14 degrees replaced by the clip (default yes, eye glow kept).
- depends_on_global: task-284
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: task-285-04-capture-slam
- result: open

## Subtasks
1. task-285-01-bake-heavy-slam — 3d: Bake the heavy telegraph and slam clips
2. task-285-02-cpp-heavy-slam-clips — cpp: ARRHeavy plays the slam clips
3. task-285-03-bp-heavy-slam — bp: BP_Heavy uses the slam clips
4. task-285-04-capture-slam — cpp: Capture the heavy slam

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-285-01-bake-heavy-slam]] — step 01, 3d.
- [[task-285-02-cpp-heavy-slam-clips]] — step 02, cpp.
- [[task-285-03-bp-heavy-slam]] — step 03, bp.
- [[task-285-04-capture-slam]] — step 04, cpp.
