---
id: perf-risks
description: "Cost centers at 150–200 enemies with fallbacks and hours, plus ranked optimization leads with expected gains from the S0 baseline."
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

## Leads from the S0 baseline (task-013-01, ranked, not measured one by one)
Baseline: game-thread bound, ≈ 0.02 ms per chaser up to 400, ceiling ≈ 500 ([[stress-test-m1]]). Gains are estimates at 400 chasers; confirm each with an Insights trace (`-trace=cpu,frame`, 0.5 h) before coding. No optimization is coded without the owner.
1. Enemy-enemy capsule collision in the crowd (sweeps and depenetration of CharacterMovement against neighbours): enemies ignore the Pawn channel, separation left to RVO. Expected −3 to −5 ms game. 1 h.
2. CharacterMovement cost per robot: lighter settings (no physics interaction, no floor re-check when flat, tick interval for robots far from the miner). Expected −1 to −3 ms. 1–2 h. Full fallback: flat-floor pawn movement (row above), larger gain, 2–3 h.
3. RVO avoidance at 400 agents in one dense cluster: lower `AvoidanceConsiderationRadius` or update rate. Expected −0.5 to −2 ms. 0.5 h.
4. Per-robot tick work (state machine, mesh blink, flow-field lookup each frame): tick at 30 Hz or batch in a manager. Expected −0.5 to −1 ms. 1 h.
5. Rendering floor: draw ≈ 8 ms and GPU ≈ 7 ms already at 50 robots (Lumen, virtual shadows, ray tracing). Not limiting on this PC; −2 to −4 ms GPU if a weaker target appears. 0.5 h.

## Relations
- [[performance-index]] — parent index.
- [[sys-enemy-ai]] — pathfinding and line of sight decisions.
- [[stress-test-m1]] — the baseline table these leads come from.
