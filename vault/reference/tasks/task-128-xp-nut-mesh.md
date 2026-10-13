---
id: task-128-xp-nut-mesh
description: "Global task: Build the hex nut mesh for the basic XP pickup from the validated image, Blender route."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP pickup mesh: hex nut

- request: Blender route: build SM_XPNut from xp-nut.png for the XP pickup; budget 300 triangles (provisional, owner default 2026-10-13) (owner, 2026-10-13)
- goal: Build the hex nut mesh for the basic XP pickup from the validated image, Blender route.
- acceptance: Blender route, no Meshy call; SM_XPNut.fbx and report exist; tris_out at most 300; dimensions_cm match the placeholder cube footprint; flat shading; pivot bottom centre; materials as listed; UV checks pass; preview render produced with the barrel_preview camera for the Orchestrator to send to the owner; nothing imported in Unreal; image read from vault/assets/xp-nut.png; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-128-01-xp-nut-mesh
- result: open

## Subtasks
1. task-128-01-xp-nut-mesh — 3d

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-128-01-xp-nut-mesh]] — step 1, 3d.
