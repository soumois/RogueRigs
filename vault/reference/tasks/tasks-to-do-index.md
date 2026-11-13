---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.




- [[task-512-audio-shot-quieter]] — Lower the volume of the player shot sound, every weapon, as a table value the owner retouches by ear. Status todo.
- [[task-513-audio-wall-impact-distance]] — Split the wall impact from the enemy impact and make it quieter, fading with the distance from the player; values in the event table. Status todo.
