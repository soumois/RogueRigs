---
id: task-090-actor-pool
description: "Global task: A generic actor pool subsystem reuses actors and spreads the creation of missing ones over a capped number of frames."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Generic actor pool with staggered creation

- request: "pool de robots et étalement de l'initialisation (max 20 frames), pool des cubes d'XP (max 3 frames), une classe qui gère ça et généralisée, point 6 chevauchements" (owner 2026-10-10) (owner, 2026-10-11)
- goal: A generic actor pool subsystem reuses actors and spreads the creation of missing ones over a capped number of frames.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; sys-actor-pool written and linked from systems-index; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-090-01-actor-pool
- result: open

## Subtasks
1. task-090-01-actor-pool — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-090-01-actor-pool]] — step 1, cpp.
