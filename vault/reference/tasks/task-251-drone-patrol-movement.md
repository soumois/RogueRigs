---
id: task-251-drone-patrol-movement
description: "Global task: The drone patrols a point inside the miner zone and follows it with a spring (faster when far, slower when near); the point changes when reached or every 3.5-5 s; wall rules are kept."
parent: tasks-done-index
kind: task
status: done
---
# Drone patrol movement

- request: replace the escort slot opposite to the movement by a patrol point inside the player zone, followed with a lag spring (owner, 2026-10-20)
- goal: The drone patrols a point inside the miner zone and follows it with a spring (faster when far, slower when near); the point changes when reached or every 3.5-5 s; wall rules are kept.
- acceptance: Build passes; existing drone, dig and run tests pass (HealVein excluded, task-250); ONE headless position check (no capture): with the miner at rest the drone offset does not depend on the miner facing, it stays inside the zone and its patrol point changes within 5 s; when the miner walks away the drone lags behind then catches up; walls rule still holds (0 frames in a block); docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Patrol replaces the escort: point 60-160 uu from the miner relative to it, re-picked on reach (25 uu) or 3.5-5 s, spring lag with speed 40-700 uu/s by distance; independent of miner facing. Build OK; DronePatrol check: two opposite facings give 0.000 uu offset difference, 0 frames out of the zone, drone 86.5 uu behind a miner walking at 600 uu/s then within 30 uu of its point 0.5 s after the stop; 0 frames in a block. HealVein still fails (task-250). Not seen in play: at rest the drone reaches its points in under a second (14 hops in 10 s), may look busy.

## Subtasks
1. task-251-01-drone-patrol-movement — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-251-01-drone-patrol-movement]] — step 1, cpp.
