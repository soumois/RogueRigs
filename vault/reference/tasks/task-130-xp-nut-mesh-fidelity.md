---
id: task-130-xp-nut-mesh-fidelity
description: "Global task: Rebuild the XP hex nut mesh so it matches the validated image, judged by the fidelity loop."
parent: tasks-done-index
kind: task
status: done
---
# XP pickup mesh: hex nut, fidelity pass

- request: Correct task-128: the nut was too simplified (too tall, tiny flat hole, no chamfer); rebuild it to match vault/assets/xp-nut.png (owner, 2026-10-13); budget stays 300 triangles (owner, 2026-10-13)
- goal: Rebuild the XP hex nut mesh so it matches the validated image, judged by the fidelity loop.
- acceptance: All criteria C1 to C6 of the subtask pass in the comparison render, or the report lists the criteria still failing after 5 returns; tris_out at most 300; nothing imported in Unreal; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Hex nut SM_XPNut.fbx validated by the owner 2026-10-13: through hole, smooth wall, no blue, 108 tris, two grey slots M_XPNut_body and M_XPNut_hole (hole slot kept for a later Unreal emissive); 5 runs of the fidelity loop.

## Subtasks
1. task-130-01-xp-nut-mesh-fidelity — 3d

## Relations
- [[tasks-done-index]] — parent index.
- [[task-130-01-xp-nut-mesh-fidelity]] — step 1, 3d.
