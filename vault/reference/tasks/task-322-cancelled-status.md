---
id: task-322-cancelled-status
description: "Global task: add cancelled to the allowed task statuses, in the writing contract, lint, task-status.js, the tasks indexes and crew-board.js."
parent: tasks-done-index
kind: task
status: done
---
# cancelled task status

- request: "Fais fiabiliser les statuts des tâches du vault : script de démarrage de tâche (task-start), champ `started`, et un statut `cancelled`." Owner-approved direction, point 3: `cancelled` added to the allowed statuses with what that implies for lint.js, task-status.js, the tasks indexes and the templates; the dashboard Cancel mapping must keep working (owner, 2026-10-26). Owner answered "ok" to all nine questions (2026-10-26, relayed by the main session): Q1 yes, edit protocol step 5.1 and the _authoring status list; Q2 started on global tasks and subtasks; Q3 UTC ISO 8601 to the second (2026-10-26T23:35:54Z); Q4 no lint rule on started; Q5 cancelled globals go to tasks-done-index, line ending "Status cancelled."; Q6 cancelling a global sets its non-done subtasks cancelled and next_step none; Q7 task-start.js refuses a global without confirmed_by_owner: yes; Q8 no task for the stale task-106-01-wear-mask-texture; Q9 do not edit .claude/agents/executor.md. Run order: after task-319 is closed (it uses task-status.js), then task-320, task-321, task-322, one at a time. Owner addition "oui, va pour le 322" (2026-10-26, status wait-answers): planned as [[task-323-wait-answers-status]], run after this one, by the splitting rule (one owner point = one global task).
- evidence (Planner, 2026-10-26): allowed task statuses are `todo | in-progress | review | done | blocked` in `_authoring` (kind: rule) and `TASK_STATUS` in lint.js line 30 and its message line 103. `task-status.js` moves only `done` globals to tasks-done-index. `crew-board.js` (vault script, HTML board) labels statuses at line 27. The templates hold `status: todo` only, no status list: no template change. Tools/crew-board `register.tsx` maps `/^cancel/` to Cancel, so `cancelled` already reads as Cancel there.
- goal: a task node can be `cancelled`, lints clean, and leaves the to-do list.
- acceptance: see the subtask `done_when`; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Editing `_authoring` (kind: rule) needs the owner's explicit intent: the request names the status; confirmation asked (Q1).
- confirmed_by_owner: yes (2026-10-26): "ok" to every default, Q1-Q9.
- milestone: M2
- next_step: none
- started: 2026-10-27T00:57:16Z
- executor_tokens: input 14, cache write 21217, cache read 160999, output 2847
- executor_usage: runs 1, turns 7, context start 16k peak 32k, resumes 0, 4 min, units 0.06 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. cancelled status allowed (authoring, lint, task-status.js moves cancelled globals to done index and cancels open subtasks, indexes, runbook, crew-board.js label and style); extra CRLF fix in crew-board.js parse() (3 live nodes crashed it); tested on throwaway task-999, reverted; commit d54f182d

## Subtasks
1. task-322-01-cancelled-status — cpp, editor any: _authoring, lint.js, task-status.js, tasks indexes, crew-board.js.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-322-01-cancelled-status]] — step 1, cpp.
- [[task-323-wait-answers-status]] — the wait-answers status added by the owner, run after this task.
