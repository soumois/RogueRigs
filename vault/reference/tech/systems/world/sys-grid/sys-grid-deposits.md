---
id: sys-grid-deposits
description: "Ore deposit generation (OreTypes.csv, groups, seed) and the pickup drop rule of ore and heal ore. Open to change deposits or drops."
parent: sys-grid
kind: doc
---
# Grid — deposits

- deposits (task-070-01, [[ore-blocks]]): `FRROreTypeRow` (`Grid/RROreTypeRow.h`), one row per ore type in `Content/RogueRigs/Data/Tables/OreTypes.csv` (row name = ore id; columns `CellType`, `RockShare`, `GroupSize`, `ChunkValue`, `HealValue`, `Color`), read through `FRRBaseStatsLoader`; `Duskite` (renamed from Gold, task-100-01): Ore, 0.08, 2, 1, 0, colour alpha 0 (no colour written, the crystals keep the violet of their material; name provisional); `Heal` (task-088-02, [[heal-ore]]): HealOre, 0 (never placed: heal veins come from the layout), 2, 0, 15 HP per pickup, colour alpha 0 (no colour written, the heal material keeps its own). `BuildDeposits` runs after `LoadLayout`: layout Ore and HealOre cells become one deposit per 4-connected group, taking the first row (by id) of their block type (`arena_m1_fine`: 12 heal veins of 2 × 2, 48 cells, row `Heal`), then, when `bPlaceOreDeposits` (true), each row with a share places round(share × layout rock / size²) squares of GroupSize × GroupSize rock blocks, turned into the row CellType: anchors scored by a fixed integer hash of (x, y, `OreSeed` (1), CRC of the ore id), taken greedily while their square is still all rock, so only rock is converted (never heal ore, barrels, floor) and deposits may touch; no start-zone protection. Logged per type (`359 Duskite deposits of 2 x 2` on `arena_m1_fine`, 17 964 rock blocks after task-088-01; 44 on `arena_m1`). `FRRGridDeposit` (OreId, CellType, Cells, ChunkValue, HealValue, Color, `bHasColor`, `bPlacedFromTable`), `Deposits` (id = index), `CellDepositId` (cleared when a block breaks). Colour: block types of deposits with a row whose colour alpha is above 0 get custom data floats `OreColorCustomDataIndex` (1) to +2 = R, G, B on every instance, for the material (task-070-02). Consoles mark table deposit blocks like any rock inside their zone (`ARRConsole::FindMarkableCells`, [[sys-objectives]]; skipped before task-120-01).
- drops: ore and heal ore spawn one pickup each time their accumulated damage crosses a multiple of `DropInterval` (never on the destroying hit) and `DestroyDropCount` in `DestroyCell`, whatever the source, each with the ore value (`ChunkValue`, 1 without a row) and heal value (`HealValue`, 0 without a row: pickup default) of its deposit, requested from `URRPickupSubsystem::RequestSpawn` (4 per frame, rest queued); rule detail in [[sys-pickups]].

## Relations
- [[sys-grid]] — parent hub: summary and verdict.
- [[ore-blocks]] — referenced above.
- [[heal-ore]] — referenced above.
- [[sys-objectives]] — referenced above.
- [[sys-pickups]] — referenced above.
