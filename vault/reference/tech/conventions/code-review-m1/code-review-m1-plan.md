---
id: code-review-m1-plan
description: "Ranked refactor list from the M1 review: gain, risk, hours, Blueprint impact per item, the scope recommended for task-020-02 and what is deferred."
parent: code-review-m1
kind: doc
---
# Refactor plan (ranked)

All items keep behavior identical. Findings `F<n>` / `H<n>` / `D<n>`: [[code-review-m1-gameplay]], [[code-review-m1-systems]], [[code-review-m1-hooks]]. "No BP" = no reflected name changes. Status: todo until 020-02 marks done or deferred.

## Recommended for task-020-02 (about 11 h, fits 150k tokens in this order)
| # | Item | Gain | Risk | h | Blueprint impact |
|---|---|---|---|---|---|
| R1 | **done (020-02)**: `SetMovementActive`, `IsTargetWithin`, `MarkStateStart` / `GetTimeInState` / `HasStateLasted`, `TimeTolerance`, `ApplyDamageSpecTo`, `UpdateTelegraphBlink`, `ArrivalRangeRatio` in `ARREnemy`. F1: in `ARREnemy` add `SetMovementActive(bool)`, `IsTargetWithin(Target, Range)`, `GetTimeInState()` + protected `StateStartTime`, one `TimeTolerance`, `ApplyDamageSpecTo(TargetASC, EffectClass, Damage)`, `UpdateTelegraphBlink(Rate)`; hoist `ArrivalRangeRatio` (defaults 0.9 / 0.5 set in subclass constructors). Chaser and kamikaze call them. | high (shooter, heavy reuse) | low | 2.5 | `ArrivalRangeRatio` moves to base, same name and type: no redirect; overridden by no asset. Do NOT add a native `BodyMesh` (BP_Chaser and BP_Kamikaze own one). |
| R2 | **done (020-02)**: `URRFlowFieldSubsystem::GatherReachableCells(OutCells, Filter, bFloorOnly, ExcludedCells)` + `GatherPawnCells`; seeded placements unchanged. F7: one reachable-cell query, e.g. `URRFlowFieldSubsystem::GatherReachableFloorCells(Filter)`, used by `FindConsoleCell`, `FindShuttleCell`, `GetSpawnCandidates`, stress spawn. Keep Y-outer, X-inner order so seeded tests draw the same cells. | high (shooter reposition) | low | 2 | No BP |
| R3 | F8: `FRRBaseStatsLoader::FindRow<T>(Path, RowName, OutRow)` with a per-path cache invalidated on file timestamp; replaces the 3 copies and the explosion cache; ends the per-spawn CSV parse. Optionally move to `Data/`. | medium | low | 1.5 | No BP |
| R4 | **done (020-02)**: `World/RRWorld.h`: `RRWorld::FindArenaGrid`, `FindMiner(World, bLivingOnly)`, `FindLivingMiner`; 6 grid and 4 miner first-lookups replaced (the remaining miner loops act on every miner). F6: `RRWorld::FindArenaGrid(World)` and `RRWorld::FindLivingMiner(World)` free functions; replace the 13 loops. | medium now, high for the 4-level run | low | 1.5 | No BP |
| R5 | **done (020-02)**: `ARRProjectile::CanHit` (default: `HitActorClass` filter, None = any), `bDamagesTerrain`, static `SpawnProjectile`; `ARRWeapon::GetAimDirection` / `GetShotOrigin` virtual; test `RogueRigs.Weapons.ProjectileHitFilter`. F4 + F5 seam: `ARRProjectile` gets `virtual bool CanHit(const AActor*) const` and `bDamagesTerrain` (default true); static `ARRProjectile::SpawnProjectile(...)` used by `ARRWeapon::FireOnce`; make `ARRWeapon::GetAimDirection` / `GetShotOrigin` virtual. | high (shooter, turrets) | low | 1.5 | New property only; no BP |
| R6 | F9: shared `ARRFloatingDamageText::SpawnForHit(...)`; enemy and dummy keep their properties and call it. Move the class to `UI/` or `Damage/`. | low-medium | low | 1 | No BP (properties unchanged, folder move is safe) |
| R7 | H1, H2, H3: cheat entry points into category `Cheats` with shipping early-return; gate `URRStressSubsystem` for shipping; gather the 18 test `Access` structs in `Tests/RRTestAccess.h`. | medium readability | low | 1 | Category only; no BP |

## Relations
- [[code-review-m1]] — parent index.
- [[code-review-m1-deferred]] — R8 to R16, deferred with their trigger.
- [[task-020-02-refactor]] — the task that executes the recommended scope.
- [[task-021-01-shooter-cpp]] — the next feature these items prepare.
