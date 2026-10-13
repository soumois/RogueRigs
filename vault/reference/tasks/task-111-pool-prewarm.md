---
id: task-111-pool-prewarm
description: "Global task: The pools are prewarmed at level start, spread over frames per class, and projectile pooling is evaluated."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Staggered prewarm

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: The pools are prewarmed at level start, spread over frames per class (XP 30 over 20, chasers 30 over 20, other robots 5 or fewer over 60), and projectile pooling is evaluated.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-111-02-pool-prewarm
- result: open

## Subtasks
1. task-111-01-pool-prewarm — cpp
2. task-111-02-pool-prewarm — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-111-01-pool-prewarm]] — step 1, cpp.
- [[task-111-02-pool-prewarm]] — step 2, cpp.
