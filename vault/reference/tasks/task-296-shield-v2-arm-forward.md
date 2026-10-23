---
id: task-296-shield-v2-arm-forward
description: "Global task: Shield chaser v2 holds its forearm forward and carries the shield about 30 cm further forward to free the feet."
parent: tasks-done-index
kind: task
status: done
---
# Shield chaser v2 holds the shield forward

- request: "Concernant le shield, il faudrait l'avancer au moins de 30 cm (estimation) pour laisser un peu plus de place au pied. Il faut que le robot ait le bras tenu vers l'avant" (owner, 2026-10-23)
- goal: On the task-291 shield clips (0 shield run, 1 shield bash), the left forearm is held forward and the shield sits at least 30 cm further forward than now (estimate, provisional, owner 2026-10-23), clear of the feet over the run; plain clips 2 and 3 unchanged.
- acceptance: shield forward offset and minimum foot distance reported; plain rows unchanged; new mesh and texture on BP_ShieldChaserV2; -game capture; committed.
- gdd_check: enemy-chaser read (2026-10-23): shield chaser v2 forearm forward, shield about 30 cm further forward (provisional, owner 2026-10-23). Depends on task-291 (its shield clips are rebaked there); runs after it.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Shield chaser v2 forearm about horizontal, hand on the shield, shield +33 cm forward on run (+27 to +32 bash), min shield-leg 43.6 cm run / 34.8 bash (was 2.64 / 6.34); new SM_ShieldChaserV2_Meshy_AnimFwd and T_ShieldChaserV2AnimFwd on BP_ShieldChaserV2, AnimV1Clips kept; plain rows unchanged; renders Saved/Renders/task-296/, -game captures Saved/RobotMeshes/shield-forward/ (side run shows shield ahead of the feet, hand joined); unverified: bash pose in game, bake error 0.068 cm above 0.05 warning.

## Subtasks
1. task-296-01-bake-shield-arm-forward — 3d: Bake the shield held forward
2. task-296-02-bp-shield-arm-forward — bp: Shield held forward in Unreal
3. task-296-03-capture-shield-arm-forward — cpp: Capture the shield held forward

## Relations
- [[tasks-done-index]] — parent index.
- [[task-296-01-bake-shield-arm-forward]] — step 01, 3d.
- [[task-296-02-bp-shield-arm-forward]] — step 02, bp.
- [[task-296-03-capture-shield-arm-forward]] — step 03, cpp.
