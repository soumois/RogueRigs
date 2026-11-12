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




- [[task-509-options-old-workflow-on-master]] — Replaces the new-workflow options screen on master with the old-workflow one from exp/old-workflow-options (code, Blueprints, assets, images, docs), renumbers its task to 510, then pushes master. Status todo.
