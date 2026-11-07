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
- [[task-492-cpp-layout-generator]] — deterministic C++ port of the 240 x 240 generator recipe that writes a layout JSON, then a 20-seed validation by the Level-Design agent. Status todo.
- [[task-493-async-layout-loader]] — builds the grid from a layout JSON, loading its assets asynchronously and exposing a 0-100 load percentage for a future loading screen. Status todo.
- [[task-494-generator-test-map]] — a test map without robots where the owner generates a new layout on the fly with a console command and plays it. Status todo.
