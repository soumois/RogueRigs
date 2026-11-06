---
id: task-320-task-start-script
description: "Global task: one script, task-start.js, sets a global task and its first subtask in-progress with next_step, run by the Executor at its step 0."
parent: tasks-done-index
kind: task
status: done
---
# Task start script

- request: "Fais fiabiliser les statuts des tâches du vault : script de démarrage de tâche (task-start), champ `started`, et un statut `cancelled`." Goal stated by the main session: the owner follows the agents live on the crew-board dashboard (Tools/crew-board), which shows To do, In progress, Done, Cancel. Owner-approved direction, point 1: a script `vault/scripts/task-start.js` that sets the global task and its first subtask `in-progress` and the global `next_step` in one command, run by the Executor at its step 0 so the agent no longer has to remember (owner, 2026-10-26). Owner answered "ok" to all nine questions (2026-10-26, relayed by the main session): Q1 yes, edit protocol step 5.1 and the _authoring status list; Q2 started on global tasks and subtasks; Q3 UTC ISO 8601 to the second (2026-10-26T23:35:54Z); Q4 no lint rule on started; Q5 cancelled globals go to tasks-done-index, line ending "Status cancelled."; Q6 cancelling a global sets its non-done subtasks cancelled and next_step none; Q7 task-start.js refuses a global without confirmed_by_owner: yes; Q8 no task for the stale task-106-01-wear-mask-texture; Q9 do not edit .claude/agents/executor.md. Run order: after task-319 is closed (it uses task-status.js), then task-320, task-321, task-322, one at a time.
- evidence (Planner, 2026-10-26): protocol step 5.1 asks the agent to set the subtask in-progress by hand; only `task-next.js` (lines 29-30) does it by script, and only for the next subtask after one closes. Step 5.1 never says to set the global in-progress. task-319-01 was set in-progress by hand (commit bfa39517).
- goal: starting a confirmed global task is one command that leaves the global and its first open subtask `in-progress`, `next_step` set, lint clean and committed.
- acceptance: see the subtask `done_when`; protocol step 5.1, the Executor node and the runbook name the script; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Editing `orchestration-protocol` (kind: rule) needs the owner's explicit intent: asked (Q1). The `started` bullet is task-321, not this task; this script goes through `task-status.js`, so it gets `started` once task-321 is done.
- confirmed_by_owner: yes (2026-10-26): "ok" to every default, Q1-Q9.
- milestone: M2
- next_step: none
- executor_tokens: input 20, cache write 41440, cache read 261373, output 4036
- executor_usage: runs 1, turns 10, context start 16k peak 41k, resumes 0, 3 min, units 0.10 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Done. vault/scripts/task-start.js written; protocol step 5.1 and Roles, agent-executor and runbook name it. Tested on a scratch worktree copy: one run sets in-progress and commits only 3 files, second run no-op exit 0, confirmed_by_owner: no refused. Lint clean.

## Subtasks
1. task-320-01-task-start-script — cpp, editor any: write task-start.js and update protocol step 5.1, agent-executor and runbook.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-320-01-task-start-script]] — step 1, cpp.
