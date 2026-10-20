---
id: task-252-drone-mesh-parts
description: "Global task: The Meshy drone is cleaned, split into base, laser and a modelled parabolic dish, and imported in Unreal with its material."
parent: tasks-done-index
kind: task
status: done
---
# Drone mesh in parts

- request: use the Meshy satellite drone mesh and the validated ChatGPT image as the drone model, split into parts (owner, 2026-10-20)
- goal: The Meshy drone is cleaned, split into base, laser and a modelled parabolic dish, and imported in Unreal with its material.
- acceptance: the three drone parts exist in Unreal with MI_Drone, the 3d renders were shown to the owner, docs written, lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. SM_DroneBase (4556 tris), SM_DroneLaser (754), SM_DroneAntenna (392) imported in /Game/RogueRigs/Art/Drone/ with MI_Drone / MI_DroneAntenna; bounds and pivots read back match the reports (Unreal Y mirrored: antenna foot offset (-4.257, -0.837, 28.752), emitter tip (-0.054, +0.143, -19) in the laser frame); LightMapCoordinateIndex set to 1; T_Drone_MetalRough capped to 2048. Unverified: antenna vertex colours and UV channels in the render data (headless check pending), look.

## Subtasks
1. task-252-01-drone-mesh-parts — 3d
2. task-252-02-drone-mesh-parts — 3d
3. task-252-03-drone-mesh-parts — bp
4. task-252-04-drone-mesh-parts — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-252-01-drone-mesh-parts]] — step 1, 3d.
- [[task-252-02-drone-mesh-parts]] — step 2, 3d.
- [[task-252-03-drone-mesh-parts]] — step 3, bp.
- [[task-252-04-drone-mesh-parts]] — step 4, bp.
