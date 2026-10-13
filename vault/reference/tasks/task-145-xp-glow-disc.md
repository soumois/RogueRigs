---
id: task-145-xp-glow-disc
description: "Global task: Add a flat glowing disc under the XP nut and chip: geometry in the FBX, additive material, reimport and Blueprint slots."
parent: tasks-done-index
kind: task
status: done
---
# XP pickups: glow disc on the floor

- request: A light effect on the floor and a halo around the XP nut and chip that does not depend on bloom or Lumen: a flat additive radial disc under each pickup (owner, 2026-10-13, plan accepted; no performance test requested) (owner, 2026-10-13)
- goal: Add a flat glowing disc under the XP nut and chip: geometry in the FBX, additive material, reimport and Blueprint slots.
- acceptance: both pickups show a glowing radial disc on the floor with the colour and intensity tunable in the editor; the FBX, assets and Blueprints committed; the performance test is NOT part of this task (owner declined it, 2026-10-13)
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. M_XPGlowDisc (unlit, additive, radial falloff) and two instances created; slot 2 of BP_PickupXP and slot 7 of BP_PickupXPBig set; values provisional (Intensity 0.5, Falloff 2); wishlist line done; no performance test (owner declined).

## Subtasks
1. task-145-01-xp-glow-disc — 3d (disc in the FBX files)
2. task-145-02-disc-reimport — bp (reimport, editor closed)
3. task-145-03-disc-material — bp (material and Blueprint slots, editor open)

## Relations
- [[tasks-done-index]] — parent index.
- [[task-145-01-xp-glow-disc]] — step 1, 3d.
- [[task-145-02-disc-reimport]] — step 2, bp.
- [[task-145-03-disc-material]] — step 3, bp.
