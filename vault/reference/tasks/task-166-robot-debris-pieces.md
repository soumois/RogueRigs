---
id: task-166-robot-debris-pieces
description: "Global task: at a chaser death, throw 2 or 3 random robot pieces (whole or cut limbs, torso) as pooled Niagara mesh particles."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot death: Niagara debris pieces (chaser)

- request: Lorsqu'un robot meurt (le chasseur pour l'instant), lorsqu'il disparait, des particules niagara avec la forme des parties du robot (pas toujours les mêmes, entre 2 et 3 pièces, ex : jambe (découpée ou non au genou) / torse / bras (découpé ou non au coude)). Pas de collision avec le joueur et les autres robots. Léger. (owner, 2026-10-14)
- goal: When a chaser disappears at death, throw 2 or 3 random robot pieces (arm or leg whole or cut at the joint, torso) as pooled Niagara mesh particles.
- acceptance: When a chaser or shield chaser dies, after its 0.15 s shrink 2 or 3 piece meshes (whole or cut limbs, torso block) are thrown from the last hit direction, bounce on the floor only, shrink and sink within 1.2 s, through a pool of 40; every subtask done_when passed; docs updated; lint clean; committed. Visual result not seen by the crew: the owner tests it and measures its cost.
- gdd_check: enemy-death, enemy-chaser, chaser-meshy-pipeline, milestones, sys-enemies-base read. Conflicts: all 10 parts thrown (now 2 or 3 pieces), milestone M3 (brought forward), M3 hides the robot at once (owner keeps the 0.15 s shrink, hidden by a puff). Open values settled with the owner defaults (2026-10-14): puff about 12 sparks 0.3 s; count = meshes thrown, cut at elbow or knee one time in two; pelvis with the torso block; last-hit direction, floor plane only, 1.2 s; pools of 40, empty debris pool = puff only; shield chaser included. enemy-death updated (provisional, owner 2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: task-166-05-debris-not-visible
- result: open (owner 2026-10-14: debris not visible in game, puff OK)

## Subtasks
1. task-166-01-robot-debris-pieces — 3d
2. task-166-02-debris-code — cpp
3. task-166-03-debris-meshes-import — bp
4. task-166-04-debris-niagara — bp
5. task-166-05-debris-not-visible — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-166-01-robot-debris-pieces]] — step 1, 3d.
- [[task-166-02-debris-code]] — step 2, cpp.
- [[task-166-03-debris-meshes-import]] — step 3, bp.
- [[task-166-04-debris-niagara]] — step 4, bp.
- [[task-166-05-debris-not-visible]] — step 5, cpp: owner sees no debris in game.
