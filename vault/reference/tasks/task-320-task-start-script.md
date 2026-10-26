---
id: task-320-task-start-script
description: "Global task: one script, task-start.js, sets a global task and its first subtask in-progress with next_step, run by the Executor at its step 0."
parent: tasks-to-do-index
kind: task
status: todo
---
# Task start script

- request: "Fais fiabiliser les statuts des tâches du vault : script de démarrage de tâche (task-start), champ `started`, et un statut `cancelled`." Goal stated by the main session: the owner follows the agents live on the crew-board dashboard (Tools/crew-board), which shows To do, In progress, Done, Cancel. Owner-approved direction, point 1: a script `vault/scripts/task-start.js` that sets the global task and its first subtask `in-progress` and the global `next_step` in one command, run by the Executor at its step 0 so the agent no longer has to remember (owner, 2026-10-26). Answers: open (questions sent 2026-10-26).
- evidence (Planner, 2026-10-26): protocol step 5.1 asks the agent to set the subtask in-progress by hand; only `task-next.js` (lines 29-30) does it by script, and only for the next subtask after one closes. Step 5.1 never says to set the global in-progress. task-319-01 was set in-progress by hand (commit bfa39517).
- goal: starting a confirmed global task is one command that leaves the global and its first open subtask `in-progress`, `next_step` set, lint clean and committed.
- acceptance: see the subtask `done_when`; protocol step 5.1, the Executor node and the runbook name the script; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Editing `orchestration-protocol` (kind: rule) needs the owner's explicit intent: asked (Q1). The `started` bullet is task-321, not this task; this script goes through `task-status.js`, so it gets `started` once task-321 is done.
- confirmed_by_owner: no (plan sent 2026-10-26)
- milestone: M2
- next_step: task-320-01-task-start-script
- result: open

## Subtasks
1. task-320-01-task-start-script — cpp, editor any: write task-start.js and update protocol step 5.1, agent-executor and runbook.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-320-01-task-start-script]] — step 1, cpp.
