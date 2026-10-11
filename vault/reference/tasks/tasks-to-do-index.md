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
- [[task-094-hud-refactor]] — The HUD is split into one widget per logic, all placed by WBP_HUD, which only lays them out; widgets react to events, tick only for continuous values. Status in-progress.
