---
id: perf-stress-chaser-meshy
description: "Meshy chaser and death effects stress (task-181-01): S0, S2, S7 chaser burst, game 100-300; puff and debris plays and skips; M1 comparison; verdict."
parent: performance-index
kind: doc
---
# Stress: Meshy chaser and death effects (2026-10-14, task-181-01)

Editor `-game` 1920 × 1080 through `Scripts/Run-StressTest.ps1` ([[sys-stress]]), HEAD c719274 (puff / debris counters, [[sys-stress-diagnostics]]). Owner everyday profile (Brave, Discord, Epic launcher, Zoom, NVIDIA Share, Parsec…), no game, no editor running. Not paced at 60 Hz (1800–3100 frames per 20–25 s). S0 / S2: 5 s settle, 20 s measure; S7: one launch per count, MinCells 8, 25 s; game: one launch per cap, settle 30, measure 60, `rr.Pacing.RunLookahead 0`. CSV `Saved/Stress/Stress_20261014_2214..2225*.csv`, logs `task181_*.log` (not committed). ms avg / p99 / max; draw, GPU avg / p99; M1 = [[perf-stress-final-m1]].

## S0 peace and S2 fire (chasers)
| N | S0 frame | S0 game | S0 draw | S0 GPU | M1 S0 frame | S2 frame | S2 game | S2 draw / GPU avg |
|---|---|---|---|---|---|---|---|---|
| 50 | 7.99 / 9.83 / 13.2 | 3.71 / 5.13 | 7.98 / 9.72 | 7.05 / 7.32 | 8.59 / 10.66 | 8.32 / 9.97 / 11.4 | 3.83 / 5.36 | 8.31 / 7.33 |
| 100 | 8.30 / 10.10 / 11.2 | 4.12 / 5.65 | 8.29 / 9.97 | 7.34 / 7.62 | 9.07 / 11.12 | 8.70 / 10.68 / 12.5 | 4.28 / 6.12 | 8.69 / 7.70 |
| 200 | 8.98 / 10.80 / 39.0 | 4.81 / 6.80 | 8.96 / 10.74 | 8.01 / 8.36 | 11.15 / 14.86 | 9.48 / 10.95 / 40.9 | 4.98 / 6.97 | 9.45 / 8.51 |
| 300 | 9.86 / 11.63 / 26.9 | 5.62 / 8.14 | 9.84 / 11.58 | 8.87 / 9.15 | 11.57 / 14.44 | 9.99 / 11.82 / 29.1 | 5.83 / 8.10 | 9.97 / 9.00 |
| 400 | 10.44 / 12.97 / 44.9 | 6.87 / 9.45 | 10.41 / 12.97 | 9.33 / 9.72 | 13.57 / 17.48 | 10.55 / 13.42 / 43.8 | 6.87 / 9.31 | 10.52 / 9.45 |
| 600 | 11.13 / 14.18 / 15.3 | 8.30 / 11.01 | 11.10 / 14.13 | 10.15 / 10.99 | 16.82 / 21.52 | — | — | — |

- Frame below M1 at every count (−0.6 to −5.7 ms), game thread 32–51 % lower (optimizations since M1). Now render-thread bound (draw ≈ frame): draw and GPU grow ≈ 5.6 µs per robot (+3.1 ms from 50 to 600), flat in M1 (draw 7.0, GPU 7.3 at 600). S2 adds ≤ 0.5 ms.
- Trace S0 600 (`-Trace cpu,frame,gpu,bookmark,region -statnamedevents`, one launch, frame 10.16 / 13.05): render thread visibility 2.1 ms, Lumen scene update 1.4 ms with ≈ 100 `MeshCardCapture` per frame (1.1 ms), primitive updates 0.86 ms per frame. Moving robot parts re-capturing Lumen cards is the likely per-robot cost; not proven (no A/B).

