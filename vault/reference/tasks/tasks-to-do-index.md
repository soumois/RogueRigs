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
- [[task-189-ore-chunk-look]] — In-game look of the ore chunk pickup: a glowing Duskite violet crystal cluster like the HUD ore icon, Meshy mesh, violet trail, replacing the cyan cube. Status in-progress.
- [[task-192-pickup-spawn-orientation-variants]] — Pickup spawn: random rest orientation per Blueprint (none, yaw only, full) and a random mesh variant picked per drop, pool-safe. Status todo.
- [[task-193-debris-gpu-cost-remeasure]] — Remeasure the GPU debris with wall collision with the PC in its usual state, against task-182-03, frame for frame. Status blocked.
