---
id: task-132-xp-nut-integration
description: "Global task: Import the XP hex nut mesh into Unreal, give it a glowing material with a light emissive pulse, and use it in BP_PickupXP."
parent: tasks-done-index
kind: task
status: done
---
# XP nut integration in Unreal

- request: Integrate the validated hex nut (Art/Meshes/xp-nut-mesh/SM_XPNut.fbx) as the basic XP pickup mesh with a light bloom-friendly glow and no shadow (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Import the XP hex nut mesh into Unreal, give it a glowing material with a light emissive pulse, and use it in BP_PickupXP.
- acceptance: Blueprint compiles and is saved; mesh, two material slots and emissive in place; doc node bp-pickup-xp updated; bloom status reported; no screenshot tool used; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Nut mesh, three materials, BP_PickupXP updated; trail scale 0.3 relative to keep the old look; vertical offset not applied (root cannot be offset); bloom unconfirmed, no PostProcessVolume; CastShadow unchanged.

## Subtasks
1. task-132-01-xp-nut-integration — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-132-01-xp-nut-integration]] — step 1, bp.
