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




- [[task-508-workflow-back-to-old]] — Removes gros-morceau split, 120k context handover and active budget from the crew rules; keeps reglage route, preview gate, task-confirm, failure-only runbook, extraProjectSlugs, three agents. Status wait-answers.
- [[task-509-options-old-workflow-on-master]] — Replaces the new-workflow options screen on master with the old-workflow one from exp/old-workflow-options (code, Blueprints, assets, images, docs), renumbers its task to 510, then pushes master. Status wait-answers.
