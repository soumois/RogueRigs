---
id: task-263-player-light-shadows
description: "Global task: The player carries a warm light that is the only source of cast shadows (rock walls), like the reference game: the walls block it and release it when they fall."
parent: tasks-done-index
kind: task
status: done
---
# Player light and wall shadows

- request: only cast shadows: the player light on the walls; the light changes while digging (reference: DRGS video 1:10-1:15) (owner, 2026-10-21)
- goal: The player carries a warm light that is the only source of cast shadows (rock walls), like the reference game: the walls block it and release it when they fall.
- acceptance: rocks cast shadows only from the player light, robots, pickups, drone and miner cast none, the light exists on BP_Miner; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. BP_Miner gets PlayerLight (point light, child of the capsule, relative (0, 0, 300), warm (1.0, 0.62, 0.30), 60 cd, AttenuationRadius 1400, SourceRadius 20, inverse squared falloff, castShadows true dynamic only, no volumetric); compiled and saved; docs bp-miner and lights-no-shadows updated; perf knobs noted. Look, brightness against the ambient, shadow quality and frame cost unverified.

## Subtasks
1. task-263-01-player-light-shadows — cpp
2. task-263-02-player-light-shadows — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-263-01-player-light-shadows]] — step 1, cpp.
- [[task-263-02-player-light-shadows]] — step 2, bp.
