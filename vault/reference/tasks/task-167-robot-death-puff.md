---
id: task-167-robot-death-puff
description: "Global task: At a robot death, play a pooled Niagara puff of sparks that hides the 0.15 s shrink (chaser and shield chaser first)."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot death: puff hiding the shrink

- request: Lorsqu'un robot meurt, on lance une partie de mort, mais ça doit être léger. Tu peux continuer à rétrécir, mais il faut une particule pour cacher ça. (owner, 2026-10-14)
- goal: At a robot death, play a pooled Niagara puff of sparks that hides the 0.15 s shrink (chaser and shield chaser first).
- acceptance: When a chaser or shield chaser dies, a pooled Niagara puff of sparks (about 12, 0.3 s) plays at once and hides the 0.15 s shrink; pool of 40; every subtask done_when passed; docs updated; lint clean; committed. Visual result not seen by the crew.
- gdd_check: enemy-death, enemy-chaser, chaser-meshy-pipeline, milestones, sys-enemies-base read. Conflicts: all 10 parts thrown (now 2 or 3 pieces), milestone M3 (brought forward), M3 hides the robot at once (owner keeps the 0.15 s shrink, hidden by a puff). Open values settled with the owner defaults (2026-10-14): puff about 12 sparks 0.3 s; count = meshes thrown, cut at elbow or knee one time in two; pelvis with the torso block; last-hit direction, floor plane only, 1.2 s; pools of 40, empty debris pool = puff only; shield chaser included. enemy-death updated (provisional, owner 2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: task-167-02-death-puff-niagara
- result: open

## Subtasks
1. task-167-01-robot-death-puff — cpp
2. task-167-02-death-puff-niagara — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-167-01-robot-death-puff]] — step 1, cpp.
- [[task-167-02-death-puff-niagara]] — step 2, bp.
