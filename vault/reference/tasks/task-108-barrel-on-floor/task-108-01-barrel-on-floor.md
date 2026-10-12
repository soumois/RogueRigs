---
id: task-108-01-barrel-on-floor
description: "Barrels stand on the floor and keep the floor slab under them: the barrel is no longer sunk into a floorless cell."
parent: task-108-barrel-on-floor
kind: task
status: in-progress
---
# Barrel on the floor

- goal: Barrels stand on the floor and keep the floor slab under them: the barrel is no longer sunk into a floorless cell.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/sys-grid.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), barrel (vault/reference/gdd/terrain/barrel.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: Defect of task-107-01 (the Orchestrator brief asked the barrel bottom at the block bottom): in ARRArenaGrid::GetBlockRenderTransform the barrel is anchored at Bottom = -FloorThickness (-200 uu), so it sits at the bottom of the slab, seen through a hole in the floor. Fix, two points. (1) The barrel render transform anchors its bottom at the floor top (z = 0, never -FloorThickness): the barrel stands on the floor. (2) A barrel cell keeps a floor slab: besides its barrel HISM instance, the cell gets a floor instance exactly like an open cell (slab [-FloorThickness, 0], same floor HISM and material), so there is no hole under the barrel. The barrel cell stays a block cell for gameplay (collision block, health, destruction, explosion, shots, flow field and passability unchanged; the collision ISM keeps its block-sized transform). When a barrel is destroyed and the cell becomes open floor, no second floor instance may be created (no duplicate, no z-fighting): reuse the existing one or skip it, and keep the instance bookkeeping (indices, chunk counts, removal) consistent. Look at every place that creates, counts or removes floor instances (BuildInstances, destruction, dig, stress tools, tests) and keep them consistent; tests that assert floor instance counts get their expectations fixed if (and only if) this change breaks them, no new test. Update sys-grid (floor under barrels, barrel anchored on the floor). Build with the editor closed; no screenshot, no bot run. Do not change CellSize, BlockHeight or any value.
- done_when: build exits 0 with the editor closed; the RogueRigs.* tests pass headless; barrel bottom at z = 0 in the transform; barrel cells have a floor instance and no duplicate appears once the barrel is destroyed (explained in the report with the code path); collision and gameplay of barrels unchanged; sys-grid updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-108-barrel-on-floor]] — parent global task.
