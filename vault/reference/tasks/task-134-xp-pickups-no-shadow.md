---
id: task-134-xp-pickups-no-shadow
description: "Global task: Turn off the mesh shadow of the XP nut and chip Blueprints."
parent: tasks-done-index
kind: task
status: done
---
# XP pickups: no mesh shadow

- request: Turn off the mesh shadow of both new XP pickups (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Turn off the mesh shadow of the XP nut and chip Blueprints.
- acceptance: CastShadow reads false on both Blueprints after compile and save; both .uasset modified in git; doc node updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. CastShadow false on the PickupMesh of BP_PickupXP and BP_PickupXPBig (set on the CDO path), read back after compile and save.

## Subtasks
1. task-134-01-xp-pickups-no-shadow — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-134-01-xp-pickups-no-shadow]] — step 1, bp.
