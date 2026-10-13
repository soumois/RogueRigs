---
id: sys-grid-api
description: "Public functions of the arena grid: cell centers, face centers, world lookups, damage and destruction entry points. Open to call the grid from C++."
parent: sys-grid
kind: doc
---
# Grid — api

- entry_points: `GetCellCenter(X, Y)`, `GetCellAtWorld(FVector)`, `GetCellFaceCenter(Cell, Side)` (world center of the block face toward `Cell + Side`, at half `BlockHeight`; Side reduced to a unit step on its dominant axis), `GetCellSize()`, `GetGridSize()` (width, height in cells), `GetBlockHeight()`, `GetSpawnCell()`, `GetCellType(FIntPoint)` (current type; destroyed = Floor), `IsBlockCell(FIntPoint)`, `GetDepositCells(Cell, OutCells)` (C++, replaces `GetVeinCells` in task-070-01: the standing blocks of Cell's deposit, by id, Cell first; a block outside deposits gives Cell alone; a non-block nothing; used by the drone for whole-deposit mining and shake, [[sys-drone]]), `GetDepositId`, `GetDepositOreId`, `IsTableDepositCell`, `GetPlacedDepositCount(OreId)`, `GetDeposit(Id)`, `GetDepositCount`, `GetOreColorCustomDataIndex`, static `GetOreType(OreId, Row)`, `GetClearDistance(From, Direction, MaxDistance)` (grid line of sight on XY: exact cell traversal, distance to the first block cell entered, capped; 0 inside a block; a ray through a cell corner is blocked if either side cell is a block; void and floor never block; unscaled grid assumed), `HasLineOfSight(From, To)` (no block cell crossed, To's cell included; shotgun targets and wave clipping, [[sys-weapons]]), `GetCellHealth(FIntPoint)`, `ApplyTerrainDamage(Cell, Amount, Source)` (true when it destroyed the block), `DestroyCell(Cell, Source)`, `DestroyCellsInRadius(Center, Radius, Source)` (every block whose center is within the 2D radius, returns count), `QueueDestroyCellsInRadius` (C++, same cells spread over frames, explosions, [[sys-grid-barrels]]), `SetCellShaking(Cell, bool)`, `GetCellShotHealth(Cell)`, `GetCellFromHit(FHitResult)` (cell of the hit block instance via the instance maps, collision ISM or block HISM, else the cell 1 uu behind the impact point along the normal), `ApplyShotDamage(Cell, Damage)` (barrels only; true when it destroyed the barrel), `GetBarrelSource(Cell)`. Delegates `OnCellDamaged(Cell, Type, Amount, Source)`, `OnCellDestroyed(Cell, Type, Source)`. Objective marks (task-016-01, [[sys-objectives]]): `SetCellMarked(Cell, bMarked)` (block cells only), `IsCellMarked`, `GetMarkedCells`, `GetMarkedCustomDataIndex`; a mark writes `MarkedCustomDataValue` (1, else 0) into per-instance custom data float `MarkedCustomDataIndex` (0; each block HISM gets index + 1 floats at build) of the block's rendering instance, for a material outline, and calls the Blueprint event `OnCellMarkedChanged(Cell, bMarked, CellCenter)`; `DestroyCell` unmarks (event with false). Marks reset on rebuild; remove-at-swap moves custom data with the instance.

## Relations
- [[sys-grid]] — parent hub: summary and verdict.
- [[sys-drone]] — referenced above.
- [[sys-weapons]] — referenced above.
- [[sys-objectives]] — referenced above.
- [[sys-grid-barrels]] — referenced above.
