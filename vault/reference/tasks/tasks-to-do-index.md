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
- [[task-103-hud-events-only]] — The HUD parts that still poll every frame (minimap redraw and texture, locator arrow, vignette warm-up) react to events instead; only genuinely continuous values keep a per-frame update. Status todo.
- [[task-101-barrel-asset]] — The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh. Status in-progress.
