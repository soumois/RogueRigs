---
id: task-280-drone-thruster-boost
description: "Global task: The drone thruster idle look equals its current top-speed look, and top speed is stronger than that."
parent: tasks-to-do-index
kind: task
status: todo
---
# Drone thruster particles boosted

- request: booster ses particules (ses particules quand il va vite maintenant, doit être celle quand il est au repos) (owner, 2026-10-21)
- goal: The drone thruster idle look equals its current top-speed look, and top speed is stronger than that.
- acceptance: Every lerp endpoint read back; system compiles (0 errors) and is saved; doc updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser 25 -> 50, drone eye orange (1, 0.45, 0.08) x 6 with the colour deviation accepted, thruster table with white-teal kept, no captures, XP back to pre-269)
- milestone: M1
- next_step: task-280-01-drone-thruster-boost
- result: open

## Subtasks
1. task-280-01-drone-thruster-boost — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-280-01-drone-thruster-boost]] — step 1, bp.
