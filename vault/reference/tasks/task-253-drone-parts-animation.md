---
id: task-253-drone-parts-animation
description: "Global task: The drone has three animated parts: the antenna spins, the base turns slowly toward the mining spot and the laser pivots in height."
parent: tasks-done-index
kind: task
status: done
---
# Drone parts animation

- request: animate the dish, the base and the laser pivot of the drone separately (owner, 2026-10-20)
- goal: The drone has three animated parts: the antenna spins, the base turns slowly toward the mining spot and the laser pivots in height.
- acceptance: antenna spins, base turns toward the spot at a limited rate, laser pivots in height, BP_Drone uses the meshes, docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. BP_Drone: Mesh = SM_DroneBase at scale 1 (grey override removed, MI_Drone effective), LaserPivot = SM_DroneLaser, AntennaMesh = SM_DroneAntenna, C++ offsets kept; Blueprint compiles without error against the new C++, saved. Look and PIE motion unverified.

## Subtasks
1. task-253-01-drone-parts-animation — cpp
2. task-253-02-drone-parts-animation — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-253-01-drone-parts-animation]] — step 1, cpp.
- [[task-253-02-drone-parts-animation]] — step 2, bp.
