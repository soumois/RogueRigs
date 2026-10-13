---
id: task-135-xp-pickups-pivot-float
description: "Global task: Stop the XP nut and chip from floating: put their pivot at their centre like the old cube."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP pickups: fix the floating pivot

- request: The new XP pickups float slightly; the old cube had its pivot at its centre, the nuts and chips at their bottom, and the mesh is the inherited root so it cannot be offset (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Stop the XP nut and chip from floating: put their pivot at their centre like the old cube.
- acceptance: Both meshes report centred bounds after reimport; Blueprints compile and save; doc node updated; or a report that the import cannot offset; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-135-01-xp-pickups-pivot-float
- result: open

## Subtasks
1. task-135-01-xp-pickups-pivot-float — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-135-01-xp-pickups-pivot-float]] — step 1, bp.
