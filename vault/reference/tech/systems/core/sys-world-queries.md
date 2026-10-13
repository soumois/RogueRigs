---
id: sys-world-queries
description: "Shared world lookups as built (task-020-02): RRWorld::FindArenaGrid, FindMiner, FindLivingMiner; who calls them and the caching rule."
parent: systems-core-index
kind: doc
---
# World queries

- responsibility: the one place that answers "which arena grid" and "which miner" for a world, so a run that swaps grids or miners changes one function.
- main_classes: namespace `RRWorld` (`Source/RogueRigs/World/RRWorld.h/.cpp`), free functions, no state.
- entry_points: `RRWorld::FindArenaGrid(World)` (first `ARRArenaGrid`, null-safe); `RRWorld::FindMiner(World, bLivingOnly)` (first valid miner not being destroyed, not dead when `bLivingOnly`); `RRWorld::FindLivingMiner(World)`. Callers: `ARRMiner`, `ARRShotgun`, `URRExplosionSubsystem`, `URRFlowFieldSubsystem`, `URRRunFlowSubsystem`, `URRStressSubsystem`, `ARREnemy::FindTarget`, `RRCheats` (any miner).
- data: none.
- depends_on: [[sys-grid]], [[sys-player]].
- implements: open (plumbing, no gdd node).
- gotchas: they iterate actors: per-frame callers keep a weak cache and call them only when it is empty or stale (robots, flow field, run flow). Loops that act on every miner (cheats, stress invulnerability) stay loops. Placement scans use the flow field query `GatherReachableCells` ([[sys-flow-field]]).

## Relations
- [[systems-index]] — parent index.
- [[sys-flow-field]] — the reachable-cell query built on the same grid.
