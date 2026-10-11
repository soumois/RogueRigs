---
id: task-070-ore-in-rock
description: "Global task: Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS."
parent: tasks-done-index
kind: task
status: done
---
# Ore scattered in the rock

- request: "disséminer le minerai dans la roche pour une prochaine tâche" (owner 2026-10-08) (owner, 2026-10-09)
- goal: Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS.
- acceptance: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-09).
- confirmed_by_owner: yes (2026-10-10), design decisions answered; gold only for now
- milestone: M2
- next_step: none
- result: Duskite deposits 2 x 2 at 8 %, table, mining and pool done; violet look kept (owner 2026-10-11); rename in task-100.

## Subtasks
1. task-070-01-ore-in-rock — cpp
2. task-070-02-ore-in-rock — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-070-01-ore-in-rock]] — step 1, cpp.
- [[task-070-02-ore-in-rock]] — step 2, bp.
