---
id: sys-enemies-movement
description: "Robot light movement as built: kinematic step behind rr.Enemy.LightMovement (task-071-01), cached floor, light attached parts, far-movement tick LOD, tunables."
parent: sys-enemies
kind: doc
---
# Robot light movement

- responsibility: keep horde movement cheap without changing paths or speeds (category `Enemy|Performance`).
- main_classes: `URRRobotMovementComponent` (`RRRobotMovementComponent.h/.cpp`, the CharacterMovement class set by `SetDefaultSubobjectClass` in `ARREnemy`, so every caller keeps the CharacterMovement API: `MaxWalkSpeed` from MoveSpeed, `PendingLaunchVelocity`, `StopMovementImmediately`, `DisableMovement`, `IsMovingOnGround`).
- light step (task-071-01): console variable `rr.Enemy.LightMovement` (1 default, 0 = engine path, same build). While the robot walks at the floor height recorded by its last engine-path tick (± `LightHeightTolerance` 0.5 uu), `TickComponent` replaces the engine walk: input acceleration and the engine `CalcVelocity` (same speed, acceleration, friction, braking), orient-to-movement yaw, one swept capsule move carrying translation and yaw, a 2D slide on a blocking hit (rock, the miner), all in one deferred scoped update (one child transform and overlap update per frame). No floor search, no step up, no separate rotation move, height never changes. Knockback: a pending launch replaces the horizontal velocity and stays unbraked for the engine's fall time from the recorded floor gap (sqrt(2 × gap / gravity) ≈ 0.066 s); a launch within it keeps the remaining time. Robots cannot leave the flat floor, so there is no gravity on this path.
- engine path (falls back automatically, then records the floor again): not walking (spawn fall, death `MOVE_None`), height off the recorded floor (relocation or teleport to another height, penetration push), pending impulse or force, `bAllowLightMovement` false, switch 0. On it, `FindFloor` reuses the last walking floor until the capsule moved `FloorCacheDistance` (25 uu, 2D) or its height changed (> 0.1 uu), or the engine forces a check (task-018-03).
- overlaps: with the switch on at BeginPlay, `ARREnemy` turns off overlap events on the root capsule (no listener: shots overlap `HitCapsule`, which keeps them; pickups, explosions and weapons query).
- light parts: `bLightAttachedParts` (true): at BeginPlay an inherited `CharacterMesh0` with no skeletal mesh and no children stops ticking, colliding and rendering and goes fully absolute; other non-capsule primitives without collision stop generating overlap events.
- far movement: every `MovementLODCheckInterval` (0.25 s, random phase) a walking robot farther than `FarMovementDistance` (2000 uu, 2D, 0 = off) from the miner and not rendered within `OffScreenGraceSeconds` (0.2 s) runs its movement and `URREnemyMovementComponent` at `FarMovementTickInterval` (0.1 s); only with a game viewport (never in tests). The actor tick (attacks, proximity) stays every frame.
- entry_points: `URRRobotMovementComponent::IsLightMovementEnabled`, `GetLightMoveCount`, `GetEngineMoveCount`, `GetCachedFloorHits`; `ARREnemy::SetFarMovement`, `IsFarMovementActive`.
- data: the tunables above (`EditDefaultsOnly`); the console variable.
- depends_on: [[sys-flow-field]].
- implements: open (performance work, no gdd node).
- gotchas: the light step trusts a flat floor: a floor height change under a walking robot is not seen (the arena has none). Measured effects in [[perf-risks]] and [[stress-grid-cells]]. The overlap setting is read at BeginPlay: set the switch before robots spawn.

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemies-base]] — the base that owns these components.
- [[stress-grid-cells]] — before/after game-thread numbers of the light step.
