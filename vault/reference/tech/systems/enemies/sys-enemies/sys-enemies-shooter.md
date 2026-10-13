---
id: sys-enemies-shooter
description: "ARRShooter as built: Approach, Hold, Reposition (flow-field detour), Aim, Dead; hold distance, never backs away, grid sight, on-screen gate, laser, Blueprint events, tunables."
parent: sys-enemies
kind: doc
---
# Shooter (`ARRShooter`)

- responsibility: the ranged robot's decisions, laser and shots.
- main_classes: `ARRShooter` (Blueprintable, `RRShooter.h/.cpp`), `ERRShooterState` Approach, Hold, Reposition, Aim, Dead. Components: `MuzzlePoint` (capsule child, (40, 0, −38): 50 uu above the floor), `LaserBeam` (static mesh under the muzzle, hidden, no collision). Shots: `ARREnemyProjectile` ([[sys-projectiles]]). Capsule 34 × 88 (miner size).
- per tick (alive): distance to the miner (2D), sight = `ARRArenaGrid::HasLineOfSight` from the actor (only within `AttackRange`), on screen = actor inside the view rectangle (`SetViewRectangleOverride`, else `ARREnemySpawner::ComputeCameraViewRectangle`, [[sys-spawner]]).
- fire: sight + on screen + cadence → Aim (stopped, faces the miner, laser on, `OnAimStart(Target)`); losing sight, range or screen cancels it (`OnAimEnd`, no shot, cadence kept); after `WindUp` → `OnAimEnd`, one shot at the miner's current position (speed `ProjectileSpeed`, lifetime `AttackRange / ProjectileSpeed`, `URRGE_DamageFlat` spec via `MakeDamageSpec`), `OnFire`. Next aim at last shot + 1 / `AttackRate` − `WindUp`: shots 2 s apart.
- movement (task-031-02: never backs away from the miner, owner 2026-10-06; task-038-01: flow-field detour), hold distance `GetRingOuterDistance()` = min(`PreferredDistance` + tolerance, `AttackRange`) = 960 uu, no inner edge: Approach (flow field; arrival = preferred distance with sight, 0 without) → Hold with sight within the preferred distance or arrived; → Reposition without sight within 960 uu. Hold → Approach beyond 960 uu; with sight it stays put however close the miner comes; → Reposition when sightless. Reposition = the flow field like the chasers (arrival 0, detours around rock) until sight returns → Hold and fire; → Approach beyond 960 uu. Within 960 uu every flow-field step goes through `MakeNonRetreatingDirection` (`URREnemyMovementComponent::SetDirectionFilter`, [[sys-flow-field]]): its away-from-the-miner part is removed and `RepositionInwardBias` inward added; a step pointing straight away gives no input (it waits).
- Blueprint-facing: events `OnAimStart(Target)`, `OnAimEnd`, `OnFire`; pure `GetState`, `IsAiming`, `GetAimTarget`, `GetAimOrigin`, `GetAimTargetPoint`, `GetLaserLength` (clipped by `GetClearDistance`), `GetAimProgress`, `HasSightOfTarget`, `IsOnScreen`. C++ drives `LaserBeam` while aiming: centered on the beam, X along it, scale (length, `LaserThickness`, `LaserThickness`) / `LaserMeshSize`.
- entry_points: the getters above, `Set/ClearViewRectangleOverride` (tests), `GetShotCount`, `GetLastShotTime`, `GetLastAimStartTime`.
- data: CSV row `Shooter` ([[sys-enemies]]); `EditDefaultsOnly` code defaults: `ProjectileClass` (native `ARREnemyProjectile`), `DamageEffectClass` (`URRGE_DamageFlat`), `LaserMeshSize` 100, `LaserThickness` 4, `RepositionInwardBias` 0.1; `ArrivalRangeRatio` 1. The cell search tunables (`RepositionSearchRadiusCells`, `RepositionSearchInterval`, `WaypointAcceptanceRadius`, `RepositionMaxDuration`) are gone (task-038-01).
- depends_on: [[sys-enemies-base]], [[sys-flow-field]], [[sys-grid]], [[sys-projectiles]], [[sys-spawner]].
- implements: [[enemy-shooter]].
- measured (task-038-01, speed 180): approached at 300 uu/s from 800 uu to 150 uu, largest step away 0.00 uu, 3 shots, never Reposition; open sight from 1600 uu: first aim at 958 uu (3.617 s); behind rock ((39, 48), miner (39, 43)): flow-field detour, largest step away during Reposition 0.00 uu, first aim from (42, 45) at 696 uu (6.517 s); `BP_Shooter` same, hit at 7.600 s.
- gotchas: without any view (no camera, no override) it never fires. Measured (range 960 since task-030-02): spawned 1600 uu away in open sight, first aim at 956 uu (t = 2.750 s), shots 0.6 s later and 2.000 s apart, holds at 956 uu (inside the ring). Tests: [[sys-automation-tests]].

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemy-ai]] — the pathfinding decision its detour reuses.
- [[sys-projectiles]] — the robot shot class.
