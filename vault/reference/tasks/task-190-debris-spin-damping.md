---
id: task-190-debris-spin-damping
description: "Global task: Debris spin slower and damped: 0.3-0.8 rev/s per piece, half-life 1 s in the air, halved at each bounce, stopped once at rest on the floor."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot debris: slower, damped spin

- request: Les pièces pivotent très vite et elles continuent sans ralentir sur le temps (owner, 2026-10-15); proposal accepted: 0.3-0.8 rev/s, half-life 1 s, halved on bounce, stops at rest (owner, 2026-10-15)
- goal: Debris spin slower and damped: 0.3-0.8 rev/s per piece, half-life 1 s in the air, halved at each bounce, stopped once at rest on the floor.
- acceptance: The new modules and expressions read back; compiles with no stack issue; saved by explicit path; docs updated; committed by path; a -game dump (fx.Niagara.DumpComponents) shows the spin speed decreasing over age and 0 for pieces at rest (values, no screenshots); committed.
- gdd_check: enemy-death read (2026-10-15): no spin value in the GDD, no conflict; owner accepted the proposal.
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: task-190-01-debris-spin-damping
- result: open

## Subtasks
1. task-190-01-debris-spin-damping — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-190-01-debris-spin-damping]] — step 1, bp.
