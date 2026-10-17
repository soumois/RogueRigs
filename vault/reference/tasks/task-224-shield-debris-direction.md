---
id: task-224-shield-debris-direction
description: "Throw the shield fragments along the breaking hit like the robot pieces, turned 20 degrees toward the shield side unless the hit came from behind."
parent: tasks-done-index
kind: task
status: done
---
# Shield debris: hit-driven throw with a shield-side bias

- request: "Pour la projection du bouclier, reprends comme pour les pièces pour la projection et l'impact, sauf que décale-le de 20 degrés du côté du bouclier (si on tient le bouclier sur la gauche, le bouclier aura d'office tendance à partir vers la gauche même si le tir vient de tout droit). Si ce qui a détruit le robot vient de derrière lui, alors pas de modification de l'angle." (owner 2026-10-17)
- goal: NS_ShieldDebris throws along a direction given by the hit that broke the shield, exactly like NS_RobotDebris uses User.HitDirection; that direction is turned 20 degrees toward the side the shield is held on, except when the hit came from behind the robot.
- acceptance: C++ direction function, Niagara and Blueprint wired; a `-game` capture of a break whose fragments lean to the shield side, sent to the owner.
- gdd_check: [[enemy-death]] (robot pieces thrown along the last hit), [[bp-robot-debris]] (velocity from User.HitDirection), [[bp-shield-debris]] (current: owner forward, no user parameter). Definitions (Orchestrator): hit direction = horizontal travel direction of the breaking hit (RecordHitDirection); "from behind" = that direction points along the robot forward (dot > 0); shield side = the side of the shield fragments' offsets (negative Unreal Y in shield_debris_pieces.json = robot left); fallback without a recorded hit = the robot backward direction, then the same rule.
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: Done. Commit c7611a63: vault/assets/shield-chaser-debris-direction.png; one -game run; robot facing the miner, rr.Debug.BreakShields (miner hit from the front): fragments travel back and toward the robot left (shield side), about +163 px back and +58 px left from the break point, about 20 degrees; no Blueprint or Niagara error. Fragments look like long motion-blur streaks (game at 4-6 fps during shots); BreakShields counts 6 living shield chasers for 1 spawned.

## Subtasks
- [[task-224-01-shield-debris-direction-cpp]] — cpp: direction function, debug break records a miner hit.
- [[task-224-02-ns-shield-debris-hitdir]] — bp: NS_ShieldDebris velocity from User.HitDirection.
- [[task-224-03-bp-shield-chaser-hitdir]] — bp: BP_ShieldChaser passes the direction on break.
- [[task-224-04-shield-debris-direction-capture]] — cpp: one `-game` run, capture.

## Relations
- [[tasks-done-index]] — parent index.
- [[bp-shield-debris]] — the system changed.
- [[task-222-shield-break-debris]] — the debris it refines.
