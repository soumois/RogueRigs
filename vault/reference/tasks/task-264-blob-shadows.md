---
id: task-264-blob-shadows
description: "Global task: Simple fake round shadows under the enemies, the miner and the drone."
parent: tasks-done-index
kind: task
status: done
---
# Blob shadows

- request: the rest is blob: a simple fake shadow under the enemies (owner, 2026-10-21)
- goal: Simple fake round shadows under the enemies, the miner and the drone.
- acceptance: M_BlobShadow exists and robots, miner and drone carry a cheap blob shadow; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. URRBlobShadowComponent (engine Plane + MI_BlobShadow, no tick, no collision, no cast shadow, sort priority -10, absolute rotation) on ARREnemy (diameter = capsule radius x 2.2, hidden at death, restored on pool reuse), ARRMiner and ARRDrone (70 uu, on the floor under it, shrinks above 130 uu; updated from the drone's own tick). Build OK, 39 of 39 tests pass (Dig, Ore, PulseDesync, FineContact, RunFlow, Enemies); one existing enemy test filters the disc out of its body parts. Perf: translucent draw per robot (400 robots about 400 small draws), instanced alternative written as perf lead, not measured. Not seen on screen.

## Subtasks
1. task-264-01-blob-shadows — bp
2. task-264-02-blob-shadows — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-264-01-blob-shadows]] — step 1, bp.
- [[task-264-02-blob-shadows]] — step 2, cpp.
