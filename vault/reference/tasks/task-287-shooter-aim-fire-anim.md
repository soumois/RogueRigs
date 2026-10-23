---
id: task-287-shooter-aim-fire-anim
description: "Global task: Animate the shooter aim clip (cannon arm raised during the laser) and fire clip (recoil at the shot), after its walk."
parent: tasks-done-index
kind: task
status: done
---
# Shooter aim and fire animation

- request: "Tireur: clip de marche + un clip de visée et un clip de tir (owner answer relayed by the main session)" (owner, 2026-10-22)
- goal: Animate the shooter aim clip (cannon arm raised during the laser) and fire clip (recoil at the shot), after its walk.
- acceptance: every subtask done and its checks passed; a -game capture shows the shooter aiming then firing with its cannon arm; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Defaults accepted by the owner (provisional, owner 2026-10-22): aim and fire timing (default: arm raised over the first 0.3 s of the 0.6 s laser, recoil 0.2 s at the shot).
- depends_on_global: task-284
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: none
- result: Done. -game L_ArenaM1 side capture (robot_shots.ps1 + slomo 0.2): cannon arm raised horizontal along the laser (slow 4/8/10), laser from cannon tip, shot leaves the glowing tip with recoil tilt (slow 11-14), arm back horizontal after (slow 20); before: arm hanging, laser from hip (walk/shooter_0). Shots Saved/RobotMeshes/shooter-aim/. Unverified: 0.3 s raise not caught, rest pose after fire vs walk arm, side view only. No code change.

## Subtasks
1. task-287-01-bake-shooter-aim-fire — 3d: Bake the shooter aim and fire clips
2. task-287-02-cpp-shooter-aim-fire — cpp: ARRShooter plays aim and fire clips
3. task-287-03-bp-shooter-aim-fire — bp: BP_Shooter uses the aim and fire clips
4. task-287-04-capture-shooter-aim-fire — cpp: Capture the shooter aiming and firing

## Relations
- [[tasks-done-index]] — parent index.
- [[task-287-01-bake-shooter-aim-fire]] — step 01, 3d.
- [[task-287-02-cpp-shooter-aim-fire]] — step 02, cpp.
- [[task-287-03-bp-shooter-aim-fire]] — step 03, bp.
- [[task-287-04-capture-shooter-aim-fire]] — step 04, cpp.
