---
id: ore-in-rock-plan
description: "Ore-in-rock proposal, part 2: performance budget for nugget pickups, where each change lands in code, follow-up subtasks with sizes, open values with defaults."
parent: ore-in-rock-proposal
kind: note
---
# Ore in the rock: plan (task-070-01, 2026-10-10)

All values `proposal`, numbered after [[ore-in-rock-proposal]].

## Performance budget
- Measured at 250 robots ([[perf-trace-real-game]]): plain game thread 10.42 ms avg; XP burst of 195 cubes cost 9.0 ms `SpawnActor` + 5.1 ms component instancing (Niagara `TrailFX` included), ≈ 0.07–0.1 ms per cube; a robot spawn costs 0.74 ms.
- 9. Drip cost at 3.2 nuggets/s ≈ 0.3 ms per second, ≈ 0.005 ms per frame; a full 8-block break gives 1–2 nuggets (≈ 0.2 ms). The drip alone fits without pooling.
- 10. Risk is explosions: every block in the radius breaks at once, 20 % of them drop. Cap: spawn at most 4 nuggets per frame (≈ 0.4 ms) from a queue in `URRPickupSubsystem`; 240/s capacity, far above the drip. Pooling `ARRPickup` (shared with perf target 3, XP cubes) is recommended but not required by this feature.
- 11. Nugget Blueprint without the trail Niagara component, the measured instancing cost.

## Where it lands in code
- Grid data (`ARRArenaGrid`, [[sys-grid]]): `NuggetDensity` 0.2, `NuggetSeed`; a nugget bit per Rock cell filled at build; `NuggetPickupClass`; `DestroyCell` queues one nugget for a nugget cell, any source; fleck through a per-instance custom data float on the rock HISM (as the objective marks); `HasNugget(Cell)`.
- Drone dig ([[sys-drone]]): no change expected; the drop rides on `DestroyCell`.
- Pickup ([[sys-pickups]]): spawn queue with the per-frame cap; `PickupType` Ore reused, so collection, events and the extraction count work unchanged.
- Tests: nugget count matches density on a seeded grid, deterministic for a seed, one drop per nugget block for drone and explosion, queue cap respected.

## Follow-up subtasks (after "ok")
1. cpp: grid nugget data, drop, custom data, tests. Size M.
2. cpp: pickup spawn queue with cap (pooling only if the owner wants it now, then L). Size S.
3. bp: `BP_PickupOreNugget` (small mesh, short pop, no trail), `BP_ArenaGrid` sets `NuggetPickupClass`, rock material fleck from custom data; doc nodes. Size S.
4. gdd: amend [[ore-blocks]] and [[ore-economy]] with the accepted values. Size S.

## Open values and proposed defaults
- Density: 20 %.
- Ore per nugget: 1; ore value of every chunk: 1.
- Visible fleck vs hidden: visible fleck.
- Explosions drop nuggets: yes, as Ore blocks.
- Pop distance 30–67 uu toward the miner, attract delay 0.5 s.
- Ore veins in the layout: none added.
- Share of rock dug per level (sets the real ore total): measure in the next playtest.
- Spawn cap per frame: 4; pooling: deferred.
- HUD ore counter: not found in the nodes read; check before the bp subtask.

## Relations
- [[ore-in-rock-proposal]] — parent: the numbered proposal.
- [[perf-trace-real-game]] — the measured spawn costs used here.
- [[sys-grid]] — where the nugget data and drop land.
- [[sys-pickups]] — where the spawn queue lands.
