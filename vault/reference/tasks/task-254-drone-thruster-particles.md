---
id: task-254-drone-thruster-particles
description: "Global task: Two engine particles, one per side pod, driven by the drone speed."
parent: tasks-done-index
kind: task
status: done
---
# Drone thruster particles

- request: add two particles for the two engines depending on the speed of the drone (owner, 2026-10-20)
- goal: Two engine particles, one per side pod, driven by the drone speed.
- acceptance: two thruster effects follow the drone speed, placed on the pods, docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. NS_DroneThruster at /Game/RogueRigs/FX/Drone/ (+ M_DroneThruster): soft white teal-tinted glow and downward streaks, local space, CPU, at most about 20 particles; User.Speed (uu/s, 700 = top speed) drives size, count, length and alpha, idle glow about 25 percent. Compiles 0 errors; node bp-drone-thruster. Look and C++ drive unverified.

## Subtasks
1. task-254-01-drone-thruster-particles — bp
2. task-254-02-drone-thruster-particles — cpp
3. task-254-03-drone-thruster-particles — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-254-01-drone-thruster-particles]] — step 1, bp.
- [[task-254-02-drone-thruster-particles]] — step 2, cpp.
- [[task-254-03-drone-thruster-particles]] — step 3, bp.
