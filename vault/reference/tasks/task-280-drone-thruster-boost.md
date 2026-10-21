---
id: task-280-drone-thruster-boost
description: "Global task: The drone thruster idle look equals its current top-speed look, and top speed is stronger than that."
parent: tasks-done-index
kind: task
status: done
---
# Drone thruster particles boosted

- request: booster ses particules (ses particules quand il va vite maintenant, doit être celle quand il est au repos) (owner, 2026-10-21)
- goal: The drone thruster idle look equals its current top-speed look, and top speed is stronger than that.
- acceptance: Every lerp endpoint read back; system compiles (0 errors) and is saved; doc updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser 25 -> 50, drone eye orange (1, 0.45, 0.08) x 6 with the colour deviation accepted, thruster table with white-teal kept, no captures, XP back to pre-269)
- milestone: M1
- next_step: none
- result: Done. Commit 5a1b8cfe. NS_DroneThruster lerp endpoints (idle, top), read back after save: Glow size (7, 15) -> (15, 22) cm, alpha (0.2, 0.8) -> (0.8, 1.0); Trail spawn (3, 40) -> (40, 70)/s, speed min (20, 100) -> (100, 150), max (30, 125) -> (125, 190), width (2, 4.5) -> (4.5, 6.5), length (3, 20) -> (20, 32), alpha (0.25, 0.7) -> (0.7, 1.0). Colours, lifetimes, cone, Trail bounds (-40,-40,-80)..(40,40,20) kept (longest streak about 70 cm). Compile UpToDate, 0 errors. Doc bp-drone-thruster (revert = old endpoints, about 20 live idle, 30 top). Not seen on screen.

## Subtasks
1. task-280-01-drone-thruster-boost — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-280-01-drone-thruster-boost]] — step 1, bp.
