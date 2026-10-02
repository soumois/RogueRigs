---
id: sys-grid
description: "Grid terrain as built: ARRArenaGrid loads a layout JSON, builds floor and block HISMs, block health, dig damage, destruction, ore drops, shake; autotiling to come."
parent: systems-index
kind: doc
---
# Grid

- responsibility: terrain data, rendering, block health and destruction.
- main_classes: `ARRArenaGrid` (`Source/RogueRigs/Grid/RRArenaGrid.h/.cpp`, Blueprintable), `ERRCellType` (Floor, Void, Rock, Ore, HealOre, SpecialOre, Barrel, Spawn), `ERRTerrainDamageSource` (Drone, Barrel, Kamikaze). Reads `Content/RogueRigs/Data/Levels/<LayoutName>.json` ([[level-layout-schema]]) and fills six HISMs: floor, rock, ore, heal_ore, special_ore, barrel. `BP_ArenaGrid` sets layout, meshes, materials.
- build rules: origin = actor, floor top at actor Z, cell (x, y) center at ((x+0.5)·CellSize, (y+0.5)·CellSize). Floor instance (slab [-FloorThickness, 0]) only on open cells (floor, spawn); block instance only on block cells, spanning [-FloorThickness, BlockHeight] when `bBlocksFillFloorSlab` (default), else [0, BlockHeight]. No face is coplanar with another visible face. Collision BlockAll.
- lifecycle: built in `OnConstruction` (editor) and again in `BeginPlay`; `OnConstruction` is ignored once play has begun, so runtime destruction survives.
- properties (all editable): `LayoutName` `arena_m1`, `CellSize` 200, `BlockHeight` 100 (provisional), `FloorThickness` 200, `bBlocksFillFloorSlab` true, `MeshBaseSize` 100; health in dig-seconds at mining speed 1 (provisional): `RockHealth` 1.5, `OreHealth` 4.5, `HealOreHealth` 4.5, `SpecialOreHealth` 4.5, `BarrelHealth` 1.5; `ShakeAmplitude` 15 uu, `ShakeFrequency` 10 Hz (provisional, owner feel feedback task-008-02; not overridden in `BP_ArenaGrid`); drops: `OrePickupClass`, `HealPickupClass` (none; set to the pickup Blueprints), `DropInterval` 1.5 dig-seconds, `DestroyDropCount` 3 (provisional); `<Type>Mesh`/`<Type>Material`.
- entry_points: `GetCellCenter(X, Y)`, `GetCellAtWorld(FVector)`, `GetCellFaceCenter(Cell, Side)` (world center of the block face toward `Cell + Side`, at half `BlockHeight`; Side reduced to a unit step on its dominant axis), `GetCellSize()`, `GetBlockHeight()`, `GetSpawnCell()`, `GetCellType(FIntPoint)` (current type; destroyed = Floor), `IsBlockCell(FIntPoint)`, `GetCellHealth(FIntPoint)`, `ApplyTerrainDamage(Cell, Amount, Source)` (true when it destroyed the block), `DestroyCell(Cell, Source)`, `DestroyCellsInRadius(Center, Radius, Source)` (every block whose center is within the 2D radius, returns count), `SetCellShaking(Cell, bool)`. Delegates `OnCellDamaged(Cell, Type, Amount, Source)`, `OnCellDestroyed(Cell, Type, Source)`.
- drops: ore and heal ore spawn one pickup each time their accumulated damage crosses a multiple of `DropInterval` (never on the destroying hit) and `DestroyDropCount` in `DestroyCell`, whatever the source; rule detail in [[sys-pickups]].
- destruction: removes the block instance, adds a floor instance, cell becomes Floor, health 0, logged Verbose by type and source. Instance maps (cell → index, index → cell per HISM) mirror HISM remove-at-swap. Shake jitters the instance horizontally (actor ticks only while something shakes) and restores it on stop.
- rules (approved): the grid is the terrain source of truth; its use for pathfinding and line of sight is provisional ([[sys-enemy-ai]]). One HISM per asset type. Autotiling: 6 shapes by open sides, with variants; top face variants with random 90° rotation; neighbors re-shape on destruction. M1 grey box: one cube and one material per type ([[color-code]]).
- data: layout JSON (M1), deterministic generator (M2). Modules Json, JsonUtilities.
- depends_on: none.
- implements: [[arena-grid]], [[destruction-rules]], [[ore-blocks]], [[explosions]] (terrain side).
- gotchas: missing file or parse error logs on `LogRogueRigs` and leaves the grid empty. The layout folder must be packaged as a non-asset directory. Moving a shaking instance dirties the HISM tree each frame (fine for a few cells). Barrel detonation and the run log will react to the delegates; not done here. Autotiling, top-face variants and the destroy particle burst: not implemented. Chaos Destruction is not used; terrain damage stays outside GAS.

## Relations
- [[systems-index]] — parent index.
- [[level-layout-schema]] — file format the grid loads.
- [[sys-pickups]] — the pickups the grid drops from ore and heal ore.
- [[sys-automation-tests]] — automation tests of damage, destruction, drops and instance maps.
