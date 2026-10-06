---
id: sys-enemies-shooter
description: "ARRShooter as built: Approach, Hold, Reposition, Aim, Dead; hold distance, never backs away, grid sight, on-screen gate, laser, Blueprint events, reposition search, tunables."
parent: sys-enemies
kind: doc
---
# Shooter (`ARRShooter`)

- responsibility: the ranged robot's decisions, laser and shots.
- main_classes: `ARRShooter` (Blueprintable, `RRShooter.h/.cpp`), `ERRShooterState` Approach, Hold, Reposition, Aim, Dead. Components: `MuzzlePoint` (capsule child, (40, 0, −38): 50 uu above the floor), `LaserBeam` (static mesh under the muzzle, hidden, no collision). Shots: `ARREnemyProjectile` ([[sys-projectiles]]). Capsule 34 × 88 (miner size).
- per tick (alive): distance to the miner (2D), sight = `ARRArenaGrid::HasLineOfSight` from the actor (only within `AttackRange`), on screen = actor inside the view rectangle (`SetViewRectangleOverride`, else `ARREnemySpawner::ComputeCameraViewRectangle`, [[sys-spawner]]).
- fire: sight + on screen + cadence → Aim (stopped, faces the miner, laser on, `OnAimStart(Target)`); losing sight, range or screen cancels it (`OnAimEnd`, no shot, cadence kept); after `WindUp` → `OnAimEnd`, one shot at the miner's current position (speed `ProjectileSpeed`, lifetime `AttackRange / ProjectileSpeed`, `URRGE_DamageFlat` spec via `MakeDamageSpec`), `OnFire`. Next aim at last shot + 1 / `AttackRate` − `WindUp`: shots 2 s apart.
- movement (task-031-02: never backs away from the miner, owner 2026-10-06), hold distance `GetRingOuterDistance()` = min(`PreferredDistance` + tolerance, `AttackRange`) = 960 uu, no inner edge (the 600 uu retreat edge is gone): Approach (flow field; arrival = preferred distance with sight, 0 without) → Hold with sight within the preferred distance or arrived. Hold → Approach beyond 960 uu; with sight it stays put however close the miner comes; → search when sightless. Reposition → Hold with sight within 960 uu, at the path end, or after `RepositionMaxDuration`; a new search when the miner changes cell. Each reposition step goes through `MakeNonRetreatingDirection`: its away-from-the-miner part is removed and `RepositionInwardBias` inward added (zero = no input). Approach follows the flow field, which may detour around rock.
- search ([[sys-enemy-ai]]): at most every `RepositionSearchInterval`; local 8-neighbour BFS (no corner cutting) over the square of `RepositionSearchRadiusCells` around the robot; candidates from `URRFlowFieldSubsystem::GatherReachableCells` inside it, no farther from the miner than the robot now and within 960 uu, with grid sight to the miner; fewest local steps wins (ties: closest to the preferred distance). The robot walks the cell centers itself (`AddMovementInput`, skipping waypoints in `HasDirectPath` reach). No cell: Approach without sight, Hold with sight.
- Blueprint-facing: events `OnAimStart(Target)`, `OnAimEnd`, `OnFire`; pure `GetState`, `IsAiming`, `GetAimTarget`, `GetAimOrigin`, `GetAimTargetPoint`, `GetLaserLength` (clipped by `GetClearDistance`), `GetAimProgress`, `HasSightOfTarget`, `IsOnScreen`. C++ drives `LaserBeam` while aiming: centered on the beam, X along it, scale (length, `LaserThickness`, `LaserThickness`) / `LaserMeshSize`.
- entry_points: the getters above, `Set/ClearViewRectangleOverride` (tests), `GetShotCount`, `GetLastShotTime`, `GetLastAimStartTime`, `GetRepositionCell`.
- data: CSV row `Shooter` ([[sys-enemies]]); `EditDefaultsOnly` code defaults: `ProjectileClass` (native `ARREnemyProjectile`), `DamageEffectClass` (`URRGE_DamageFlat`), `LaserMeshSize` 100, `LaserThickness` 4, `RepositionSearchRadiusCells` 18 (6 cells of 200 uu × 3, task-029-04; BFS square 37 × 37 cells), `RepositionSearchInterval` 0.5 s, `WaypointAcceptanceRadius` 40, `RepositionMaxDuration` 4 s, `RepositionInwardBias` 0.1; `ArrivalRangeRatio` 1.
- depends_on: [[sys-enemies-base]], [[sys-flow-field]], [[sys-grid]], [[sys-projectiles]], [[sys-spawner]].
- implements: [[enemy-shooter]].
- measured (task-031-02, speed 168): approached at 300 uu/s from 800 uu to 150 uu, largest step away 0.00 uu, 3 shots, never Reposition; open sight from 1600 uu: first aim at 959 uu (3.867 s), holds 959–998 uu (the miner still); behind rock: 6-cell path to (43, 45), aim at 891 uu (6.733 s).
- gotchas: without any view (no camera, no override) it never fires. Measured (range 960 since task-030-02): spawned 1600 uu away in open sight, first aim at 956 uu (t = 2.750 s), shots 0.6 s later and 2.000 s apart, holds at 956 uu (inside the ring); behind rock 1000 uu away it walks a 6-cell path to (43, 45) and fires at 896 uu. Tests: [[sys-automation-tests]].

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemy-ai]] — the repositioning decision it implements.
- [[sys-projectiles]] — the robot shot class.
