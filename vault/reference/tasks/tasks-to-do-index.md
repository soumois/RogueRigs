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
- [[task-182-debris-pool-80-test]] — Test 80 debris systems with debris living 4.5 s plus a 0.4 s sink, and measure what raising the cap costs. Status todo.
- [[task-184-minimap-opaque]] — Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included. Status todo.
- [[task-185-alert-label-remove]] — Remove the ALERT text next to the alert bar, and the one test check that reads it. Status todo.
- [[task-186-debris-throw-speed-range]] — Debris throw: final speed 325-600 cm/s along the hit direction and 350-550 cm/s upward, per-death factor 1.3-2 kept, angle and spread unchanged. Status todo.
- [[task-187-deposit-whole-on-explosion]] — When an explosion destroys any block of an ore deposit, the whole deposit is destroyed and pops all its chunks: no partly destroyed deposit. Status in-progress.
- [[task-188-editor-lock]] — Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it. Status todo.
