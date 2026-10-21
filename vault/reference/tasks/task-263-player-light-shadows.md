---
id: task-263-player-light-shadows
description: "Global task: The player carries a warm light that is the only source of cast shadows (rock walls), like the reference game: the walls block it and release it when they fall."
parent: tasks-to-do-index
kind: task
status: todo
---
# Player light and wall shadows

- request: only cast shadows: the player light on the walls; the light changes while digging (reference: DRGS video 1:10-1:15) (owner, 2026-10-21)
- goal: The player carries a warm light that is the only source of cast shadows (rock walls), like the reference game: the walls block it and release it when they fall.
- acceptance: rocks cast shadows only from the player light, robots, pickups, drone and miner cast none, the light exists on BP_Miner; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: task-263-01-player-light-shadows
- result: open

## Subtasks
1. task-263-01-player-light-shadows — cpp
2. task-263-02-player-light-shadows — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-263-01-player-light-shadows]] — step 1, cpp.
- [[task-263-02-player-light-shadows]] — step 2, bp.
