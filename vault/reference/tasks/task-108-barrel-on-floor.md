---
id: task-108-barrel-on-floor
description: "Global task: Barrels stand on the floor and keep the floor slab under them: the barrel is no longer sunk into a floorless cell."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Barrel on the floor

- request: le tonneau est dans le sol; il ne faut pas retirer la dalle sous le tonneau (owner 2026-10-11, capture du jeu après task-107) (owner, 2026-10-12)
- goal: Barrels stand on the floor and keep the floor slab under them: the barrel is no longer sunk into a floorless cell.
- acceptance: build exits 0 with the editor closed; the RogueRigs.* tests pass headless; barrel bottom at z = 0 in the transform; barrel cells have a floor instance and no duplicate appears once the barrel is destroyed (explained in the report with the code path); collision and gameplay of barrels unchanged; sys-grid updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: task-108-01-barrel-on-floor
- result: open

## Subtasks
1. task-108-01-barrel-on-floor — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-108-01-barrel-on-floor]] — step 1, cpp.
