---
id: sys-spawner
description: "Enemy spawner as built: ARREnemySpawner, wave director or older trickle/packs, weighted classes, caps, spawn band, detour filter, run-heading sectors, recycling by relocation, tunables."
parent: systems-index
kind: doc
---
# Enemy spawner

- responsibility: keep robots coming from outside the screen (M1 rule in [[enemies-index]]); count the robots it spawned that are alive.
- main_classes: `ARREnemySpawner` (`Source/RogueRigs/Enemies/RREnemySpawner.h/.cpp`, Blueprintable placed actor, ticks).
  - pacing: run clock `GetRunTime` = seconds of enabled spawning. Default: the wave director ([[sys-spawner-director]], `DirectorConfig.bEnabled`). Older timers when it is off: trickle (run time < `TrickleDuration`): one `TrySpawnEnemy` every `SpawnInterval`. Pack phase (`IsInPackPhase`): one `TrySpawnPack` as the phase starts, then every `PackInterval`; between packs, every `SpawnInterval` (timer reset by each pack), a lone `TrySpawnEnemy` with chance `LoneSpawnChance` (defaults: two rolls per gap, about one lone robot every 5 gaps).
  - pack: one candidate scan; size drawn in [`PackSizeMin`, `PackSizeMax`], clamped to the free slots (`MaxAlive` − alive); candidates split by side of the view rectangle (`GetSideOfRectangle`: axis of the larger gap, X on a tie); side drawn uniformly among sides with at least that many cells, else the side with the most; distinct random cells of that side.
  - class: `PickEnemyClass`, weighted draw among `WeightedEnemyClasses` entries with a class and a weight > 0; `EnemyClass` when none is valid. Per-class cap (task-022-01): an entry whose `MaxAlive` > 0 is left out of the draw while that many robots of its class (IsA) from this spawner are alive (`GetAliveCountOfClass`), its weight going to the others; every valid entry capped = no spawn (no `EnemyClass` fallback). Every draw uses the spawner `FRandomStream` (new seed in BeginPlay; `SeedRandomStream` for tests).
