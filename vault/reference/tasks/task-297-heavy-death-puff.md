---
id: task-297-heavy-death-puff
description: "Global task: The heavy gets its own larger death puff that hides its death shrink."
parent: tasks-done-index
kind: task
status: done
---
# Bigger death puff for the heavy

- request: "On voit le gros réduire son scale lorsqu'il meurt. Il faudrait une plus grosse particule pour lui (pour cacher ça)" (owner, 2026-10-23)
- goal: A heavy-only puff (NS_HeavyDeathPuff) sized for the 248 cm robot covers the 0.15 s death shrink; the other robots keep NS_RobotDeathPuff; shrink duration unchanged (provisional, owner 2026-10-23).
- acceptance: BP_Heavy DeathPuffSystem = NS_HeavyDeathPuff; other Blueprints unchanged; -game frames during a heavy death show the shrink hidden; committed.
- gdd_check: enemy-heavy read (2026-10-23): death puff line added (provisional, owner 2026-10-23); enemy-death rule (shrink 0.15 s, puff) unchanged for the others. Owner default accepted 2026-10-23: same warm orange as the robots' death puff, puff big enough to hide the heavy's death shrink (provisional, owner 2026-10-23).
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Added rr.Debug.KillRobots (b5b37c44, sys-cheats 8ea89703). Run 1: heavy head drawn over the puff at 0.04 s -> 297-01 rework f729a121 (puff +130 cm, sprites 70-190 cm). Run 2 (Saved/RobotMeshes/heavy-death/heavy_sheet.png): head, torso and arms hidden from the first puff frame (0.04 s); feet and shins still visible below the puff 0.04-0.09 s, hidden from 0.14 s; colour reads cream-yellow with orange edges. Residual feet left for owner review.

## Subtasks
1. task-297-01-bp-heavy-death-puff-ns — bp: NS_HeavyDeathPuff
2. task-297-02-bp-heavy-death-puff-set — bp: BP_Heavy uses the heavy puff
3. task-297-03-capture-heavy-death — cpp: Capture a heavy death

## Relations
- [[tasks-done-index]] — parent index.
- [[task-297-01-bp-heavy-death-puff-ns]] — step 01, bp.
- [[task-297-02-bp-heavy-death-puff-set]] — step 02, bp.
- [[task-297-03-capture-heavy-death]] — step 03, cpp.
