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
- [[task-495-generator-window-repair]] — the C++ layout generator enforces every layout review rule (24 x 24 windows, straight lanes, pocket distance) by repair and acceptance checks. Status in-progress.
- [[task-494-generator-test-map]] — a test map without robots where the owner generates a new layout on the fly with a console command and plays it. Status todo.
