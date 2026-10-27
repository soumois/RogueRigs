---
id: tasks-to-do-index
description: "Open global tasks (in progress, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done (`vault/scripts/task-status.js` does it).

## Relations
- [[tasks-index]] — parent index.
- [[task-188-editor-lock]] — Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it. Status blocked.
- [[task-308-chest-lock-light]] — Chest lock light: red blinking while waiting, orange blinking faster with hack progress in the zone, steady green once open. Status todo.
- [[task-309-chest-opening]] — Chest opening: latch swings, lid slams open with overshoot, freeze and shake, inner light, ground ring, three popping XP chips, sparks. Status todo.
- [[task-314-chest-lid-logo]] — Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game. Status todo.
- [[task-320-task-start-script]] — one script, task-start.js, sets a global task and its first subtask in-progress with next_step, run by the Executor at its step 0. Status in-progress.
- [[task-321-started-field]] — every move of a task node to in-progress writes a started ISO date-time bullet, so the dashboard can flag stale in-progress tasks. Status todo.
- [[task-322-cancelled-status]] — add cancelled to the allowed task statuses, in the writing contract, lint, task-status.js, the tasks indexes and crew-board.js. Status todo.
- [[task-323-wait-answers-status]] — add the wait-answers task status with a waiting_for bullet, set by Planner and Executor when they stop for the owner. Status todo.
