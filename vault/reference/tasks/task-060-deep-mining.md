---
id: task-060-deep-mining
description: "Global task: Drone arc reaches 2.5 cells and cuts up to 8 blocks at once, about two rows deep."
parent: tasks-index
kind: task
status: done
---
# Mining in depth

- request: "Miner en profondeur aussi" (owner: "Testons ça" for 2.5 cells and 8 blocks) (owner, 2026-10-07)
- goal: Drone arc reaches 2.5 cells and cuts up to 8 blocks at once, about two rows deep.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. ARRDrone ArcReachCells 1.5 → 2.5, MaxArcBlocks 4 → 8 (no .cpp change; beam pool fills to 8; BP_Drone overrides neither): 6 blocks on a 2-cell wall, 8 (cap) on a thick wall, same dig time per block, front row first. Dig and fine-grid tests updated. Full run 194/195: FlowField.BudgetAt67uu fails alone and on the baseline without this change (goal age 33 > 30 frames). Commit a19be0a.

## Subtasks
1. task-060-01-deep-mining — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-060-01-deep-mining]] — step 1, cpp.
