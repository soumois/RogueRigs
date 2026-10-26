---
id: task-321-started-field
description: "Global task: every move of a task node to in-progress writes a started ISO date-time bullet, so the dashboard can flag stale in-progress tasks."
parent: tasks-to-do-index
kind: task
status: todo
---
# started field on in-progress

- request: "Fais fiabiliser les statuts des tâches du vault : script de démarrage de tâche (task-start), champ `started`, et un statut `cancelled`." Owner-approved direction, point 2: a `started: <ISO date>` bullet written at every move to `in-progress`, so the dashboard can flag a task in-progress for hours with no live agent (owner, 2026-10-26). Answers: open (questions sent 2026-10-26: Q2 globals too, Q3 format, Q4 lint).
- evidence (Planner, 2026-10-26): nothing detects a stale in-progress; task-106-01-wear-mask-texture is `in-progress` under a done global with nobody on it. No task node or template has a `started` field. Every scripted status change goes through `task-status.js` (task-next.js calls it), so writing `started` there covers task-next.js and task-start.js.
- goal: any `status=in-progress` set by script leaves `- started: <date-time>` on that node.
- acceptance: see the subtask `done_when`; templates and task-create.js carry the field; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Existing nodes are not back-filled (not requested). The dashboard reading `started` is out of scope (Tools/crew-board).
- confirmed_by_owner: no (plan sent 2026-10-26)
- milestone: M2
- next_step: task-321-01-started-field
- result: open

## Subtasks
1. task-321-01-started-field — cpp, editor any: task-status.js writes `started`; templates and task-create.js get the field.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-321-01-started-field]] — step 1, cpp.
