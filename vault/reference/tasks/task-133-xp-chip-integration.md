---
id: task-133-xp-chip-integration
description: "Global task: Import the big XP chip mesh into Unreal, give it a glowing magenta core with a light pulse, use it in BP_PickupXPBig."
parent: tasks-done-index
kind: task
status: done
---
# Big XP chip integration in Unreal

- request: Integrate the validated data chip (Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx) as the big XP block mesh with a light bloom-friendly glow and no shadow (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Import the big XP chip mesh into Unreal, give it a glowing magenta core with a light pulse, use it in BP_PickupXPBig.
- acceptance: Blueprint compiles and is saved; mesh, seven slots and emissive in place; doc node updated; bloom status reported; no screenshot tool used; the two XP tasks never run in the Editor at the same time; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. SM_XPChipBig imported, 7 slot materials, BP_PickupXPBig scale 1.8 and TrailFX 0.3; pool restores the template scale (no C++ change); pivot offset not applied; bloom unconfirmed; CastShadow unchanged.

## Subtasks
1. task-133-01-xp-chip-integration — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-133-01-xp-chip-integration]] — step 1, bp.
