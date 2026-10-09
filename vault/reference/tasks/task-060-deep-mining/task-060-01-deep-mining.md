---
id: task-060-01-deep-mining
description: "Drone arc reaches 2.5 cells and cuts up to 8 blocks at once, about two rows deep."
parent: task-060-deep-mining
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-060-deep-mining]] — parent global task.
