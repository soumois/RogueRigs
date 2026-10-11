---
id: task-095-xp-cube-scale
description: "Global task: reused XP cubes come back at their normal size, the pooled reset restores the scale."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP cube scale after reuse

- request: "rapport de test du 2026-10-11" (owner) (owner, 2026-10-11)
- goal: Reused XP cubes come back at their normal size: the pooled reset restores the scale (and any other visual state that differs from a fresh cube).
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs sys-pickups and sys-actor-pool updated if a statement changes; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-095-01-xp-cube-scale
- result: open

## Subtasks
1. task-095-01-xp-cube-scale — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-095-01-xp-cube-scale]] — step 1, cpp.
