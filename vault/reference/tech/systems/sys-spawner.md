---
id: sys-spawner
description: "Enemy spawner as built: ARREnemySpawner placed actor, spawn band outside the projected camera view, reachable free cells, alive cap, tunables, rr.Spawner.Enabled."
parent: systems-index
kind: doc
---
# Enemy spawner

- responsibility: keep robots coming from outside the screen (M1 rule in [[enemies-index]]); count the robots it spawned that are alive.
- main_classes: `ARREnemySpawner` (`Source/RogueRigs/Enemies/RREnemySpawner.h/.cpp`, Blueprintable placed actor, ticks). Every `SpawnInterval` it tries one spawn: nothing when `GetAliveCount() >= MaxAlive` (the timer still runs: a freed slot waits for the next interval). The cell is drawn at random among candidates: walkable and reachable in the flow field ([[sys-flow-field]] `IsWalkableCell`, `GetCellDistance`), no pawn's actor location in it, and its center at a box distance from the view rectangle between `ViewMargin + 0.5` and `MaxSpawnDistanceCells + 0.5` cells (with 1 and 6: the whole cell at least one cell beyond the view edge, at most 6.5 cells). Spawn at the cell center, capsule bottom `SpawnFloorClearance` above the floor top (grid actor Z), collision handling AdjustIfPossibleButAlwaysSpawn. Alive = valid, not being destroyed, not `IsDead` (pruned on query: a kill frees the slot at once, the later destroy does not free a second one).
- view rectangle (world XY bounding box of the four frustum corner rays projected on the floor plane; rays above the horizon clamped to a 0.05 downward slope), first source available: (1) `SetViewRectangleOverride` (tests, scripted scenes; `ClearViewRectangleOverride`); (2) the first player controller's camera manager (`GetCameraCacheView`, aspect ratio of the game viewport, else the view's); (3) the first `UCameraComponent` of the flow-field goal actor (the miner's camera; headless worlds have no player controller). Measured with the miner camera ([[sys-player]] defaults) on cell (40, 40) (8100, 8100): X [7217, 10223], Y [5312, 10888].
- editable (`EditAnywhere`, placed instance or Blueprint): `EnemyClass` (none by default: set the chaser Blueprint), `SpawnInterval` 2 s, `MaxAlive` 20, `ViewMargin` 1 cell, `MaxSpawnDistanceCells` 6, `bEnabled` true, `SpawnFloorClearance` 2 uu. Values provisional ([[enemies-index]]).
- entry_points: `TrySpawnEnemy()` (ignores timer and enabled flags), `GetAliveCount()`, `GetAliveEnemies()`, `IsSpawningEnabled()`, `SetSpawningEnabled`, `GetViewRectangle`, `GetSpawnCandidates`, static `ProjectViewOnFloor(View, AspectRatio, FloorZ)`, `GetDistanceToRectangle`. Console: `rr.Spawner.Enabled 0/1` (default 1; cheat).
- data: none (tunables on the actor). Threat-driven spawn rate: open ([[difficulty]]).
- depends_on: [[sys-flow-field]], [[sys-grid]], [[sys-enemies]], [[sys-player]] (camera).
- implements: [[enemies-index]] M1 spawn rule.
- gotchas: candidates scan the whole grid each attempt (80 × 80, every 2 s). The bounding box of the trapezoid is wider than the near edge of the screen, so spawns there are farther than needed. Pawn occupancy uses actor locations only (a capsule overlapping a neighbour cell does not block it). No candidate (view covers the arena, miner missing) logs Verbose and skips. Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemies]] — the robots it spawns.
- [[sys-flow-field]] — reachability of a spawn cell.
- [[enemies-index]] — the spawn rule it implements.
- [[sys-automation-tests]] — `RogueRigs.Spawner.*` tests.
