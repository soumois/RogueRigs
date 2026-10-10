---
id: task-070-ore-in-rock
description: "Global task: Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Ore scattered in the rock

- request: "disséminer le minerai dans la roche pour une prochaine tâche" (owner 2026-10-08) (owner, 2026-10-09)
- goal: Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS.
- acceptance: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-09).
- confirmed_by_owner: yes (2026-10-10), design decisions answered; gold only for now
- milestone: M2
- next_step: task-070-02-ore-in-rock
- result: Design proposal written, awaiting the owner's values.

## Subtasks
1. task-070-01-ore-in-rock — cpp
2. task-070-02-ore-in-rock — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-070-01-ore-in-rock]] — step 1, cpp.
- [[task-070-02-ore-in-rock]] — step 2, bp.
