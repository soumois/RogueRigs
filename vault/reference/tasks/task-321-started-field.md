---
id: task-321-started-field
description: "Global task: every move of a task node to in-progress writes a started ISO date-time bullet, so the dashboard can flag stale in-progress tasks."
parent: tasks-done-index
kind: task
status: done
---
# started field on in-progress

- request: "Fais fiabiliser les statuts des tâches du vault : script de démarrage de tâche (task-start), champ `started`, et un statut `cancelled`." Owner-approved direction, point 2: a `started: <ISO date>` bullet written at every move to `in-progress`, so the dashboard can flag a task in-progress for hours with no live agent (owner, 2026-10-26). Owner answered "ok" to all nine questions (2026-10-26, relayed by the main session): Q1 yes, edit protocol step 5.1 and the _authoring status list; Q2 started on global tasks and subtasks; Q3 UTC ISO 8601 to the second (2026-10-26T23:35:54Z); Q4 no lint rule on started; Q5 cancelled globals go to tasks-done-index, line ending "Status cancelled."; Q6 cancelling a global sets its non-done subtasks cancelled and next_step none; Q7 task-start.js refuses a global without confirmed_by_owner: yes; Q8 no task for the stale task-106-01-wear-mask-texture; Q9 do not edit .claude/agents/executor.md. Run order: after task-319 is closed (it uses task-status.js), then task-320, task-321, task-322, one at a time.
- evidence (Planner, 2026-10-26): nothing detects a stale in-progress; task-106-01-wear-mask-texture is `in-progress` under a done global with nobody on it. No task node or template has a `started` field. Every scripted status change goes through `task-status.js` (task-next.js calls it), so writing `started` there covers task-next.js and task-start.js.
- goal: any `status=in-progress` set by script leaves `- started: <date-time>` on that node.
- acceptance: see the subtask `done_when`; templates and task-create.js carry the field; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Existing nodes are not back-filled (not requested). The dashboard reading `started` is out of scope (Tools/crew-board).
- confirmed_by_owner: yes (2026-10-26): "ok" to every default, Q1-Q9.
- milestone: M2
- next_step: none
- result: Done. task-status.js writes started (UTC ISO, replace or insert before result) on every move to in-progress; templates and task-create.js carry the field; runbook line added; verified on throwaway vault copy; commit 41f8969d

## Subtasks
1. task-321-01-started-field — cpp, editor any: task-status.js writes `started`; templates and task-create.js get the field.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-321-01-started-field]] — step 1, cpp.
