---
id: perf-risks
description: "Cost centers at 150–200 enemies with the finding in the repo, the measure or fallback, and hours; ranked leads live in perf-leads."
parent: performance-index
kind: doc
---
# Performance risks at 150–200 enemies

Measure in S0 ([[perf-stress-test-m1]]) before changing any architecture.

- 2026-10-02 first read (task-009-07, headless, no rendering; not the S0 result): `RogueRigs.Integration.Perf50Chasers`, 50 `BP_Chaser` (CharacterMovement + RVO) chasing an unarmed `BP_Miner` on `arena_m1` for 600 frames at 1/60 s, editor `-nullrhi`, Development: world tick (game thread, rendering excluded) avg 2.44–2.49 ms, median 2.36–2.42 ms, p95 3.22–3.26 ms, max 4.87–4.97 ms per frame (two runs); flow-field BFS recompute 0.32–0.36 ms (3 forced by destroyed rocks). Only 6 of 50 reached the miner in 10 s (crowding). Development machine, not a target machine.

| Cost center | Finding in the repo or GDD | Measure or fallback | Hours |
|---|---|---|---|
| Enemy movement | Robots are `ACharacter`s; CharacterMovement cost 5.56 ms per frame at ≈ 205 real-game robots (task-068-01 trace) | Done (task-071-01): light kinematic step on the flat floor behind `rr.Enemy.LightMovement`, movement 4.39 ms, game thread −2.4 to −4.4 ms at 250 ([[perf-stress-grid-cells]], [[sys-enemies-movement]]); next: the remaining swept move (child transforms, hit capsule overlaps) | done |
| Pathfinding | `RuntimeGeneration=Dynamic`, 1000 uu tiles: each destroyed block rebuilds tiles, then 200 agents re-path | Flow field: BFS from the miner on the grid, no navmesh | 2 |
| Animation | 150+ animated skeletal meshes (rigid parts on bones) | Animation Budget Allocator plugin, or lower animation rate at distance | 1 |
| Rendering | `DefaultEngine.ini` enables Lumen GI and reflections, virtual shadows, hardware ray tracing, Substrate | Test without Lumen and ray tracing for flat low-poly with one main light; 5.8 also offers Lumen Lite | 0.5 |
| Player light shadows | REMOVED in task-272-15 (owner 2026-10-21): the light is gone, so this cost is gone (measured +2.5 to 3 ms at 1080p, see the figures below); the post-process mask ([[mpp-player-light-mask]]) replaces it at the cost of one fullscreen pass. History: task-263: one movable point light on the miner is the only shadow caster; grid rock pieces and barrels cast ([[sys-grid]] `bBlocksCastShadows`), nothing else does. A moving local light renders its virtual shadow pages uncached each frame it moves | Measure with the light's shadows on and off, then with `bBlocksCastShadows` off; fallback: smaller attenuation radius or no shadow on broken pieces; radius 3000 uu since task-272-02. Measured at 1080p, ProfileGPU frame D: r6b static +0.8 ms frame vs the old 640 uu light (ShadowDepths 0.61 -> 1.26 ms); task-272-04 shadows on vs off at 3000 uu: about +2.5 to 3.0 ms (ShadowDepths +1.8, Lights +1.1), the same moving (miner moved every frame) as static | 0.5 |
| Miner light wall shadows | task-445 (owner 2026-11-05): `FloorWallLight` ([[bp-miner]]) casts shadows at radius 800 uu; casters are rock pieces inside the radius ([[lights-no-shadows]]). Moving, it renders its virtual shadow pages uncached. Not measured (owner Q17, no GPU measure); only reference: the removed PlayerLight row above, at radius 3000 uu, with more casters | Untick Cast Shadows on the component; `bBlocksCastShadows` off also removes rock and barrel sun shadows | 0.5 |
| Blob shadows | task-264-02: one translucent disc component per robot, the miner and the drone ([[sys-blob-shadow]]); 400 robots = 400 sorted translucent draws, proxy updates and child transforms (unmeasured) | S0 with `BlobShadowDiameterScale` 0 vs 2.2; fallback: [[perf-lead-blob-shadow-instancing]] | 0.5 |
| Line of sight | Shooters and turrets test line of sight; blocks are the only obstacles | Grid cell walk instead of physics traces, throttled | 1 |
| Explosion chains | Barrel → kamikaze → barrel, synchronous in damage callbacks: recursion, double hits | Queue on next tick, "already exploded" flag ([[sys-explosions]]) | 0.5 |
| Projectiles | Template projectile spawned and destroyed each shot, `bForceSubStepping = true` | Projectile pool; sub-stepping off unless needed | 1 |
| Target machine | This PC, 1920×1080 windowed (owner, 2026-10-03; [[perf-stress-test-m1]]) | Ceiling ≈ 500 chasers at baseline, ≈ 1500 after task-018 ([[perf-stress-opt-m1]]) | — |

## Leads
Ranked optimization leads, with what task-018 already did: [[perf-leads]].

## Relations
- [[performance-index]] — parent index.
- [[sys-enemy-ai]] — pathfinding and line of sight decisions.
- [[perf-stress-test-m1]] — protocol and baseline.
- [[perf-stress-final-m1]] — pre-optimization stress tables.
- [[perf-trace-m1]] — per-timer trace of the cost centers.
- [[perf-leads]] — ranked optimization leads split out of this node.
- [[perf-stress-opt-m1]] — post-optimization tables and ceiling.
- [[perf-stress-grid-cells]] — before/after numbers of the robot light movement (task-071-01).
- [[sys-enemies-movement]] — the light movement that replaced the robots' walking physics.
- [[sys-blob-shadow]] — the per-actor blob shadow row.
- [[bp-miner]] — carries the shadowed miner light.
- [[lights-no-shadows]] — which casters the miner light has.
