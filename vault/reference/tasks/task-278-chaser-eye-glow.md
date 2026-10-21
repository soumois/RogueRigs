---
id: task-278-chaser-eye-glow
description: "Global task: The chaser and shield chaser red eyes glow stronger; the body look is unchanged."
parent: tasks-to-do-index
kind: task
status: todo
---
# Chaser eye glow stronger

- request: J'aimerais faire que l'oeil des chasseur glow plus (attention, ça ne doit pas impacter le reste de leur corps) (owner, 2026-10-21)
- goal: The chaser and shield chaser red eyes glow stronger; the body look is unchanged.
- acceptance: Leak check reported; GlowAmount read back on both masters and their instances; materials compile and are saved; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser 25 -> 50, drone eye orange (1, 0.45, 0.08) x 6 with the colour deviation accepted, thruster table with white-teal kept, no captures, XP back to pre-269)
- milestone: M1
- next_step: task-278-01-chaser-eye-glow
- result: open

## Subtasks
1. task-278-01-chaser-eye-glow — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-278-01-chaser-eye-glow]] — step 1, bp.
