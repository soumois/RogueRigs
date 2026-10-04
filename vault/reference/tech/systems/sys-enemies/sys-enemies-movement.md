---
id: sys-enemies-movement
description: "Robot light movement as built (task-018-03): URRRobotMovementComponent cached floor, light attached parts, far-movement tick LOD and its tunables."
parent: sys-enemies
kind: doc
---
# Robot light movement

- responsibility: keep horde movement cheap without changing paths (category `Enemy|Performance`).
- main_classes: `URRRobotMovementComponent` (`RRRobotMovementComponent.h/.cpp`, CharacterMovement class set by `SetDefaultSubobjectClass` in `ARREnemy`): while walking on a walkable floor, `FindFloor` reuses the last floor until the capsule moved `FloorCacheDistance` (25 uu, 2D) or its height changed (> 0.1 uu), or the engine forces a check (teleport, landing); `GetCachedFloorHits`.
- light parts: `bLightAttachedParts` (true): at BeginPlay an inherited `CharacterMesh0` with no skeletal mesh and no children stops ticking, colliding and rendering and goes fully absolute; other non-capsule primitives without collision stop generating overlap events.
- far movement: every `MovementLODCheckInterval` (0.25 s, random phase) a walking robot farther than `FarMovementDistance` (2000 uu, 2D, 0 = off) from the miner and not rendered within `OffScreenGraceSeconds` (0.2 s) runs CharacterMovement and `URREnemyMovementComponent` at `FarMovementTickInterval` (0.1 s); back to every frame when near or on screen; only with a game viewport (never in tests). The actor tick (attacks, proximity) stays every frame.
- entry_points: `ARREnemy::SetFarMovement`, `IsFarMovementActive`.
- data: the tunables above (`EditDefaultsOnly`).
- depends_on: [[sys-flow-field]].
- implements: open (performance work, no gdd node).
- gotchas: measured effects in [[perf-risks]].

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemies-base]] — the base that owns these components.
