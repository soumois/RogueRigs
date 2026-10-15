---
id: task-186-debris-throw-speed-range
description: "Global task: Debris throw: final speed 325-600 cm/s along the hit direction and 350-550 cm/s upward, per-death factor 1.3-2 kept, angle and spread unchanged."
parent: tasks-to-do-index
kind: task
status: todo
---
# Robot debris: throw speed ranges

- request: On va passer de 325 - 600 pour la direction / 350 - 550 vers le haut. Le reste, reste le même. Tu peux faire une tâche pour ça aussi (owner, reading the final speeds after the factor F) (owner, 2026-10-15)
- goal: Debris throw: final speed 325-600 cm/s along the hit direction and 350-550 cm/s upward, per-death factor 1.3-2 kept, angle and spread unchanged.
- acceptance: The Velocity expression reads back with the new base ranges; compiles with no stack issue; saved by explicit path; docs updated; committed by path; the editor stays open; committed.
- gdd_check: enemy-death throw line read (2026-10-15): changes provisional values, no conflict; reading to confirm: the ranges are final speeds after F (F kept 1.3-2), so per-piece base becomes 250-300 and 269-275.
- confirmed_by_owner: yes (2026-10-15); final speeds after F, F kept 1.3-2
- milestone: M3
- next_step: task-186-01-debris-throw-speed-range
- result: open

## Subtasks
1. task-186-01-debris-throw-speed-range — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-186-01-debris-throw-speed-range]] — step 1, bp.
