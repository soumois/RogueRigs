---
id: perf-risks
description: "Cost centers at 150–200 enemies with fallbacks and hours, plus optimization leads ranked by expected gain from the final run and the Insights trace."
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

## Leads (task-013-02, re-ranked by the trace of task-018-01)
From [[stress-final-m1]], measured per timer in [[perf-trace-m1]]: game-thread bound; CharacterMovement is 53 % of it at 400. Gains are estimates; no optimization is coded without the owner.
1. CharacterMovement cost per robot (5.9 ms S0, 7.0 ms S2 at 400): floor check 1.9 ms (skip or throttle on the flat floor), move sweeps 2.2 ms, scoped movement update 1.6 ms (transforms of attached mesh parts), tick interval far from the miner. Done in task-018-03 ([[sys-enemies]], light movement): empty character mesh stripped, floor cache (25 uu), far movement (off-screen beyond 2000 uu: 0.1 s tick); physics interaction off and `bAlwaysCheckFloor` off measured no gain, reverted. Game thread avg, S0 100/200/400 5.38/7.91/11.24 → 4.39–4.49/6.09–6.40/8.88–9.28 ms; S2 vs the 018-02 after runs 6.0/9.4/14.65 → 5.35–5.62/7.94–8.10/12.03–12.11 ms. Remaining: move sweeps (needed for walls and the miner). Full fallback: flat-floor pawn movement (table row), 2–3 h.
2. Periodic 30 ms GC every 61.1 s: editor-process only (Python plugin hook 9–11 ms, reachability 18 ms over editor objects). Closed by task-018-04: in the staged Development build the GC frame is 10.8–16.4 ms (GC 3.6–5.3 ms, [[perf-trace-m1]]); no GC setting changed. Editor `-game` max frames over 25 ms at 61 s intervals are expected and not a game cost.
3. Render thread in long explosion chains: draw p99 10.9 → 23.2 ms from 66 to 216 queued explosions (FX actor per explosion, terrain instance updates). Pool or cap FX, batch instance updates. −5 to −10 ms draw p99 in chains. 1–2 h.
4. Enemy-enemy capsule collision (was 1): done in task-018-02 for packing (robot-robot sweeps skipped by mask filter, lighter RVO, [[sys-enemies]]). Same procedure as [[stress-final-m1]], same day, game thread avg before → after: S0 100 5.46 → 5.55, 200 8.28 → 7.78, 400 11.31 → 10.96 ms; S2 100 6.28 → 5.98/6.09, 200 9.30 → 9.31/9.53, 400 13.34 → 14.61/14.71 ms (two after runs; draw +1.6 ms at 400). S0 gain ≈ 0.3–0.5 ms at 200–400, inside the run spread; S2 at 400 costs ≈ +1.3 ms, likely because the denser crowd puts more robots in weapon reach (more hits, damage numbers, knockback), unconfirmed.
5. Fire cost at 400 (+2.1 ms game): +1.1 CharacterMovement under knockback, +0.5 end-of-frame render updates, +0.45 damage text, projectiles, auto-fire. Projectile and damage-text pools. −0.5 to −1 ms. 1–2 h.
6. Burst death frame: ≈ 0.23 ms per robot dying in the same frame; 100 at once = 29.5 ms. Spread death work or pool pickups, if an upgrade can clear a crowd. 1 h.
7. Fixed game-thread overhead: viewport draw 0.9–1.4 ms, Slate 0.35–0.5 ms, skeletal mesh ticks 0.6–0.7 ms; HUD 0.03, flow-field steering 0.26, GAS negligible. −0.5 ms. 0.5 h.
8. Rendering floor: draw ≈ 8 ms and GPU ≈ 7 ms at 50 robots (Lumen, virtual shadows, ray tracing). −2 to −4 ms GPU on a weaker target. 0.5 h.
9. RVO (was 8): 0.33 ms at 400. Drop unless the crowd grows.

## Relations
- [[performance-index]] — parent index.
- [[sys-enemy-ai]] — pathfinding and line of sight decisions.
- [[stress-test-m1]] — protocol and baseline.
- [[stress-final-m1]] — the final tables these leads come from.
- [[perf-trace-m1]] — per-timer trace that re-ranked the leads.
