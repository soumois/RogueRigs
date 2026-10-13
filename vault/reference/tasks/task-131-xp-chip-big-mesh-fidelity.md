---
id: task-131-xp-chip-big-mesh-fidelity
description: "Global task: Rebuild the big XP data chip mesh so it matches the validated image, judged by the fidelity loop."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Big XP block mesh: data chip, fidelity pass

- request: Correct task-129: the chip differed a lot from vault/assets/xp-chip-big.png (flat quad core, small bolts, wrong legs, one trace); budget raised to 1000 triangles if needed (owner, 2026-10-13); textured decals allowed for details (owner, 2026-10-13)
- goal: Rebuild the big XP data chip mesh so it matches the validated image, judged by the fidelity loop.
- acceptance: All criteria C1 to C8 of the subtask pass in the comparison render, or the report lists the criteria still failing after 5 returns; tris_out at most 1000; nothing imported in Unreal; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-131-01-xp-chip-big-mesh-fidelity
- result: open

## Subtasks
1. task-131-01-xp-chip-big-mesh-fidelity — 3d

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-131-01-xp-chip-big-mesh-fidelity]] — step 1, 3d.
