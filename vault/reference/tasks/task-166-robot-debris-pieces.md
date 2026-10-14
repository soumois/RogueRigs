---
id: task-166-robot-debris-pieces
description: "Global task: at a chaser death, throw 2 or 3 random robot pieces (whole or cut limbs, torso) as pooled Niagara mesh particles."
parent: tasks-done-index
kind: task
status: done
---
# Robot death: Niagara debris pieces (chaser)

- request: Lorsqu'un robot meurt (le chasseur pour l'instant), lorsqu'il disparait, des particules niagara avec la forme des parties du robot (pas toujours les mêmes, entre 2 et 3 pièces, ex : jambe (découpée ou non au genou) / torse / bras (découpé ou non au coude)). Pas de collision avec le joueur et les autres robots. Léger. (owner, 2026-10-14)
- goal: When a chaser disappears at death, throw 2 or 3 random robot pieces (arm or leg whole or cut at the joint, torso) as pooled Niagara mesh particles.
- acceptance: When a chaser or shield chaser dies, after its 0.15 s shrink 2 or 3 piece meshes (whole or cut limbs, torso block) are thrown from the last hit direction, bounce on the floor only, shrink and sink within 1.2 s, through a pool of 40; every subtask done_when passed; docs updated; lint clean; committed. Visual result not seen by the crew: the owner tests it and measures its cost.
- gdd_check: enemy-death, enemy-chaser, chaser-meshy-pipeline, milestones, sys-enemies-base read. Conflicts: all 10 parts thrown (now 2 or 3 pieces), milestone M3 (brought forward), M3 hides the robot at once (owner keeps the 0.15 s shrink, hidden by a puff). Open values settled with the owner defaults (2026-10-14): puff about 12 sparks 0.3 s; count = meshes thrown, cut at elbow or knee one time in two; pelvis with the torso block; last-hit direction, floor plane only, 1.2 s; pools of 40, empty debris pool = puff only; shield chaser included. enemy-death updated (provisional, owner 2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: none
- result: Done. NS_RobotDebris (/Game/RogueRigs/FX): CPU world-space mesh emitter, burst = PieceMeshIndex length, 13 piece meshes by MeshIndex, velocity along HitDirection + up + spread, spin, gravity, analytic floor plane at system Z, shrink over last 0.3 s, 1.2 s, one-shot; offsets baked per MeshIndex in the system (User.PieceOffset declared but unread: no indexed Array Vector read through MCP); no sink (shrink only); BP_Chaser/BP_ShieldChaser DebrisSystem, pool 40, 13 DebrisPieces; PIE 11 kills, no Niagara errors; activation/release, look and cost unverified; commits 71c24af 75f75c5 51f27b5 705e03e

## Subtasks
1. task-166-01-robot-debris-pieces — 3d
2. task-166-02-debris-code — cpp
3. task-166-03-debris-meshes-import — bp
4. task-166-04-debris-niagara — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-166-01-robot-debris-pieces]] — step 1, 3d.
- [[task-166-02-debris-code]] — step 2, cpp.
- [[task-166-03-debris-meshes-import]] — step 3, bp.
- [[task-166-04-debris-niagara]] — step 4, bp.
