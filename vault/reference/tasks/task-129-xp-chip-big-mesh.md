---
id: task-129-xp-chip-big-mesh
description: "Global task: Build the data chip mesh for the big XP block from the validated image, Blender route."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Big XP block mesh: data chip

- request: Blender route: build SM_XPChipBig from xp-chip-big.png for the XP pickup; budget 600 triangles (provisional, owner default 2026-10-13) (owner, 2026-10-13)
- goal: Build the data chip mesh for the big XP block from the validated image, Blender route.
- acceptance: Blender route, no Meshy call; SM_XPChipBig.fbx and report exist; tris_out at most 600; dimensions_cm match the placeholder cube footprint; flat shading; pivot bottom centre; materials as listed; UV checks pass; preview render produced with the barrel_preview camera for the Orchestrator to send to the owner; nothing imported in Unreal; image read from vault/assets/xp-chip-big.png; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-129-01-xp-chip-big-mesh
- result: open

## Subtasks
1. task-129-01-xp-chip-big-mesh — 3d

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-129-01-xp-chip-big-mesh]] — step 1, 3d.
