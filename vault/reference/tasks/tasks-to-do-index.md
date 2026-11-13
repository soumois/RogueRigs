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




- [[task-523-audio-impact-near-quieter]] — Robot impact sound a little quieter near the player, unchanged near the screen edge. Status todo.
- [[task-527-audio-zone-objective-complete]] — One distance-faded sound when a zone objective (bonus chest, console hack) is validated. Status todo.
