---
id: sys-grid-rock-pieces
description: "Dual-grid rock display as built: corner piece HISMs per shape, variant and chunk, mask table, rebuild on destruction, floor slabs. Open to change rock rendering."
parent: sys-grid
kind: doc
---
# Grid — dual-grid rock pieces (task-230-07)

- responsibility: draw rock, ore, heal ore and special ore cells (`IsRockPieceType`; barrels keep their own mesh) with one slab piece per cell corner, design in [[rock-render-piece-set]]; gameplay, health and collision cubes unchanged.
- main_classes: `ARRArenaGrid` (`Grid/RRArenaGrid.h/.cpp`): `ERRRockPieceShape` (Full, Edge, Outer, Inner, Diagonal), `ERRRockPieceSet` (Intact, Broken: swap and meshes in [[sys-grid-rock-damage]]), `FRRRockPieceMeshes` (one mesh list per shape, entry = variant), `FRRGridPieceVisual` (set, shape, variant, chunk, HISM, `InstanceCorners`).
- build: corner (x, y) at (x · CellSize, y · CellSize), (W + 1) × (H + 1) corners; mask bit = (+X side) | (+Y side) << 1 over the 4 cells around. `BuildPieceTable` turns the canonical rock corners of `SM_RockSlab.report.json` (Y mirrored when `bRockPieceImportFlipsY`) by 0-3 yaw quarter turns and keeps the first shape and turn per mask. Variant = hash(corner, `RockPieceSeed`) mod mesh count; full pieces get a hash-chosen turn, the others the table turn. Transform: corner on the floor top, scale CellSize / `RockPieceMeshSize` horizontally, BlockHeight / `RockPieceMeshHeight` vertically. One HISM per set, shape, variant and chunk (corner / `ChunkCells`, last line in the last chunk), created on first instance from the `RockInstances` template, override materials cleared, `RockPieceMaterial` if set, NoCollision, `bDisallowNanite` = !`bRockPiecesUseNanite`. Instances added in one batch per HISM; maps `CornerPieceVisual`, `CornerPieceInstance`, plus `InstanceCorners` per HISM (remove at swap, patched like the cell maps). Logged: `N rock pieces in M piece HISMs`.
- floor: a slab under every non-void cell (`HasFloorSlab`), `bBlocksFillFloorSlab` false; a destruction adds no floor (guarded add kept).
- destruction: `DestroySingleCell` on a piece type calls `UpdateCellCornerPieces`: each of the 4 corners removes its old instance and adds the new shape (none when empty), `MarkRenderStateDirty` on add (task-202-01 rule); same HISM: `UpdateInstanceTransform`. Not batched per frame (open lead in [[rock-render-rebuild]]).
- properties (provisional): `RockPieceMeshes` (empty: meshes come with [[task-230-11-rock-slab-assign]]), `RockPieceMaterial` none, `RockPieceMeshSize` 67, `RockPieceMeshHeight` 100 (report values), `bRockPieceImportFlipsY` true (checked in game by [[task-230-12-rock-slab-check]]: outlines join, no mirroring seen), `RockPieceSeed` 1, `bRockPiecesUseNanite` true.
- gotchas: unset meshes are tolerated (pieces placed and tracked, nothing drawn, Log line): until task-230-11 the rock is invisible in game, only collision and floor. Rock and special ore have no per-cell render instance; shake, outline and damage live in the piece custom data, ore and heal ore draw crystals ([[sys-grid-rock-damage]]). `RockMesh`, `OreMesh`, `HealOreMesh`, `SpecialOreMesh` now only shape the collision ISMs. Tests: [[sys-tests-dig-ore]].

## Relations
- [[sys-grid]] — parent hub.
- [[sys-grid-rock-damage]] — custom data layout, broken swap and crystals on top of these pieces.
- [[rock-render-piece-set]] — the design built here.
- [[rock-render-rebuild]] — destruction design and the batching lead.
- [[sys-tests-dig-ore]] — instance-map and rock piece tests.
- [[task-230-07-dual-grid-placement]] — the task that built it.
