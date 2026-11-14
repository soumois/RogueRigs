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




- [[task-548-shop-frame-layout]] — The shop card panel gets a frame image made for its ratio (no stretched button frame) and the layout of owner image 8. Status in-progress.
- [[task-549-ui-shared-patterns]] — Audit of the UI widgets and UI build scripts after this batch; repeated patterns factored into shared pieces with no visible change, documented. Status todo.
