---
id: task-156-chaser-meshy-integration
description: "Put the corrected Meshy chaser in the game as a static mesh, replacing the current chaser model."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser Meshy: game-ready mesh

- request: The owner validated the corrected Meshy chaser and asks for its integration in the game, then the run and attack animations (owner, 2026-10-14); step 1 prepares the mesh for Unreal: height, pivot, orientation, units (owner, 2026-10-14)
- goal: Put the corrected Meshy chaser in the game as a static mesh in place of the current chaser model.
- acceptance: BP_Chaser and BP_ShieldChaser show the corrected Meshy chaser (123 cm tall, feet at the capsule bottom, front on +X, painted texture, glowing red eye) instead of the current model; the shield shell still encloses it; doc nodes updated; no C++ change; the owner is told how to see it in game.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-156-01-chaser-meshy-integration
- result: open

## Subtasks
1. task-156-01-chaser-meshy-integration — 3d
2. task-156-02-chaser-meshy-unreal-import — bp (import, material, Blueprint swap)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-156-01-chaser-meshy-integration]] — step 1, 3d.
- [[task-156-02-chaser-meshy-unreal-import]] — step 2, bp.
