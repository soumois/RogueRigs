---
id: task-130-xp-nut-mesh-fidelity
description: "Global task: Rebuild the XP hex nut mesh so it matches the validated image, judged by the fidelity loop."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP pickup mesh: hex nut, fidelity pass

- request: Correct task-128: the nut was too simplified (too tall, tiny flat hole, no chamfer); rebuild it to match vault/assets/xp-nut.png (owner, 2026-10-13); budget stays 300 triangles (owner, 2026-10-13)
- goal: Rebuild the XP hex nut mesh so it matches the validated image, judged by the fidelity loop.
- acceptance: All criteria C1 to C6 of the subtask pass in the comparison render, or the report lists the criteria still failing after 5 returns; tris_out at most 300; nothing imported in Unreal; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-130-01-xp-nut-mesh-fidelity
- result: open

## Subtasks
1. task-130-01-xp-nut-mesh-fidelity — 3d

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-130-01-xp-nut-mesh-fidelity]] — step 1, 3d.
