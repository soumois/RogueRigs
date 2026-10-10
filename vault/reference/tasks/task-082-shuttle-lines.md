---
id: task-082-shuttle-lines
description: "Global task: A4 shuttle and extraction lines rewritten so the company states what it does and never orders the player (new rule T9), then reviewed."
parent: tasks-done-index
kind: task
status: done
---
# A4 shuttle and extraction lines

- request: "tu peux lancer le reste des tâches pour le devoir 4" (owner playtest 2026-10-10: shuttle line "Cease all extraction" misread, card flavor line not visible) (owner, 2026-10-10)
- goal: A4 shuttle and extraction lines rewritten so the company states what it does and never orders the player (new rule T9), then reviewed.
- acceptance: validate_text.js exits 0; 15 lines, 9 unchanged; critic verdict on all 15; no line gives the player an order that contradicts the objective; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: none
- result: Done. Shuttle and extraction lines rewritten under new rule T9; critic round 0: 11 pass 4 fail (3 T9, 1 T6), corrected; round 1: 15 pass. Run 20261010-112240-alerts.

## Subtasks
1. task-082-01-shuttle-lines — text
2. task-082-02-shuttle-lines — style-guide

## Relations
- [[tasks-done-index]] — parent index.
- [[task-082-01-shuttle-lines]] — step 1, text.
- [[task-082-02-shuttle-lines]] — step 2, style-guide.
