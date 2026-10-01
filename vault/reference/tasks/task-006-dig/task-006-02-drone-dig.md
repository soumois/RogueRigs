---
id: task-006-02-drone-dig
description: "Dig trigger on the miner and the drone's Dig state: fly to the block, red laser, damage over time scaled by MiningSpeed, cancel rule."
parent: task-006-dig
kind: task
status: todo
---
# Drone digs

- goal: pushing into a block makes the drone cut it.
- agent: cpp
- depends_on: task-006-01-grid-destruction
- inputs: [[drone]], [[sys-drone]], [[sys-player]], [[sys-grid]], [[cpp-standard]].
- expected_output: `ARRMiner`: each tick, when move input points into an adjacent block cell (the cell next to the miner in the input direction, within an editable reach, default capsule radius + 30 uu), ask the drone to dig that cell; finds the grid once (first `ARRArenaGrid` in the world). `ARRDrone` Dig state: flies to a hover point beside the target block on the miner's side (same `HoverHeight`), applies `ApplyTerrainDamage(cell, DeltaTime × GetMiningSpeed(), Drone)` each tick, calls `SetCellShaking` on start and stop, shows a laser from the drone to the block (a `UStaticMeshComponent` named `LaserMesh`, scaled cylinder, mesh and material set in Blueprint, hidden outside Dig); stops and returns to Orbit when the block is destroyed or the miner is farther than 1.5 cells (editable) from the block. Damage stays on the block. [[sys-drone]] and [[sys-player]] updated.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; docs match; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-006-dig]] — parent global task.
