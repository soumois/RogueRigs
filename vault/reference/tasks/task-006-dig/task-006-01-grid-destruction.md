---
id: task-006-01-grid-destruction
description: "Extend ARRArenaGrid: no floor under blocks, cell-to-instance map, block health, terrain damage and destruction API, shaking block."
parent: task-006-dig
kind: task
status: done
---
# Grid destruction

- goal: blocks can be damaged and destroyed one cell at a time, and the floor no longer has faces coplanar with block bottoms.
- agent: cpp
- depends_on: none
- inputs: [[sys-grid]], [[arena-grid]], [[destruction-rules]], [[ore-blocks]], [[heal-ore]], [[explosions]], [[drone]], [[cpp-standard]].
- expected_output: in `ARRArenaGrid`: (1) floor instances only on cells without a block; a destroyed block's cell gets a floor instance (the likely cause of the flicker; check for any other coplanar or overlapping faces and report). (2) A per-type map instance index ↔ cell, kept correct when `RemoveInstance` moves the last instance into the freed slot. (3) Block health per cell, in dig-seconds at mining speed 1: editable per type, defaults rock 1.5, ore 4.5, heal_ore 4.5, special_ore 4.5, barrel 1.5. (4) API: `GetCellAtWorld(FVector)`, `IsBlockCell(FIntPoint)`, `GetCellType`, `ApplyTerrainDamage(FIntPoint Cell, float Amount, ERRTerrainDamageSource Source)` returning true when destroyed, `DestroyCell(Cell, Source)`, `DestroyCellsInRadius(FVector Center, float Radius, Source)` (every block in the radius, per [[explosions]]); `ERRTerrainDamageSource` = Drone, Barrel, Kamikaze. (5) Multicast delegates `OnCellDamaged(Cell, Type, Amount, Source)` and `OnCellDestroyed(Cell, Type, Source)`. (6) `SetCellShaking(Cell, bool)`: the block instance jitters (editable amplitude, default 6 uu, and frequency) while shaking, restored when stopped. Runtime changes must not be wiped by `OnConstruction` during play. [[sys-grid]] updated.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; [[sys-grid]] matches; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: no floor under blocks (blocks fill the floor slab, `bBlocksFillFloorSlab`), floor added on destroy; cell↔instance maps with swap-remove; health per type in dig-seconds; GetCellAtWorld, IsBlockCell, GetCellType(FIntPoint), GetCellHealth, ApplyTerrainDamage, DestroyCell, DestroyCellsInRadius, SetCellShaking (6 uu, 20 Hz provisional), OnCellDamaged and OnCellDestroyed; runtime state rebuilt at BeginPlay. Flicker diagnosis: buried coplanar floor faces, most likely seen by Lumen and distance fields; owner to confirm. [[sys-grid]] updated. Build exit 0 (orchestrator re-ran it). About 77k tokens.

## Relations
- [[task-006-dig]] — parent global task.
