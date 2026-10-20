---
id: task-247-drone-escort-movement
description: "Global task: The drone no longer circles the miner: it holds an escort slot beside and behind the miner that follows the miner velocity, with a damped spring and a slow figure-eight at rest."
parent: tasks-to-do-index
kind: task
status: todo
---
# Drone escort movement

- request: replace the drone circular orbit by a more interesting movement that follows the miner movement (owner, 2026-10-20)
- goal: The drone no longer circles the miner: it holds an escort slot beside and behind the miner that follows the miner velocity, with a damped spring and a slow figure-eight at rest.
- acceptance: Build passes; existing drone tests pass; one position check (headless, no capture): with the miner moving at constant velocity the drone settles behind it on the movement side, and at rest it stays within the escort distance; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-247-01-drone-escort-movement
- result: open

## Subtasks
1. task-247-01-drone-escort-movement — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-247-01-drone-escort-movement]] — step 1, cpp.
