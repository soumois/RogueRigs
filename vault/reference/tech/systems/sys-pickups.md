---
id: sys-pickups
description: "Pickups as built: ARRPickup (Ore, Heal) pops on spawn, flies to the miner within PickupRadius, is collected; grid drop rules and tunables."
parent: systems-index
kind: doc
---
# Pickups

- responsibility: the pickup actor (pop, attraction, collection) and the drop rule of ore and heal ore blocks. No heal or ore effect yet: collection only logs and broadcasts.
- main_classes: `ARRPickup` (`Source/RogueRigs/Pickups/RRPickup.h/.cpp`, `Abstract`, Blueprintable) and `ERRPickupType` (Ore, Heal). Root `PickupMesh` (static mesh, `NoCollision`, no overlap events, no navigation): pawns and projectiles pass through. Blueprint subclasses set `PickupType` and the mesh.
- lifecycle: spawned by `ARRArenaGrid` at the cell center (floor top), owner = grid. `BeginPlay` picks a pop target: spawn + random X and Y in [-`PopMaxOffset`, `PopMaxOffset`], Z + `HoverHeight`. The pop moves linearly over `PopDuration` with a parabolic bump of `PopArcHeight`. After the pop, each tick: miner = player 0 pawn cast to `ARRMiner` (cached); when the 2D distance is at most `ARRMiner::GetPickupRadius()`, the pickup latches attraction and flies to the miner's location at `FlySpeed`, even if the miner then moves away. Within `CollectDistance` (or when the step would overshoot): log `<Type> collected` on `LogRogueRigs`, `ARRMiner::NotifyPickupCollected(Type)` broadcasts `OnPickupCollected(Type)`, destroy.
- properties (all `EditDefaultsOnly`, provisional): `PickupType` Ore, `PopMaxOffset` 80 uu, `PopDuration` 0.35 s, `PopArcHeight` 80 uu, `HoverHeight` 30 uu, `FlySpeed` 1500 uu/s, `CollectDistance` 50 uu.
- drop rule (in `ARRArenaGrid`, see [[sys-grid]]): `OrePickupClass` for Ore, `HealPickupClass` for HealOre; other types drop nothing. Per cell the grid counts dig drops already spawned (`CellDigDrops`). After each `ApplyTerrainDamage` that leaves the block standing, crossed = floor((max health − health) / `DropInterval`), capped at the number of multiples strictly below max health; the difference with the count is spawned. The destroying hit spawns no dig drop; `DestroyCell` spawns `DestroyDropCount`, whatever the source. With 4.5 health and 1.5 interval: drops at 1.5 and 3.0, then 3 at destruction (5 total).
- entry_points: `ARRPickup::GetPickupType()`; `ARRMiner::GetPickupRadius()`, `ARRMiner::OnPickupCollected` ([[sys-player]]).
- data: `PickupRadius` from `PlayerBaseStats.csv` ([[sys-gas-core]]); drop tunables on the grid Blueprint.
- depends_on: [[sys-grid]], [[sys-player]], [[sys-gas-core]].
- implements: [[ore-blocks]], [[heal-ore]], [[ore-economy]] (drops only), [[player-stats]] (pickup radius).
- gotchas: one tick and one 2D distance test per pickup; fine at M1 drop counts, pool or batch if pickups reach hundreds. A single hit that both crosses a multiple and destroys the block gives no dig drop for that multiple (only with very large hits). Ore value and heal amount: open. Pickups spawn only in a game world.

## Relations
- [[systems-index]] — parent index.
- [[sys-grid]] — the drop properties and the damage path that spawns pickups.
- [[sys-player]] — the miner's pickup radius and collection event.
