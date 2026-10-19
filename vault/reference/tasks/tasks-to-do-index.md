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
- [[task-188-editor-lock]] — Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it. Status todo.
- [[task-230-rock-slab-integration]] — Dual-grid slab rock look in the grid: pieces, world-UV top texture, cracks, plates, crystals. Status in-progress.
- [[task-234-ore-deposit-look]] — Each 2 x 2 ore or heal deposit is drawn as one crystal formation, at least 3 variants per type, from images the owner generates with prompts we write. Status in-progress.
- [[task-240-number-rise-variation]] — Each damage number rises with its own random speed (x0.7 to x1.3) and angle (up to 35 degrees left or right of screen up), so consecutive numbers no longer travel in parallel. Status in-progress.
- [[task-241-number-pop-duration]] — The pop of the damage numbers lasts 0.25 s instead of 0.1 s. Status todo.
- [[task-242-number-arc]] — Damage numbers are thrown in an arc (gravity) instead of rising in a straight line; peak about 60 uu, lifetime unchanged at 0.7 s. Status todo.
