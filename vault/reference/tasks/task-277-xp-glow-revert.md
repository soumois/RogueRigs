---
id: task-277-xp-glow-revert
description: "Global task: The XP pickups glow as before task-269: task-269-01 values reverted, PPV bloom 1.3 / -1 kept."
parent: tasks-done-index
kind: task
status: done
---
# XP glow back to before task-269

- request: remettre l'effet de lumière sur l'xp comme avant (owner, 2026-10-21)
- goal: The XP pickups glow as before task-269: task-269-01 values reverted, PPV bloom 1.3 / -1 kept.
- acceptance: Five values read back at their pre-task-269 values; assets saved; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser 25 -> 50, drone eye orange (1, 0.45, 0.08) x 6 with the colour deviation accepted, thruster table with white-teal kept, no captures, XP back to pre-269)
- milestone: M1
- next_step: none
- result: Done. Commit 4eac8d71. Read back after save: M_XPNut_hole GlowAmount 8 -> 25; MI_XPGlowDisc_Nut Radius 22 -> 50; MI_XPGlowDisc_Chip Radius 32 -> 68, Intensity 0.25 -> 0.5; M_XPCube_core_bright 6 -> 12, _dark 3 -> 6. PPV_XPGlow bloom and exposure, sparkle, trail, colours, pulse untouched. Docs bp-pickup-xp (xp_glow_revert, revert = task-269 values), perf-xp-pickups-glow. Not seen on screen.

## Subtasks
1. task-277-01-xp-glow-revert — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-277-01-xp-glow-revert]] — step 1, bp.