- cap: `MaxAlive` (80 since task-046-01); nothing spawns at `GetOccupiedCount() >= MaxAlive`; timers still run. Alive = valid, not being destroyed, not `IsDead`, not parked in the pool (pruned on query: a kill frees the slot at once).
- editable (`EditAnywhere`): `EnemyClass` (none), `WeightedEnemyClasses` (`FRRWeightedEnemyClass` {`EnemyClass`, `Weight` 1, `MaxAlive` 0 = no cap}, empty in C++; target mix with the heavy: chaser 65, kamikaze 12, shooter 15, heavy 8 with `MaxAlive` 5, to be set on [[bp-enemy-spawner]] by task-022-02 (task-045-01 asks heavy `MaxAlive` 3: the cap lives only on the Blueprint, not in C++); before it chaser 70, kamikaze 15, shooter 15 (task-021-02)), `SpawnInterval` 2 s, `TrickleDuration` 30 s, `PackInterval` 6 s, `PackSizeMin` 8, `PackSizeMax` 15, `LoneSpawnChance` 0.1 per roll (chosen in task-014-01; the gdd says "rare"; these five only without the director), `MaxAlive` 150, `DirectorConfig` ([[sys-spawner-director]]), `SpawnBandMinDistance` 100, `SpawnBandMaxDistance` 500 (owner 2026-10-06; replace `ViewMargin` 3 / `MaxSpawnDistanceCells` 18, 234–1240 uu; no Blueprint or map override existed), `SpawnBandFallbackMaxDistance` 3000, `MaxPathDetourRatio` 1.5 (approved provisional by the owner), `HeadingWindowSeconds` 1, `HeadingMinSpeed` 150 uu/s (miner walks 600), `FrontSectorWeight` 55, `SideSectorWeight` 35, `BackSectorWeight` 10, `FrontHalfAngle` 45, `BackHalfAngle` 45, `bRecycleEnabled` true, `RecycleDistance` 1800, `RecycleDelay` 3 s, `RecycleUnseenDelay` 6 s, `RecycleOffScreenMargin` 0, `bEnabled` true, `SpawnFloorClearance` 2 uu. Provisional ([[enemies-index]]).
- entry_points: `GetPendingCount`, `GetOccupiedCount`, `GetLevelRobotClasses`, `BuildPoolPrewarmOrders`, `GetPoolPrewarmCount`, `QueuePoolPrewarm`, `HasQueuedPoolPrewarm`, `Get/SetSpawnMaxFrames`, `CancelPendingSpawns`, `TrySpawnEnemy`, `TrySpawnPack` (ignore timers and enabled flags), `PickEnemyClass(const FRandomStream&)`, `PickEnemyClass()` (spawner stream), `GetAliveCountOfClass`, `SeedRandomStream`, `GetRunTime`, `IsInPackPhase`, `GetSpawnableClasses` (used by [[sys-stress]]), `GetAliveCount`, `GetAliveEnemies`, `IsSpawningEnabled`, `SetSpawningEnabled`, `GetViewRectangle`, `GetSpawnCandidates`, static `ProjectViewOnFloor`, `ComputeCameraViewRectangle(World, FloorZ, FallbackCameraOwner)` (the camera part of the view rectangle without override; also the shooter's on-screen test, [[sys-enemies-shooter]]), `GetDistanceToRectangle`, `GetSideOfRectangle` (`ERRSpawnSide`), static `GetSectorOfPoint`, `GetRunHeading`, `GetRunSpeed`, `GetSpawnSector`, `SetHeadingOverride` / `ClearHeadingOverride` (tests), recycling queries above; `ARREnemy::RelocateTo` ([[sys-enemies]]). Director API and delegates: [[sys-spawner-director]]. Console: `rr.Spawner.Enabled 0/1` (cheat).
- data: none (tunables on the actor). Threat-driven spawn rate: open ([[difficulty]]).
- depends_on: [[sys-flow-field]], [[sys-grid]], [[sys-enemies]], [[sys-player]] (camera).
- implements: [[enemies-index]] M1 spawn rule.
- gotchas: a queued robot appears up to 20 frames (about 0.25 s at 80 fps) after its cell was chosen off screen; a fast run may bring the nearest band row to the screen edge meanwhile. One full-grid scan (240 × 240, 1.13 ms measured in [[stress-grid-cells]]) per single spawn or per pack. A side with fewer cells than the pack (arena edge) gives a smaller pack; other sides do not fill it. The bounding box of the trapezoid is wider than the near screen edge. Pawn occupancy uses actor locations only. No candidate logs Verbose and skips. Tests: [[sys-automation-tests]].

## Relations
- [[sys-spawner-pool-prewarm]] — Spawner pool use (task-091-01) and staggered prewarm (task-111-01): what is created ahead, over how many frames. Open to change pooling or prewarm.
- [[sys-spawner-cells-heading]] — Spawn cell choice: reachable candidate cells, run heading sectors and the view rectangle. Open to change where robots appear.
- [[sys-spawner-recycling]] — Recycling by relocation: far robots moved back instead of destroyed, and the rules around it. Open to change robot recycling.
- [[systems-index]] — parent index.
- [[sys-enemies]] — the robots it spawns.
- [[sys-flow-field]] — reachability of a spawn cell.
- [[enemies-index]] — the spawn rule it implements.
- [[sys-automation-tests]] — `RogueRigs.Spawner.*` tests.
- [[sys-stress]] — bulk stress spawns that ignore MaxAlive.
- [[bp-enemy-spawner]] — the placed Blueprint that sets the class weights.
- [[sys-spawner-director]] — the wave director: phases, packs, hordes, HUD delegates.
