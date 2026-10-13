---
id: perf-projectile-pool-evaluation
description: "Should projectiles be pooled: measured spawns per second, ms per spawn and destroy, burst frames, pool saving estimate, migration risks, recommendation (no) and revisit threshold."
parent: performance-index
kind: doc
---
# Projectile pool evaluation (task-111-02, 2026-10-12)

Editor `-game`, HEAD 79fbc4a, owner normal profile, nothing closed, no editor running. Game 250 = `rr.Stress.Run 250 30 60 1 game`, `rr.Pacing.RunLookahead 0`; one Insights run (`cpu,frame,gpu,bookmark,region`, `-statnamedevents`, trace outside the repo) and one untraced run. S2 = `fire 200 5 20`, traced. Method of [[perf-trace-real-game]]. Spawn cost = `SpawnActor` plus the `FinishSpawning` siblings that follow it under `AutoFire` (Blueprint component instancing, registration, BeginPlay), without the damage spec or ammo GE.

## Measured (traced, ms inflated by named events, about ×1.2–1.5)
| Source | Spawns / s | ms per spawn (`SpawnActor` only) | ms per destroy | ms per frame |
|---|---|---|---|---|
| Rifle `BP_Projectile_C`, game | 3.40 | 1.13 (0.41) | 0.19 | 0.074 |
| Shotgun `BP_ShotgunPellet_C`, game | 3.49 (7 per shot) | 0.54 (0.24) | 0.11 | 0.037 |
| Turret, robot shooters | 0 in this run | not measured | | |
| Rifle, S2 200 | 3.35 | 0.98 (0.42) | 0.15 | 0.040 |

- Game 250: 0.11 ms per frame for every projectile spawn and destroy, 0.67 % of the game thread (16.3 ms traced). S2: 0.04 ms, 0.38 %.
- Half the rifle cost is its Blueprint `ShotMesh` (component instancing 0.15, mesh registration 0.22). Physics body creation of the sphere is 0.07.
- Burst frames: a shotgun shot puts 7 pellets in one frame, 30 per minute, about 3.8 ms (max 7.6). Of 39 traced frames over 25 ms, two are pushed over by pellets (25.9 ms with 7.6 ms of pellets, 26 ms with 4.0). The untraced run: 8 spikes, all fill, GC or render-bound, none at a shotgun shot (game 14.53 / 21.52 / 52.15).

## What a pool would save (estimate)
Assumption: reuse removes construction, component instancing, physics creation and BeginPlay, but keeps transform, collision toggle, visibility and movement restart, about 25–35 % of a spawn (robot reuse frames still cost 25–29 ms for 10–15 robots, [[perf-stress-grid-cells]]; XP burst 204 → 91 ms). Saving: ≈ 0.07 ms per frame traced (≈ 0.05 untraced), a pellet frame 3.8 → ≈ 1.2 ms; destroy replaced by a similar release cost, GC gain negligible at 7 objects per second.

## Migration cost and risks (≈ 4–6 h with tests)
- `OnPoolAcquired` must reset `HitActors`, hit count, `MaxHits`, knockback, carried spec, `IgnoreActorWhenMoving` (owner differs: weapon, turret, robot), lifespan (pool clears timers; `InitialLifeSpan` runs once), and restart `ProjectileMovement` (`StopSimulating` clears `UpdatedComponent`).
- Idle projectiles must not collide or move; a released one hit in the same frame must not apply damage twice.
- Shots must exist the frame they fire: `AcquireImmediate` or MaxFrames 1, so creation is not spread.
- Trails and explosive projectiles: none today ([[wishlist]]); each would add reset work.

## Recommendation
Do not migrate. Cheaper lead if wanted: make the rifle mesh native or drop it (≈ −0.4 ms per rifle shot). Revisit when a frame spawns 15 or more projectiles (multishot, 4-direction fire, many turrets or shooters) or sustained spawns exceed 30 per second (≈ 0.5 ms per frame).

## Relations
- [[performance-index]] — parent index.
- [[sys-projectiles]] — projectile classes and spawn path measured here.
- [[sys-actor-pool]] — the pool a migration would use.
- [[perf-trace-real-game]] — trace method and the robot and XP spawn costs used as reference.
- [[perf-stress-grid-cells]] — robot and XP pool results behind the saving estimate.
- [[task-111-02-pool-prewarm]] — the task that produced this note.
