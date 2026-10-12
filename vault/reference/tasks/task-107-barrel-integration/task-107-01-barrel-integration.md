---
id: task-107-01-barrel-integration
description: "The validated barrel mesh replaces the grid barrel cylinder in the arena: C++ fits it to one cell, then BP_ArenaGrid uses it with its materials."
parent: task-107-barrel-integration
kind: task
status: done
---
# Barrel grid scale

- goal: The validated barrel mesh replaces the grid barrel cylinder in the arena: C++ fits it to one cell, then BP_ArenaGrid uses it with its materials.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/sys-grid.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), barrel (vault/reference/gdd/terrain/barrel.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: In ARRArenaGrid (Source/RogueRigs/Grid/RRArenaGrid.cpp, the block instance transform at about line 666) give barrel cells their own render transform: UNIFORM scale deduced from the bounds of BarrelMesh so that it fits one cell in width (CellSize, 67 uu) and BlockHeight (100 uu) in height (the smaller of the two factors), pivot at the BOTTOM of the mesh (the barrel mesh has its pivot at the bottom centre: Location z = block bottom, i.e. 0, or -FloorThickness when the cell is floorless, as the block code already computes), centred on the cell. If BarrelMesh is null or has zero bounds, fall back to the current cube-based transform. Collision is UNCHANGED: the hidden collision ISM of barrels keeps the current block-sized transform and its current behaviour (health, destruction, explosion, shots); state in the report how BarrelCollision gets its mesh and shape and make sure the new mesh (which will have no simple collision) cannot change it (for example keep the collision ISM on a basic cube or box as it is today if it uses the BarrelMesh). Other cell types keep the MeshBaseSize cube convention. No new gameplay value: CellSize and BlockHeight stay data; nothing hard-coded. Update sys-grid (the transform rule). Build with the editor closed (build-toolchain). Update an existing test only if the change breaks it; write no new test, bot run or screenshot.
- done_when: build exits 0 with the editor closed; the RogueRigs.* tests pass headless (existing ones; only fixed if broken by this change); barrels still use the cube transform when BarrelMesh is null; the collision of barrels is unchanged (stated in the report); sys-grid updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 35000
- model: opus
- tokens: input 36, cache write 40058, cache read 474224, output 8796
- runs: 1
- milestone: M2
- result: Barrel cells get their own render transform in ARRArenaGrid (uniform scale from the mesh bounds, S = min(CellSize/width, BlockHeight/height), bottom pivot at the block bottom, fallback to the cube transform); collision unchanged through the new BarrelCollisionMesh property (default Cylinder); build 0, 240 tests pass. Commit c850e83.

## Relations
- [[task-107-barrel-integration]] — parent global task.
