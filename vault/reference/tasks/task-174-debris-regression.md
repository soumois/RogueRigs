---
id: task-174-debris-regression
description: "Global task: Find with a real game run why debris pieces no longer show after tasks 170 to 173, and name the exact fix."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot debris: regression, pieces no longer visible

- request: J'ai une régression, je ne vois plus les débris de pièce lorsqu'un robot meurt (owner, 2026-10-14)
- goal: Find with a real game run why debris pieces no longer show after tasks 170 to 173, and name the exact fix.
- acceptance: Cause proven by values from a run; the commit that introduced it named; the fix stated precisely; nothing committed unless a C++ fix without build was possible; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: task-174-02-debris-position-fix
- result: open

## Subtasks
1. task-174-01-debris-regression — cpp
2. task-174-02-debris-position-fix — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-174-01-debris-regression]] — step 1, cpp.
- [[task-174-02-debris-position-fix]] — step 2, bp.
