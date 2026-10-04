---
id: sys-spawner
description: "Enemy spawner as built: ARREnemySpawner, 30 s trickle then one-side packs, lone robots, weighted classes, spawn band outside the view, reachable cells, cap, tunables."
parent: systems-index
kind: doc
---
# Enemy spawner

- responsibility: keep robots coming from outside the screen (M1 rule in [[enemies-index]]); count the robots it spawned that are alive.
- main_classes: `ARREnemySpawner` (`Source/RogueRigs/Enemies/RREnemySpawner.h/.cpp`, Blueprintable placed actor, ticks).
  - phases: run clock `GetRunTime` = seconds of enabled spawning. Trickle (run time < `TrickleDuration`): one `TrySpawnEnemy` every `SpawnInterval`. Pack phase (`IsInPackPhase`): one `TrySpawnPack` as the phase starts, then every `PackInterval`; between packs, every `SpawnInterval` (timer reset by each pack), a lone `TrySpawnEnemy` with chance `LoneSpawnChance` (defaults: two rolls per gap, about one lone robot every 5 gaps).
  - pack: one candidate scan; size drawn in [`PackSizeMin`, `PackSizeMax`], clamped to the free slots (`MaxAlive` − alive); candidates split by side of the view rectangle (`GetSideOfRectangle`: axis of the larger gap, X on a tie); side drawn uniformly among sides with at least that many cells, else the side with the most; distinct random cells of that side.
  - class: `PickEnemyClass`, weighted draw among `WeightedEnemyClasses` entries with a class and a weight > 0; `EnemyClass` when none is valid. Every draw uses the spawner `FRandomStream` (new seed in BeginPlay; `SeedRandomStream` for tests).
  - cap: nothing spawns at `GetAliveCount() >= MaxAlive`; timers still run. Alive = valid, not being destroyed, not `IsDead` (pruned on query: a kill frees the slot at once).
  - candidate cell: walkable and reachable in the flow field ([[sys-flow-field]] `IsWalkableCell`, `GetCellDistance`), no pawn's actor location in it, center at a box distance from the view rectangle between `ViewMargin + 0.5` and `MaxSpawnDistanceCells + 0.5` cells. Spawn at the cell center, capsule bottom `SpawnFloorClearance` above the floor top (grid actor Z), AdjustIfPossibleButAlwaysSpawn.
- view rectangle (world XY bounding box of the four frustum corner rays projected on the floor plane; rays above the horizon clamped to a 0.05 downward slope), first source available: `SetViewRectangleOverride` (tests; `ClearViewRectangleOverride`); the first player controller's camera manager (`GetCameraCacheView`, viewport aspect ratio); the first `UCameraComponent` of the flow-field goal actor (headless worlds). Miner camera on cell (40, 40): X [7217, 10223], Y [5312, 10888].
- editable (`EditAnywhere`): `EnemyClass` (none), `WeightedEnemyClasses` (`FRRWeightedEnemyClass` {`EnemyClass`, `Weight` 1}, empty in C++; default of the placed spawner with the shooter: chaser 70, kamikaze 15, shooter 15, set on [[bp-enemy-spawner]] by task-021-02; before it chaser 85, kamikaze 15), `SpawnInterval` 2 s, `TrickleDuration` 30 s, `PackInterval` 6 s, `PackSizeMin` 8, `PackSizeMax` 15, `LoneSpawnChance` 0.1 per roll (chosen in task-014-01; the gdd says "rare"), `MaxAlive` 100, `ViewMargin` 1, `MaxSpawnDistanceCells` 6, `bEnabled` true, `SpawnFloorClearance` 2 uu. Provisional ([[enemies-index]]).
- entry_points: `TrySpawnEnemy`, `TrySpawnPack` (ignore timers and enabled flags), `PickEnemyClass(const FRandomStream&)`, `PickEnemyClass()` (spawner stream), `SeedRandomStream`, `GetRunTime`, `IsInPackPhase`, `GetSpawnableClasses` (used by [[sys-stress]]), `GetAliveCount`, `GetAliveEnemies`, `IsSpawningEnabled`, `SetSpawningEnabled`, `GetViewRectangle`, `GetSpawnCandidates`, static `ProjectViewOnFloor`, `ComputeCameraViewRectangle(World, FloorZ, FallbackCameraOwner)` (the camera part of the view rectangle without override; also the shooter's on-screen test, [[sys-enemies-shooter]]), `GetDistanceToRectangle`, `GetSideOfRectangle` (`ERRSpawnSide`). Console: `rr.Spawner.Enabled 0/1` (cheat).
- data: none (tunables on the actor). Threat-driven spawn rate: open ([[difficulty]]).
- depends_on: [[sys-flow-field]], [[sys-grid]], [[sys-enemies]], [[sys-player]] (camera).
- implements: [[enemies-index]] M1 spawn rule.
- gotchas: one full-grid scan (80 × 80) per single spawn or per pack. A side with fewer cells than the pack (arena edge) gives a smaller pack; other sides do not fill it. The bounding box of the trapezoid is wider than the near screen edge. Pawn occupancy uses actor locations only. No candidate logs Verbose and skips. Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemies]] — the robots it spawns.
- [[sys-flow-field]] — reachability of a spawn cell.
- [[enemies-index]] — the spawn rule it implements.
- [[sys-automation-tests]] — `RogueRigs.Spawner.*` tests.
- [[sys-stress]] — bulk stress spawns that ignore MaxAlive.
- [[bp-enemy-spawner]] — the placed Blueprint that sets the class weights.