## S7 burst: N chasers killed in one frame + 16 barrels (M1: kamikazes)
| N | Expl. | Frame | Game | Draw p99 / GPU avg | Puff plays / skips | Debris plays / skips | M1 frame (game max) |
|---|---|---|---|---|---|---|---|
| 50 | 16 | 8.27 / 10.71 / 28.4 | 3.25 / 7.15 / 28.0 | 10.64 / 7.30 | 40 / 10 | 40 / 10 | 8.49 / 10.87 / 18.3 (15.9) |
| 100 | 16 | 8.07 / 11.23 / 27.1 | 3.31 / 7.55 / 16.0 | 11.08 / 7.12 | 40 / 60 | 40 / 60 | 8.73 / 14.16 / 29.5 (28.2) |
| 200 | 16 | 8.19 / 11.21 / 27.9 | 3.41 / 8.74 / 24.3 | 11.21 / 7.21 | 40 / 160 | 40 / 160 | 8.98 / 23.38 / 49.6 (47.5) |

- Chasers do not explode: 16 barrel explosions instead of 66–216, so the M1 chain p99 is gone. Death frame 27.0 (50) and 27.1 ms (100, game 16.0, draw 14.6); 200 deaths stay under 25 ms frame (game max 24.3). Barrel frames 25.5–28.4 ms at t 1.5–1.6 s. The cap holds: 40 puffs and 40 debris per frame at most, every other death of the frame skipped.

## Game mode (caps, one launch each)
| Cap | Alive avg (max) | Frame | Game | Draw / GPU avg | Spikes (gc / pause) | Puff plays / skips | Debris plays / skips |
|---|---|---|---|---|---|---|---|
| 100 | 73.8 (100) | 9.73 / 12.33 / 43.7 | 6.47 / 10.23 / 43.8 | 9.70 / 8.37 | 4 (1 / 2) | 277 / 0 | 278 / 2 |
| 200 | 165.6 (200) | 10.75 / 15.43 / 44.3 | 9.51 / 15.26 / 45.2 | 10.24 / 8.74 | 4 (1 / 2) | 633 / 36 | 465 / 204 |
| 300 | 232.3 (300) | 12.28 / 18.18 / 46.6 | 11.96 / 18.05 / 47.3 | 9.79 / 8.75 | 1 (1 / 0) | 975 / 11 | 531 / 441 |

- [[perf-stress-grid-cells]] game mode: 250 cap (alive 193–205) game 12.35–13.94 / 20.25–20.74 (task-105-01), 150 cap 10.4–10.8 / 19.4–20.1 (task-067-01). Now 300 cap (alive 232) 11.96 / 18.05: no regression within the ≈ 2 ms PC spread.
- Debris skipped for 1 %, 30 %, 45 % of deaths at 100 / 200 / 300: 40 components of 3.4 s serve ≈ 12 deaths per second; deaths come in bursts. Puff (0.3 s) skips only on mass kills.
- GC frames 39–47 ms (S0 / S2 at 200 and 400, game at t ≈ 31 s) as in M1. Untagged 27–29 ms frame at S0 / S2 300, t 16 s (game 20–22 ms), cause not traced.

## Verdict
The Meshy chaser and its death effects fit the budget: averages ≤ 12.3 ms and p99 ≤ 18.2 ms everywhere, burst frames ≤ 28.4 ms (M1: 49.6). The pool caps bound the death-effect cost; their price is visual (debris missing on 30–45 % of deaths above 200 alive). Lead: robot parts in the Lumen surface cache (render thread grows with the count).

## Relations
- [[performance-index]] — parent index.
- [[perf-stress-final-m1]] — the M1 S0, S2, S7 tables compared here.
- [[perf-stress-grid-cells]] — the game-mode results compared here.
- [[sys-stress-diagnostics]] — the puff and debris counters of the step line.
- [[sys-actor-pool]] — the Niagara pool and caps measured.
- [[enemy-death]] — the death effects measured.
- [[task-181-01-perf-chaser-meshy-death]] — the task that produced this report.
