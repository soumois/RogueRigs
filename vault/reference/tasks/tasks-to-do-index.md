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
- [[task-247-drone-escort-movement]] — The drone no longer circles the miner: it holds an escort slot beside and behind the miner that follows the miner velocity, with a damped spring and a slow figure-eight at rest. Status in-progress.
- [[task-248-drone-no-flight-over-walls]] — The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner. Status todo.
- [[task-249-drone-alive-orientation-height]] — The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures. Status todo.
