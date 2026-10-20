---
id: task-248-drone-no-flight-over-walls
description: "Global task: The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner."
parent: tasks-done-index
kind: task
status: done
---
# Drone avoids walls

- request: the drone must not pass over the walls (owner, 2026-10-20)
- goal: The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner.
- acceptance: Build passes; existing drone tests pass; one position check (headless, no capture): with the miner next to a wall, the drone cell is never a block cell in Orbit, Dig and Hack; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Escort slot candidates (ideal, +-45, +-90, behind) filtered by line of sight on the grid; fallback above the miner; 0.3 s hold before switching to a better slot (a blocked slot is left at once); dig hover on the free side of the face (90 uu from it, 130 uu high); hack hover kept clear; last guard cuts moves into blocks. Build OK; position check DroneAvoidsWalls: 0 frames in a block in Orbit (371), Dig (30), Hack (90). HealVein still fails as before (known). Not seen in play.

## Subtasks
1. task-248-01-drone-no-flight-over-walls — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-248-01-drone-no-flight-over-walls]] — step 1, cpp.
