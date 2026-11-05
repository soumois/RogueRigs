---
id: task-433-robot-hit-flash
description: "Global task: A robot that takes a hit turns plain white for a brief moment, at low cost, every class. Awaiting owner answers."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot hit flash

- request: Owner 2026-11-05, answering 'Je passe le lot 1 au planner ?': "Oui, lot 1". Proposal he accepted: "**1. Retours manquants** : Étincelles d'impact, flash blanc de 0,05 à 0,1 s sur le robot touché, burst et anneau du baril réutilisés pour le kamikaze." Formalised by the main session: "Lot 1 d'amélioration des FX, trois retours visuels manquants (une tâche globale par point). Contraintes : color-code et readability-rules (pas de rouge sur les FX joueur ni dans les explosions, orange = explose), coût à horde (perf-lead-debris-gpu). Le rendu est validé en jeu par le propriétaire." Point 2: "Flash blanc bref (0,05 à 0,1 s) sur le robot touché, à bas coût : piste Custom Primitive Data, déjà utilisées pour l'animation des robots (RREnemy.cpp:814).". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: Every hit on a robot is visible on the robot itself as a brief white flash, with no per-robot material cost.
- acceptance: A1 in game, every robot class that spawns (chaser and shield chaser, kamikaze, shooter, heavy) turns plain white for 0.08 s when it takes damage from any source, a new hit restarts it (provisional, inside the owner's 0.05 to 0.1 s); A2 the kamikaze core keeps its orange pulse during the flash and its fuse telegraph is unchanged; A3 a hit absorbed by a shield flashes the robot, the shield itself is unchanged; A4 the chaser wind-up telegraph still reads as before and baked animation still plays during and after a flash; A5 low cost: no dynamic material instance per robot, no new per-robot tick to run the flash when not flashing, the Custom Primitive Data route unless the Executor reports why not; A6 hit rules, damage and death effect unchanged, existing tests pass, only expectations this change breaks are updated; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read readability-rules, color-code, enemy-chaser ("hits are a flash", which this implements), enemy-kamikaze, enemy-death, enemies-index, milestones. In scope: M3 juice. No conflict: white is no robot's main colour and a 0.08 s flash leaves colour coding intact. Not stated: which classes, sources and shield case (Q1, Q2), exact duration and repeated hits (Q3). Whether each robot class reads Custom Primitive Data is a code fact the Executor checks, hence the representative slice before the other classes. Cost measure: Q4 of task-432 covers the lot.
- confirmed_by_owner: yes (2026-11-05, "Ok, tout par défaut")
- milestone: M2
- next_step: task-433-03-flash-other-robots
- started: 2026-11-05T16:05:27Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-05.
- Q1 Quels robots et quelles sources ? Default: toutes les classes, et tout dégât reçu (tirs, tourelles, drone, explosions). Le noyau orange du kamikaze ne flashe pas, il garde son pulse, car orange veut dire « explose ».
- Q2 Chaser à bouclier : quand le bouclier encaisse le coup ? Default: le robot flashe comme pour un coup normal et le bouclier ne change pas.
- Q3 Durée et coups répétés ? Default: blanc franc pendant 0,08 s, un nouveau coup relance le flash ; sous un tir très rapide un robot peut donc rester blanc un instant, tu juges en jeu si ça gêne.

## Subtasks
1. task-433-01-flash-trigger (vault/reference/tasks/task-433-robot-hit-flash/task-433-01-flash-trigger.md) — cpp: A robot taking damage from any source raises and clears its flash value at low cost.
2. task-433-02-flash-chaser-material (vault/reference/tasks/task-433-robot-hit-flash/task-433-02-flash-chaser-material.md) — bp: Representative slice: the chaser reads the flash value and turns plain white, baked animation intact.
3. task-433-03-flash-other-robots (vault/reference/tasks/task-433-robot-hit-flash/task-433-03-flash-other-robots.md) — bp: Every other robot class flashes the same way, the kamikaze core excepted.
4. task-433-04-doc (vault/reference/tasks/task-433-robot-hit-flash/task-433-04-doc.md) — doc: Document the flash value, its slot and the materials that read it.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-433-01-flash-trigger]] — step 1, cpp.
- [[task-433-02-flash-chaser-material]] — step 2, bp.
- [[task-433-03-flash-other-robots]] — step 3, bp.
- [[task-433-04-doc]] — step 4, doc.
