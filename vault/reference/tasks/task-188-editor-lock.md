---
id: task-188-editor-lock
description: "Global task: editor lock shared by sessions: lock file, take and release script, runbook and protocol rules."
parent: tasks-to-do-index
kind: task
status: blocked
---
# Editor lock between sessions

- request: Comme j'ai parfois plusieurs discussions, comment faire pour que quand l'une d'entre elles a besoin de l'éditeur, les autres soient au courant, et le soient aussi quand la discussion n'a plus besoin de l'éditeur ? (owner, 2026-10-15)
- goal: Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it.
- acceptance: Script works for take, status, release and stale; docs updated; lint clean; committed by path; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-15).
- confirmed_by_owner: no (prepared only, owner 2026-10-15: not to run yet)
- milestone: M2
- next_step: task-188-01-editor-lock
- result: cancelled for now by the owner (2026-10-23); not run

## Subtasks
1. task-188-01-editor-lock — claude

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-188-01-editor-lock]] — step 1, claude.
