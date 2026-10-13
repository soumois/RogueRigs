---
id: task-111-pool-prewarm
description: "Global task: The pools are prewarmed at level start, spread over frames per class, and projectile pooling is evaluated."
parent: tasks-done-index
kind: task
status: done
---
# Staggered prewarm

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: The pools are prewarmed at level start, spread over frames per class (XP 30 over 20, chasers 30 over 20, other robots 5 or fewer over 60), and projectile pooling is evaluated.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Projectile pool evaluation: rifle 3.4 spawns/s at 1.13 ms (0.41 core), pellets 7 per shot at 0.54 ms, 0.11 ms per frame = 0.67 % of the game thread; saving about 0.07 ms per frame; recommendation: do not migrate now, revisit at 15+ projectiles per frame or 30+ per second. Note projectile-pool-evaluation. Commit dfdce4a.

## Subtasks
1. task-111-01-pool-prewarm — cpp
2. task-111-02-pool-prewarm — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-111-01-pool-prewarm]] — step 1, cpp.
- [[task-111-02-pool-prewarm]] — step 2, cpp.
