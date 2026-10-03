---
id: sys-pickups
description: "Pickups as built: ARRPickup (Ore, Heal, XP) pops, rests, flies to the miner within PickupRadius, is collected (XP adds XPAmount); grid drop rules and tunables."
parent: systems-index
kind: doc
---
# Pickups

- responsibility: the pickup actor (pop, attraction, collection) and the drop rule of ore and heal ore blocks. XP pickups add XP on collection; no heal or ore effect yet: those only log and broadcast.
- main_classes: `ARRPickup` (`Source/RogueRigs/Pickups/RRPickup.h/.cpp`, `Abstract`, Blueprintable) and `ERRPickupType` (Ore, Heal, XP). Root `PickupMesh` (static mesh, `NoCollision`, no overlap events, no navigation): pawns and projectiles pass through. Blueprint subclasses set `PickupType` and the mesh.
- lifecycle: spawned by `ARRArenaGrid` at the cell center (floor top), owner = grid. `BeginPlay` picks a pop target: spawn + random X and Y in [-`PopMaxOffset`, `PopMaxOffset`], Z + `HoverHeight`. The pop moves linearly over `PopDuration` with a parabolic bump of `PopArcHeight`. After the pop it rests `AttractDelay` seconds (counted from the end of the pop) and is never attracted during that rest. Then each tick: miner = player 0 pawn cast to `ARRMiner` (cached); when the 2D distance is at most `ARRMiner::GetPickupRadius()`, the pickup latches attraction and flies to the miner's location at `FlySpeed`, even if the miner then moves away. Within `CollectDistance` (or when the step would overshoot): log `<Type> collected` on `LogRogueRigs`; an XP pickup calls `GetProgression()->AddXP(XPAmount)` on the miner ([[sys-progression]], scaled by XPGain there); then `ARRMiner::NotifyPickupCollected(Type)` broadcasts `OnPickupCollected(Type)`, destroy.
- properties (all `EditDefaultsOnly`, provisional): `PickupType` Ore, `XPAmount` 1 (raw XP of an XP pickup), `PopMaxOffset` 80 uu, `PopDuration` 0.35 s, `PopArcHeight` 80 uu, `HoverHeight` 30 uu, `AttractDelay` 1.0 s (owner feel feedback, task-008-02), `FlySpeed` 1500 uu/s, `CollectDistance` 50 uu.
- XP drop: `ARREnemy` spawns `Stats.XPValue` pickups of its `XPPickupClass` at its feet after the death shrink ([[sys-enemies]]); tests use the native `ARRTestPickupXP`.
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
- [[sys-automation-tests]] — automation tests of drop counts and collection.
- [[sys-enemies]] — robots dropping XP pickups.
- [[sys-progression]] — where collected XP goes.
