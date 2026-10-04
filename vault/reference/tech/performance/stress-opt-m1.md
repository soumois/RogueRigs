---
id: stress-opt-m1
description: "Stress tables after task-018 optimization: S0, S2, S7 in editor -game vs stress-final-m1, new ceilings, staged Development build S0 and S2 ceilings."
parent: performance-index
kind: doc
---
# M1 stress test after optimization (2026-10-04, task-018-05)

Same PC, method and `-game` editor binary as [[stress-final-m1]] (settle 5 s, measure 20 s, step-by-step top-up, [[sys-stress]]); code at fe827fd (task-018-02/03, [[sys-enemies]]). Milliseconds avg / p99 / max. Steps last 25 s, so the editor-only GC frame ([[perf-trace-m1]]) lands in every 2nd or 3rd step: max frames of 35–47 ms are GC, not game cost.

## S0 peace, editor -game
| Chasers | Frame | Game | Draw / GPU avg | Final-m1 frame / game avg |
|---|---|---|---|---|
| 50 | 8.06 / 9.38 / 58.0 | 3.86 / 5.61 / 12.6 | 8.06 / 7.12 | 8.59 / 5.47 |
| 100 | 8.51 / 9.69 / 12.6 | 4.52 / 6.74 / 8.0 | 8.50 / 7.58 | 9.07 / 7.47 |
| 200 | 9.33 / 10.46 / 36.8 | 5.95 / 8.41 / 36.9 | 9.31 / 8.36 | 11.15 / 11.15 |
| 300 | 9.82 / 11.01 / 13.2 | 7.52 / 10.04 / 12.8 | 9.80 / 8.83 | 11.57 / 11.57 |
| 400 | 9.83 / 11.04 / 41.3 | 7.94 / 10.82 / 40.9 | 9.76 / 8.83 | 13.57 / 13.57 |
| 500 | 9.88 / 11.63 / 14.7 | 8.47 / 11.60 / 16.1 | 9.75 / 8.87 | 14.99 / 14.98 |
| 600 | 10.14 / 12.93 / 14.8 | 9.59 / 12.97 / 15.3 | 8.83 / 8.97 | 16.82 / 16.82 |
| 800 | 10.90 / 14.03 / 16.2 | 10.90 / 14.09 / 16.2 | 6.21 / 9.01 | — |
| 1000 | 12.37 / 16.11 / 17.7 | 12.37 / 16.22 / 17.9 | 6.35 / 8.62 | — |
| 1200 | 14.39 / 21.89 / 44.1 | 14.39 / 22.18 / 42.8 | 6.41 / 8.20 | — |
| 1400 | 15.36 / 22.76 / 42.4 | 15.36 / 22.79 / 41.9 | 6.30 / 7.98 | — |
| 1600 | 16.76 / 26.85 / 45.9 | 16.76 / 26.87 / 46.3 | 6.32 / 7.78 | — |

- 50–1000 from run A, 1200–1400 run B (chain to 1400), 1600 run C (chain to 1800); runs agree within ±0.9 ms avg. Game thread −1.6 ms at 50 to −7.2 ms at 600. Below ≈ 600 the frame equals the render thread (draw), not the game thread.
- Ceiling: ≈ 600 → ≈ 1500 (avg and p99 both cross near 1500–1600). A fresh mass spawn of 1000 measures 20.4 ms (crowd not settled); top-up numbers are the reference.

## S2 continuous fire, editor -game
| Chasers | Frame | Game | Draw p99 | Final-m1 frame / game avg |
|---|---|---|---|---|
| 100 | 9.32 / 10.86 / 12.7 | 5.20 / 8.07 / 9.6 | 10.84 | 9.72 / 7.80 |
| 200 | 9.84 / 11.27 / 37.5 | 7.79 / 10.86 / 37.4 | 11.01 | 11.20 / 11.19 |
| 300 | 10.40 / 14.26 / 17.9 | 10.10 / 13.98 / 17.2 | 13.06 | 13.68 / 13.67 |
| 400 | 12.32 / 17.12 / 42.3 | 12.30 / 17.21 / 43.3 | 14.26 | 15.76 / 15.75 |
| 500 | 13.21 / 17.01 / 21.2 | 13.21 / 17.24 / 19.7 | 13.75 | — |
| 600 | 14.46 / 19.38 / 23.1 | 14.46 / 19.36 / 22.4 | 14.19 | — |
| 800 | 17.14 / 23.16 / 46.8 | 17.13 / 22.86 / 47.8 | 14.38 | — |

- Ceiling ≈ 450–500 → ≈ 700. Fire now costs +4.6 ms game at 600 over S0: the largest remaining gameplay cost.

## S7 kamikaze burst + 16 barrels, editor -game
| Kamikazes | Frame | Game | Draw p99 | Final-m1 game max / draw p99 |
|---|---|---|---|---|
| 50 | 8.17 / 9.83 / 15.2 | 2.84 / 5.04 / 13.8 | 9.78 | 15.9 / 10.85 |
| 100 | 8.30 / 12.49 / 25.9 | 2.99 / 5.58 / 24.1 | 12.58 | 28.2 / 14.17 |
| 200 | 8.49 / 18.13 / 45.1 | 3.32 / 6.80 / 41.9 | 18.11 | 47.5 / 23.22 |

- 50: rerun with GC logging (first run max 34.6 ms, GC-like). Burst frame ≈ 0.20 ms per robot (was 0.23); chain draw p99 −1 to −5 ms.

## Staged Development build (`-Exe`, same procedure)
| Chasers | S0 frame | S0 game avg | S2 frame | S2 game avg |
|---|---|---|---|---|
| 100 | 9.05 / 10.08 / 10.5 | 3.90 | 9.50 / 10.71 / 11.6 | 4.17 |
| 400 | 10.06 / 11.17 / 11.6 | 7.16 | 10.86 / 13.63 / 18.4 | 9.73 |
| 600 | 10.28 / 12.03 / 15.5 | 8.58 | 12.41 / 16.21 / 18.8 | 12.32 |
| 800 | 10.58 / 13.33 / 14.3 | 10.31 | 14.41 / 18.91 / 20.9 | 14.39 |
| 1000 | 11.78 / 14.98 / 19.2 | 11.75 | 17.32 / 22.77 / 30.3 | 17.29 |
| 1400 | 14.30 / 21.28 / 22.5 | 14.26 | — | — |
| 1600 | 14.61 / 24.92 / 27.5 | 14.57 | — | — |
| 1800 | 16.00 / 36.49 / 38.7 | 15.97 | — | — |

- No frame over 31 ms (no editor GC hitch). Staged ceiling: S0 ≈ 1600 (p99 limits before avg), S2 ≈ 900. Remaining leads: [[perf-leads]].

## Relations
- [[performance-index]] — parent index.
- [[stress-final-m1]] — the pre-optimization tables compared here.
- [[perf-leads]] — remaining leads re-ranked from these tables.
- [[sys-stress]] — run modes, launcher and `-Exe` option used.
- [[perf-trace-m1]] — trace explaining the editor-only GC frames.
