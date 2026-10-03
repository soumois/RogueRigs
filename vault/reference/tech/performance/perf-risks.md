---
id: perf-risks
description: "Cost centers at 150–200 enemies with fallbacks and hours, plus optimization leads ranked by expected gain from the final S0, S2, S7 run."
parent: performance-index
kind: doc
---
# Performance risks at 150–200 enemies

Measure in S0 ([[stress-test-m1]]) before changing any architecture.

- 2026-10-02 first read (task-009-07, headless, no rendering; not the S0 result): `RogueRigs.Integration.Perf50Chasers`, 50 `BP_Chaser` (CharacterMovement + RVO) chasing an unarmed `BP_Miner` on `arena_m1` for 600 frames at 1/60 s, editor `-nullrhi`, Development: world tick (game thread, rendering excluded) avg 2.44–2.49 ms, median 2.36–2.42 ms, p95 3.22–3.26 ms, max 4.87–4.97 ms per frame (two runs); flow-field BFS recompute 0.32–0.36 ms (3 forced by destroyed rocks). Only 6 of 50 reached the miner in 10 s (crowding). Development machine, not a target machine.

| Cost center | Finding in the repo or GDD | Measure or fallback | Hours |
|---|---|---|---|
| Enemy movement | `ATwinStickNPC` is an `ACharacter`: one CharacterMovement per enemy, capsule, enemy-enemy collision | S0 with 200 enemies. Fallback: pawn with simple flat-floor movement, hand-made separation | 0.5 measure; 2–3 fallback |
| Pathfinding | `RuntimeGeneration=Dynamic`, 1000 uu tiles: each destroyed block rebuilds tiles, then 200 agents re-path | Flow field: BFS from the miner on the grid, no navmesh | 2 |
| Animation | 150+ animated skeletal meshes (rigid parts on bones) | Animation Budget Allocator plugin, or lower animation rate at distance | 1 |
| Rendering | `DefaultEngine.ini` enables Lumen GI and reflections, virtual shadows, hardware ray tracing, Substrate | Test without Lumen and ray tracing for flat low-poly with one main light; 5.8 also offers Lumen Lite | 0.5 |
| Line of sight | Shooters and turrets test line of sight; blocks are the only obstacles | Grid cell walk instead of physics traces, throttled | 1 |
| Explosion chains | Barrel → kamikaze → barrel, synchronous in damage callbacks: recursion, double hits | Queue on next tick, "already exploded" flag ([[sys-explosions]]) | 0.5 |
| Projectiles | Template projectile spawned and destroyed each shot, `bForceSubStepping = true` | Projectile pool; sub-stepping off unless needed | 1 |
| Target machine | This PC, 1920×1080 windowed (owner, 2026-10-03; [[stress-test-m1]]) | Baseline measured: ceiling ≈ 500 chasers | — |

## Leads after the final run (task-013-02, ranked by expected gain)
From [[stress-final-m1]]: game-thread bound, ≈ 0.02–0.025 ms per chaser; M1 cap (100) safe. Gains are estimates; confirm each with an Insights trace (`-trace=cpu,frame`, 0.5 h) before coding. No optimization is coded without the owner.
1. Enemy-enemy capsule collision in the crowd (CharacterMovement sweeps and depenetration): enemies ignore the Pawn channel, separation left to RVO. −3 to −5 ms game at 400. 1 h.
2. CharacterMovement cost per robot: no physics interaction, no floor re-check on flat floor, tick interval far from the miner. −1 to −3 ms at 400. 1–2 h. Full fallback: flat-floor pawn movement (table row), larger gain, 2–3 h.
3. Periodic ≈ 45–50 ms hitches every 61 s from 200 robots (S0 and S2), likely garbage collection: confirm, then incremental GC or fewer transient UObjects (pools, lead 5). Removes the only frames over 25 ms in steady play. 1 h after the trace; check in a packaged build first.
4. Render thread in long explosion chains: draw p99 10.9 → 23.2 ms from 66 to 216 queued explosions (FX actor per explosion, terrain instance removal and add). Pool or cap explosion FX, batch instance updates per frame. −5 to −10 ms draw p99 in chains. 1–2 h.
5. Fire cost: +2 ms game and +1.5–3 ms draw at 300–400 robots under fire (projectile spawn and destroy, damage text actors, knockback). Projectile and damage-text pools. −1 to −2 ms. 1–2 h.
6. Burst death frame: ≈ 0.23 ms per robot dying in the same frame (death, XP pickup spawn, kamikaze explosion request); 100 at once = 29.5 ms. Spread death work over frames or pool pickups. Matters only if an upgrade can clear a crowd at once. 1 h.
7. Fixed overhead +1.4 ms game at 50 robots since the baseline (run flow, HUD, second weapon): find it in the trace. −0.5 to −1 ms. 0.5 h.
8. RVO at 400 agents in one cluster: lower `AvoidanceConsiderationRadius` or update rate. −0.5 to −2 ms. 0.5 h.
9. Rendering floor: draw ≈ 8 ms and GPU ≈ 7 ms at 50 robots (Lumen, virtual shadows, ray tracing). Not limiting here; −2 to −4 ms GPU on a weaker target. 0.5 h.

## Relations
- [[performance-index]] — parent index.
- [[sys-enemy-ai]] — pathfinding and line of sight decisions.
- [[stress-test-m1]] — protocol and baseline.
- [[stress-final-m1]] — the final tables these leads come from.
