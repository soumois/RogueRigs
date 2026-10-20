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
- result: Done. Components LaserPivot and AntennaMesh on ARRDrone (root Mesh = base); antenna spins 90 deg/s; base yaw turns toward the dig/hack target at 120 deg/s max; laser pitch aims the emitter at the impact point (240 deg/s, -20..+80, 0 = down); beam starts at the emitter tip (GetLaserEmitterLocation). Build OK, 25 tests pass (HealVein known), DroneParts: yaw error 104 to under 2 deg by frame 12 with no jump, arm axis within 0.46 deg of the beam, antenna 90.00 deg in 1 s. Mesh checks: antenna vertex colours had been lost at import, reimported with replace (3 colour classes read back); engine lightmap UVs removed on the three meshes. Not seen in play, hack not measured.

## Subtasks
1. task-253-01-drone-parts-animation — cpp
2. task-253-02-drone-parts-animation — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-253-01-drone-parts-animation]] — step 1, cpp.
- [[task-253-02-drone-parts-animation]] — step 2, bp.
