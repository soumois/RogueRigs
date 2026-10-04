---
id: stress-final-m1
description: "M1 final stress tables (task-013-02): S0 peace, S2 continuous fire, S7 mass kamikaze and barrel burst; avg, p99, max ms; baseline comparison; 100-cap verdict."
parent: performance-index
kind: doc
---
# M1 stress test: final run (2026-10-03)

Same machine, binaries and method as the baseline ([[stress-test-m1]]); after task-016 (rifle + shotgun, packs, alert, console, shuttle). Tool: [[sys-stress]] modes. Milliseconds avg / p99 / max; game = game thread; draw and GPU avg / p99.

## S0: chasers, miner in peace (`-Mode peace`)
| Chasers | Frame | Game | Draw | GPU | Baseline frame |
|---|---|---|---|---|---|
| 50 | 8.59 / 10.66 / 12.1 | 5.47 / 7.88 / 9.0 | 8.58 / 10.60 | 7.29 / 8.85 | 8.06 / 9.59 |
| 100 | 9.07 / 11.12 / 12.7 | 7.47 / 10.19 / 14.8 | 9.03 / 11.04 | 7.59 / 9.20 | 8.64 / 10.72 |
| 200 | 11.15 / 14.86 / 46.8 | 11.15 / 15.08 / 47.2 | 8.31 / 10.61 | 8.01 / 9.76 | 9.51 / 12.82 |
| 300 | 11.57 / 14.44 / 17.3 | 11.57 / 14.58 / 17.6 | 7.40 / 9.38 | 8.31 / 10.02 | 9.78 / 12.53 |
| 400 | 13.57 / 17.48 / 48.6 | 13.57 / 17.57 / 49.7 | 7.30 / 9.15 | 7.77 / 9.64 | 11.27 / 13.75 |
| 500 | 14.99 / 18.43 / 19.5 | 14.98 / 18.41 / 21.0 | 7.05 / 8.81 | 7.54 / 9.16 | 13.5–16.5 avg |
| 600 | 16.82 / 21.52 / 23.7 | 16.82 / 21.87 / 24.0 | 7.01 / 8.73 | 7.34 / 8.82 | 16.66 / 20.79 |

- +0.4 to +2.3 ms frame at every count; +1.4 ms game already at 50 (fixed cost of the new systems), at the edge of the ±2–3 ms run spread. Ceiling unchanged: average crosses 16.6 ms at ≈ 600, p99 under 25 ms through 600.
- The 47–50 ms single frames fall at t ≈ 61 s and 122 s of the run (steps 200 and 400, also in S2): periodic, very likely garbage collection (default 61 s interval). Unconfirmed: needs Insights.

## S2: chasers under continuous fire, both weapons (`-Mode fire`)
| Chasers | Frame | Game | Draw | GPU |
|---|---|---|---|---|
| 50 | 9.13 / 11.47 / 18.8 | 6.22 / 9.42 / 12.0 | 9.11 / 11.38 | 7.58 / 9.36 |
| 100 | 9.72 / 11.78 / 17.0 | 7.80 / 10.84 / 16.4 | 9.68 / 11.59 | 8.52 / 10.19 |
| 200 | 11.20 / 14.81 / 45.0 | 11.19 / 14.95 / 45.6 | 8.98 / 11.52 | 8.56 / 10.22 |
| 300 | 13.68 / 17.47 / 20.8 | 13.67 / 17.44 / 20.6 | 9.04 / 11.78 | 8.17 / 9.96 |
| 400 | 15.76 / 20.11 / 46.4 | 15.75 / 20.42 / 47.0 | 9.13 / 12.23 | 7.96 / 9.64 |

- Robots at 10⁶ health, so every hit runs the damage path without deaths. Fire costs +0.5 ms at 50–200 and +2.1 ms at 300–400 (game), +1.5–3 ms draw. 400 passes; S2 ceiling ≈ 450–500.

## S7: N kamikazes killed in one frame + all 16 barrels destroyed (`-Mode burst -Class Kamikaze`, MinCells 8, 25 s window)
| Kamikazes | Explosions | Frame | Game | Draw p99 |
|---|---|---|---|---|
| 50 | 66 | 8.49 / 10.87 / 18.3 | 3.86 / 6.49 / 15.9 | 10.85 |
| 100 | 116 | 8.73 / 14.16 / 29.5 | 4.04 / 6.74 / 28.2 | 14.17 |
| 200 | 216 | 8.98 / 23.38 / 49.6 | 4.40 / 9.03 / 47.5 | 23.22 |

- The queue resolves one explosion per 0.08 s: 216 explosions take ≈ 17 s, so the average stays flat. The max (game) grows ≈ 0.23 ms per robot dying in the same frame: the burst frame. The p99 grows with the chain length on the render thread (draw p99 10.9 → 23.2 ms): FX and terrain instance updates, not game logic.

## Verdict for the M1 cap (100 alive)
Safe with margin: at 100 robots avg 9–10 ms (≈ 40 % under 16.6) and p99 11–12 ms (≈ 50 % under 25) in S0 and S2; only isolated frames exceed 25 ms (GC-like hitch; 100 deaths in one frame, 29.5 ms, not reachable in play). Leads: [[perf-risks]].

## Relations
- [[performance-index]] — parent index.
- [[stress-test-m1]] — protocol, thresholds and the baseline these numbers compare with.
- [[perf-risks]] — ranked optimization leads drawn from these tables.
- [[sys-stress]] — the run modes and launcher options used.
- [[stress-opt-m1]] — the same scenarios rerun after task-018 optimization.
