---
id: task-261-no-shadows
description: "Global task: No object casts a shadow for now: shadow casting is switched off on the lights of the arena level."
parent: tasks-to-do-index
kind: task
status: todo
---
# No shadows

- request: no object should have a shadow for now (owner, 2026-10-20)
- goal: No object casts a shadow for now: shadow casting is switched off on the lights of the arena level.
- acceptance: Shadow casting reads false on every light of the arena level after save; the node lists them with the revert steps; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-261-01-no-shadows
- result: open

## Subtasks
1. task-261-01-no-shadows — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-261-01-no-shadows]] — step 1, bp.
