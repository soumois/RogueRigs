---
id: perf-risks
description: "Likely cost centers at 150–200 enemies (movement, pathfinding, animation, rendering, line of sight, chains, projectiles) with fallbacks and hours."
parent: performance-index
kind: doc
---
# Performance risks at 150–200 enemies

Measure in S0 ([[stress-test-m1]]) before changing any architecture.

| Cost center | Finding in the repo or GDD | Measure or fallback | Hours |
|---|---|---|---|
| Enemy movement | `ATwinStickNPC` is an `ACharacter`: one CharacterMovement per enemy, capsule, enemy-enemy collision | S0 with 200 enemies. Fallback: pawn with simple flat-floor movement, hand-made separation | 0.5 measure; 2–3 fallback |
| Pathfinding | `RuntimeGeneration=Dynamic`, 1000 uu tiles: each destroyed block rebuilds tiles, then 200 agents re-path | Flow field: BFS from the miner on the grid, no navmesh | 2 |
| Animation | 150+ animated skeletal meshes (rigid parts on bones) | Animation Budget Allocator plugin, or lower animation rate at distance | 1 |
| Rendering | `DefaultEngine.ini` enables Lumen GI and reflections, virtual shadows, hardware ray tracing, Substrate | Test without Lumen and ray tracing for flat low-poly with one main light; 5.8 also offers Lumen Lite | 0.5 |
| Line of sight | Shooters and turrets test line of sight; blocks are the only obstacles | Grid cell walk instead of physics traces, throttled | 1 |
| Explosion chains | Barrel → kamikaze → barrel, synchronous in damage callbacks: recursion, double hits | Queue on next tick, "already exploded" flag ([[sys-explosions]]) | 0.5 |
| Projectiles | Template projectile spawned and destroyed each shot, `bForceSubStepping = true` | Projectile pool; sub-stepping off unless needed | 1 |
| Target machine | Not defined | Decide before S0: thresholds mean nothing without it | — |

## Relations
- [[performance-index]] — parent index.
- [[sys-enemy-ai]] — pathfinding and line of sight decisions.
