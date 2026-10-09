---
id: task-060-01-deep-mining
description: "Drone arc reaches 2.5 cells and cuts up to 8 blocks at once, about two rows deep."
parent: task-060-deep-mining
kind: task
status: done
---
# Mining in depth

- goal: Drone arc reaches 2.5 cells and cuts up to 8 blocks at once, about two rows deep.
- agent: cpp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/sys-drone.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: ARRDrone arc selection: ArcReachCells 1.5 → 2.5 and MaxArcBlocks 4 → 8 (editable), same 120° cone, same dig speed per block, nearest blocks first so the front row goes before the second; laser beam pool grows to 8; whole-vein rule unchanged; update only existing dig tests this breaks; docs sys-drone.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 64, cache write 152807, cache read 1243589, output 19219
- runs: 1
- milestone: M2
- result: ARRDrone ArcReachCells 1.5 → 2.5, MaxArcBlocks 4 → 8 (no .cpp change; beam pool fills to 8; BP_Drone overrides neither): 6 blocks on a 2-cell wall, 8 (cap) on a thick wall, same dig time per block, front row first. Dig and fine-grid tests updated. Full run 194/195: FlowField.BudgetAt67uu fails alone and on the baseline without this change (goal age 33 > 30 frames). Commit a19be0a.

## Relations
- [[task-060-deep-mining]] — parent global task.
