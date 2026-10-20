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
- [[task-255-camera-arm-1400]] — The miner camera arm is 1400 uu instead of 1700, to test the closer view. Status in-progress.
- [[task-256-pickup-pop-distance]] — Ore and heal pickups land close enough to be collected without moving: PopMaxDistance lowered. Status todo.
- [[task-257-pulse-desync]] — Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate. Status todo.
