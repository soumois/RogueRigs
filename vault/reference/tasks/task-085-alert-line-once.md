---
id: task-085-alert-line-once
description: "Global task: the company alert line shows once for 8 s when the shuttle countdown starts; the number stays."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Alert line shown once

- request: "tu peux lancer le reste des tâches pour le devoir 4" (owner playtest 2026-10-10: shuttle line "Cease all extraction" misread, card flavor line not visible) (owner, 2026-10-10)
- goal: The company alert line shows once for 8 s at the start of the shuttle countdown, then hides; the countdown number stays to the end.
- acceptance: logic reads back from the graph (once flag, 8 s timer); compile 0 errors; asset saved; wbp-hud updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: task-085-01-alert-line-once
- result: open

## Subtasks
1. task-085-01-alert-line-once — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-085-01-alert-line-once]] — step 1, bp.
