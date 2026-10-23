---
id: task-298-shooter-charge-fx
description: "Global task: A red charge-up effect grows at the shooter cannon tip during the 0.6 s targeting laser."
parent: tasks-to-do-index
kind: task
status: todo
---
# Shooter weapon charge-up effect

- request: "Il faudrait une particule de chargement d'arme avant le tir du tireur" (owner, 2026-10-23)
- goal: NS_ShooterCharge, a red glow and inward sparks that grow over 0.6 s at MuzzlePoint, started on OnAimStart and stopped on OnFire or OnAimEnd (provisional, owner 2026-10-23).
- acceptance: effect visible at the cannon tip during the laser in -game, gone after the shot or a cancelled aim; committed.
- gdd_check: enemy-shooter and color-code read (2026-10-23): charge-up line added, red as shots and laser (provisional, owner 2026-10-23). MuzzlePoint is at the aimed tip, so during the first 0.3 s raise the effect leads the cannon (accepted default).
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-298-01-bp-shooter-charge-ns
- result: open

## Subtasks
1. task-298-01-bp-shooter-charge-ns — bp: NS_ShooterCharge
2. task-298-02-bp-shooter-charge-wire — bp: BP_Shooter plays the charge-up
3. task-298-03-capture-shooter-charge — cpp: Capture the shooter charge-up

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-298-01-bp-shooter-charge-ns]] — step 01, bp.
- [[task-298-02-bp-shooter-charge-wire]] — step 02, bp.
- [[task-298-03-capture-shooter-charge]] — step 03, cpp.
