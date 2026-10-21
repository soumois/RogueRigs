---
id: task-279-drone-eye-glow
description: "Global task: The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Drone eye glow, orange

- request: J'aimerais faire aussi que l'oeil de mon robot (celui qui me suit) glow un peu plus (tu peux mettre de l'orange, c'est juste visuel) (owner, 2026-10-21)
- goal: The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone.
- acceptance: Mask texel report; parameters read back; M_Drone compiles and is saved; docs and colour decision recorded; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser 25 -> 50, drone eye orange (1, 0.45, 0.08) x 6 with the colour deviation accepted, thruster table with white-teal kept, no captures, XP back to pre-269)
- milestone: M1
- next_step: task-279-01-drone-eye-glow
- result: open

## Subtasks
1. task-279-01-drone-eye-glow — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-279-01-drone-eye-glow]] — step 1, bp.
