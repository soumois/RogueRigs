---
id: task-322-cancelled-status
description: "Global task: add cancelled to the allowed task statuses, in the writing contract, lint, task-status.js, the tasks indexes and crew-board.js."
parent: tasks-to-do-index
kind: task
status: todo
---
# cancelled task status

- request: "Fais fiabiliser les statuts des tâches du vault : script de démarrage de tâche (task-start), champ `started`, et un statut `cancelled`." Owner-approved direction, point 3: `cancelled` added to the allowed statuses with what that implies for lint.js, task-status.js, the tasks indexes and the templates; the dashboard Cancel mapping must keep working (owner, 2026-10-26). Answers: open (questions sent 2026-10-26: Q1 rule edit, Q5 index, Q6 cascade).
- evidence (Planner, 2026-10-26): allowed task statuses are `todo | in-progress | review | done | blocked` in `_authoring` (kind: rule) and `TASK_STATUS` in lint.js line 30 and its message line 103. `task-status.js` moves only `done` globals to tasks-done-index. `crew-board.js` (vault script, HTML board) labels statuses at line 27. The templates hold `status: todo` only, no status list: no template change. Tools/crew-board `register.tsx` maps `/^cancel/` to Cancel, so `cancelled` already reads as Cancel there.
- goal: a task node can be `cancelled`, lints clean, and leaves the to-do list.
- acceptance: see the subtask `done_when`; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Editing `_authoring` (kind: rule) needs the owner's explicit intent: the request names the status; confirmation asked (Q1).
- confirmed_by_owner: no (plan sent 2026-10-26)
- milestone: M2
- next_step: task-322-01-cancelled-status
- result: open

## Subtasks
1. task-322-01-cancelled-status — cpp, editor any: _authoring, lint.js, task-status.js, tasks indexes, crew-board.js.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-322-01-cancelled-status]] — step 1, cpp.
