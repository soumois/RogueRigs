---
id: task-190-debris-spin-damping
description: "Global task: debris spin slower and damped in the air, halved on bounces, stopped at rest."
parent: tasks-done-index
kind: task
status: done
---
# Robot debris: slower, damped spin

- request: Les pièces pivotent très vite et elles continuent sans ralentir sur le temps (owner, 2026-10-15); proposal accepted: 0.3-0.8 rev/s, half-life 1 s, halved on bounce, stops at rest (owner, 2026-10-15)
- goal: Debris spin slower and damped: 0.3-0.8 rev/s per piece, half-life 1 s in the air, halved at each bounce, stopped once at rest on the floor.
- acceptance: The new modules and expressions read back; compiles with no stack issue; saved by explicit path; docs updated; committed by path; a -game dump (fx.Niagara.DumpComponents) shows the spin speed decreasing over age and 0 for pieces at rest (values, no screenshots); committed.
- gdd_check: enemy-death read (2026-10-15): no spin value in the GDD, no conflict; owner accepted the proposal.
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: none
- result: Done. SpinSpeed 0.3-0.8 rev/s per piece drives UpdateMeshOrientation; update: x exp2(-dt/1) and x0.5 on a bounce (PrevVelZ < 0 -> Vz > 0 within 15 cm of the plane), 0 at rest (within 12 cm, speed < 20); -game dump: 0.57 -> 0.41 over 0.5 s (half-life 1 s), 0 at rest from about 1.8 s; bounce halving not isolated, feel unseen; commits a9c56d3c, 481ba8bd; validated on screen by the owner (2026-10-15)

## Subtasks
1. task-190-01-debris-spin-damping — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-190-01-debris-spin-damping]] — step 1, bp.
