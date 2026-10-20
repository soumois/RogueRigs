---
id: task-248-drone-no-flight-over-walls
description: "Global task: The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Drone avoids walls

- request: the drone must not pass over the walls (owner, 2026-10-20)
- goal: The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner.
- acceptance: Build passes; existing drone tests pass; one position check (headless, no capture): with the miner next to a wall, the drone cell is never a block cell in Orbit, Dig and Hack; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-248-01-drone-no-flight-over-walls
- result: open

## Subtasks
1. task-248-01-drone-no-flight-over-walls — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-248-01-drone-no-flight-over-walls]] — step 1, cpp.
