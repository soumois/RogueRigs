---
id: sys-grid
description: "Grid terrain as built: ARRArenaGrid loads a layout JSON, builds floor and block HISM instances; autotiling and destruction rules to come."
parent: systems-index
kind: doc
---
# Grid

- responsibility: terrain data, rendering and destruction.
- main_classes: `ARRArenaGrid` (`Source/RogueRigs/Grid/RRArenaGrid.h/.cpp`, Blueprintable) and `ERRCellType` (Floor, Void, Rock, Ore, HealOre, SpecialOre, Barrel, Spawn). In `OnConstruction` it reads `Content/RogueRigs/Data/Levels/<LayoutName>.json` (path from `FPaths::ProjectContentDir()`, format [[level-layout-schema]]) and rebuilds six HISM components: floor, rock, ore, heal_ore, special_ore, barrel. `BP_ArenaGrid` derives from it, sets the layout name, meshes and materials; one instance sits in the map.
- build rules: grid origin = actor; floor top at the actor's Z; cell (x, y) center at ((x+0.5)·CellSize, (y+0.5)·CellSize) in actor space. One floor instance per non-void cell (spawn included); one block instance per block cell, standing on its floor; nothing on void cells. Instances scale a `MeshBaseSize` centered cube to CellSize² × FloorThickness or BlockHeight. Collision BlockAll.
- properties: `LayoutName` (default `arena_m1`), `CellSize` 200 uu, `BlockHeight` 100 uu (provisional), `FloorThickness` 200 uu (cliff edge), `MeshBaseSize` 100 uu; `<Type>Mesh` and `<Type>Material` for Floor, Rock, Ore, HealOre, SpecialOre, Barrel (null by default, set in the Blueprint).
- entry_points: `GetCellCenter(X, Y)` (world position at floor top), `GetSpawnCell()` ((-1, -1) when none), `GetCellType(X, Y)` (Void outside the grid).
- M1 grey box (approved): engine or `LevelPrototyping` cube per type, one material instance per type after [[color-code]]; final meshes later replace references in `BP_ArenaGrid` only.

## Rules (approved)
- The grid is the source of truth for terrain. Using it for pathfinding and line of sight is provisional ([[sys-enemy-ai]]).
- Autotiling: a block's mesh depends on its open sides. With rotation, 6 shapes cover every case: buried, 1 open side, corner, corridor, wall end, isolated; each with a few variants. Not implemented yet.
- Top face: a few variants with a random 90° rotation (the most visible face under the near-vertical camera).
- Rendering: one Hierarchical Instanced Static Mesh per asset type; destroying a block removes instances.
- A destroyed block plays a particle burst; its neighbors swap to the shape matching their new open sides. Destruction not implemented yet.
- Terrain damage arrives through a dedicated interface, outside GAS.
- Chaos Destruction is not used.

- data: level layout JSON from the Level-Design agent (M1); deterministic generator (M2). Arena 60 × 60 cells in M1. Modules Json and JsonUtilities.
- depends_on: none.
- implements: [[arena-grid]], [[destruction-rules]], [[ore-blocks]].
- gotchas: a missing file or parse error is logged on `LogRogueRigs` with the path and leaves the grid empty, never crashes. The layout is re-read on every construction (each editor move or edit). The layout folder must be packaged as a non-asset directory ([[level-layout-schema]]). Destroyed blocks are logged by source and type ([[run-log-schema]]).

## Relations
- [[systems-index]] — parent index.
- [[level-layout-schema]] — file format the grid loads.
