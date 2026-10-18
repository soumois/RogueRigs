---
id: sys-grid-rock-damage
description: "Rock damage display as built: broken piece swap, data writes, shake and outline as data, ore crystal HISMs. Open to change damage display."
parent: sys-grid
kind: doc
---
# Grid — rock damage display data (task-230-08)

- responsibility: per-piece data the rock material reads ([[rock-render-damage]]), broken piece swap, ore and heal crystal clusters. Gameplay, health and collision unchanged.
- main_classes: `ARRArenaGrid` (`Grid/RRArenaGrid.h/.cpp`): `GetCellDisplayDamage`, `ComputeCornerPieceData`, `WriteCornerPieceData`, `RefreshPieceDataAround`, `BreakCellCornerPieces`, `CornerBroken`, `GetCrystalTransform`.
- data: 13 custom data floats per piece (damage, shake, marked, solid mask per quadrant), layout and damage rule in [[sys-grid-rock-piece-data]].
- broken swap: `ApplyTerrainDamage` on a piece cell sets `CornerBroken` on its 4 corners once d > 0 and rebuilds those pieces in the broken set; sticky until the next build (a destroyed cell's corners stay broken). Meshes `RockPieceBrokenMeshes` (per shape, entry = variant; `SM_RockSlabBroken_<Shape>_01/_02`, assigned in [[task-230-11-rock-slab-assign]]), `RockPieceBrokenMaterial` (null: `RockPieceMaterial`). Variant hashed per set, same turns as intact.
- writes: at build the solid mask only; then a damage tick or destruction rewrites corners x − 1..x + 2, y − 1..y + 2; shake start or stop and mark change rewrite the 4 corners. Only changed instances are written, each touched HISM dirtied once per frame. No instance moves for rock shake.
- crystals: ore and heal ore cells have a per-cell rendering instance again: one cluster in the ore or heal ore chunk HISM, lockstep with collision (removed with the cell, marked and ore colour floats as before). `OreCrystalMesh` default `SM_OreChunk_01`, `OreCrystalMaterial` / `HealOreCrystalMaterial` (null: mesh materials; heal material: [[task-230-10-heal-crystal-material]]), `OreCrystalScale` 1.5 (provisional), bounds bottom on the block top, hashed yaw. Crystals still shake by moving their instance (existing tick). Special ore and weak rock: no crystal.
- gotchas: meshes unset are tolerated; crystals do not follow the broken-piece sag (open). Tests: `RogueRigs.Dig.RockDamageData` ([[sys-tests-dig-ore]]).

## Relations
- [[sys-grid]] — parent hub.
- [[sys-grid-rock-piece-data]] — exact custom data layout and display damage rule; open for the rock material.
- [[sys-grid-rock-pieces]] — the piece HISMs this data rides on.
- [[rock-render-damage]] — the design implemented here.
- [[sys-tests-dig-ore]] — tests of the data, swap and crystals.
- [[task-230-08-rock-damage-data]] — the task that built it.
