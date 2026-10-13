---
id: perf-robot-components-audit
description: "Robot component audit (task-093-01): components per robot class, attachment, collision and overlap flags, what needs them, and the movement cost ranking."
parent: performance-index
kind: note
---
# Robot components audit (task-093-01, 2026-10-10)

## The overlap problem, in plain terms
A robot is one actor: a root capsule that moves, a hit capsule for shots and rigid visual parts, all attached to the root. Each time the root moves, the engine recomputes the world transform of every attached component (render bounds, physics body of each capsule) and, for every component that generates overlap events, runs an overlap query at its new place. It happens inside each robot, every frame, not between assets.

## Components (C++ [[sys-enemies-base]], Blueprints [[bp-chaser]], [[bp-shield-chaser]], [[bp-shooter]], [[bp-kamikaze]], [[bp-heavy]])
| Class | Visual parts on the root (all NoCollision, overlap events off at BeginPlay) | Others |
|---|---|---|
| Chaser | BodyMesh, EyeMesh | |
| Shield chaser | BodyMesh, EyeMesh, ShieldShell (hidden after the break) | |
| Shooter | BodyMesh, EyeLeft/Right, LogoPlate, UpperArm, ElbowJoint, Cannon | MuzzlePoint (scene) → LaserBeam (C++, NoCollision, hidden except aiming) |
| Kamikaze | VisualRoot (scene, C++) → BodyMesh, CoreMesh (C++), EyeMesh | CoreLight (point light, flare) |
| Heavy | BodyMesh, StripeA–D, EyeLeft/Right, LogoPlate, ShockRing (hidden at rest) | |

Every class also has: root `CollisionCylinder` (profile Pawn, Projectile ignored, robot mask filter, overlap events off with light movement, physics volume update on), `HitCapsule` (child of the root, QueryOnly, object Pawn, overlaps only Projectile, overlap events on), `CharacterMesh0` (empty, made absolute, no tick, collision or render: skipped by moves), CharacterMovement, EnemyMovement, ASC.

## Who needs what
- Shots: `ARRProjectile` sphere overlaps the hit capsule; its own sweep and end-of-move overlap find it. The event needs overlap events on both sides, but only the projectile needs to query: projectiles move every frame.
- Death effect: a shrink of the whole actor with every mesh shown ([[sys-enemies-base]]); no part exists only for death, so none can be attached at death.
- Explosions, turrets, slam, pickups, weapons: world queries on Pawn bodies (root and hit capsule physics bodies), not overlap events.
- Visual parts: all render while alive, so they must follow the root.

## Movement cost left (game 250, traced, ≈ 200 robots, ms per frame)
Robot movement tick 3.64: scoped move end 2.17 (child transforms 1.07 incl. physics body moves 0.64; overlap updates 0.83 of which the hit capsule overlap query 0.61), swept moves 1.52.

## Removed (task-093-01)
The hit capsule no longer queries overlaps when the robot moves (passive side; [[sys-enemies-movement]]): movement 3.64 → 3.16 ms per frame, numbers in [[perf-stress-grid-cells]]. Parts already had overlap events off; none is death-only. Remaining child transforms are visible parts: merging each robot's parts into one mesh is a Blueprint/asset lead, not done.

## Relations
- [[performance-index]] — parent index.
- [[sys-enemies-movement]] — light movement and the passive hit capsule.
- [[perf-stress-grid-cells]] — before/after numbers (task-093-01 section).
- [[perf-trace-real-game]] — the capture method and the earlier ranking.
- [[task-093-01-robot-overlap-cost]] — the task that produced this audit.
