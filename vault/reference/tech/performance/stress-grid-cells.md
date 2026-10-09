---
id: stress-grid-cells
description: "Cell size stress (task-023-01): 200 vs 100 vs 67 uu cells, frame times, explosions, dig, flow field, spawn scan, memory, bottlenecks, fixes with hours, verdict."
parent: performance-index
kind: doc
---
# Grid cell size stress test (2026-10-05, task-023-01)

Same PC and editor `-game` procedure as [[stress-opt-m1]]. `L_ArenaM1` split at runtime by `rr.Stress.Subdivide` ([[sys-stress]]): same world, same rock. 80² / 160² / 240² cells; colliding instances 3 729 / 14 916 / 33 561. Chain: 30 barrel explosions at the same points, 100 robots. Dig: same rock area per second (8 cells of 200 uu/s, i.e. 32 or 72 small cells/s). Budget: 16.7 ms avg, 25 ms p99 ([[stress-test-m1]]). ms, avg / p99.

## Steady state (no destruction)
| Run | 200 uu | 100 uu | 67 uu |
|---|---|---|---|
| S0 100 frame | 8.44 / 10.39 | 9.06 / 10.52 | 8.99 / 10.50 |
| S0 400 frame (game, draw, GPU avg) | 10.10 / 13.49 (9.89, 8.04, 7.95) | 10.55 / 13.02 (9.08, 9.86, 9.24) | 10.47 / 12.07 (7.71, 10.44, 9.42) |
| S2 100 frame | 8.97 / 10.58 | 9.22 / 10.90 | 9.57 / 11.50 |
| S2 400 frame (game, draw, GPU avg) | 12.67 / 16.91 (12.67, 9.90, 8.40) | 12.35 / 16.61 (12.33, 10.35, 8.61) | 12.30 / 16.50 (12.19, 11.11, 9.10) |

## Destruction (bench and 100 robots)
| Measure | 200 uu | 100 uu | 67 uu |
|---|---|---|---|
| One explosion, terrain (blocks) | 1.7 ms (7) | 22.3 ms (29) | 91.6 ms (64) |
| Chain: event frame avg / max | 11.1 / 38 | 33.0 / 52 | 117 / 208 |
| Dig: frame avg / p99 (draw avg) | 9.12 / 10.67 (9.1) | 10.65 / 13.52 (9.4) | 18.59 / 23.08 (18.6) |
| One dig destruction | 0.25 ms | 0.87 ms | 2.14 ms |
| Flow-field BFS per rebuild | 0.29 ms | 1.03 ms | 2.56 ms |
| Spawn candidate scan | 0.11 ms | 0.42 ms | 1.13 ms |
| Line of sight per ray | 0.15 µs | 0.20 µs | 0.34 µs |
| Memory (used physical) / grid build | +1 MB / 33 ms | +35 MB / 135 ms | +115 MB / 309 ms |

## Cause and what-if
Insights trace (67 uu chain): 3.0 of 3.1 s explosion time is `UInstancedStaticMeshComponent::PartialNavigationUpdate` → `CalcBoundsImpl`, a full bounds pass over every instance on each add or remove. The map has no navmesh; robots use [[sys-flow-field]]. Same runs with navigation relevance off (`Subdivide <F> 1`): explosion 0.56 ms (100 uu), 1.0 ms (67 uu); chain event frames 11.6 / 13.9 (100 uu) and 19.2 / 21.4 (67 uu); dig 100 uu 9.90 / 13.27; dig 67 uu still 18.70 / 23.34, render-thread bound (HISM proxy and distance-field buffers re-uploaded every frame, `UpdateDistanceFieldObjectBuffers` up to 14 ms).

## After task-029-01 (2026-10-05)
Grid chunked in 16 × 16-cell HISMs per type, no component navigation relevant ([[sys-grid]]). Same PC, background load gone; "before" = commit 93c6585 rebuilt in a worktree, run the same hour. 100 robots, ms avg / p99 (event frame avg / max).

| Run | 200 uu before | 200 uu after | 67 uu before | 67 uu after |
|---|---|---|---|---|
| S0 frame | 8.50 / 10.26 | 8.73 / 10.91 | 9.53 / 12.18 | 9.24 / 11.77 |
| S2 frame | 8.88 / 11.50 | 8.94 / 11.51 | 9.37 / 11.95 | 9.39 / 11.93 |
| Chain frame (event) | 9.22 / 12.13 (11.1 / 16.9) | 8.88 / 11.63 (9.5 / 13.0) | 11.14 / 111.5 (121 / 189) | 9.31 / 12.07 (10.8 / 16.4) |
| Dig frame (game avg) | 9.34 / 12.07 (5.25) | 9.12 / 11.75 (5.39) | 19.00 / 23.47 (14.08) | 9.59 / 12.15 (7.77) |

- 67 uu: chain event frames 121 → 10.8 ms avg, max 189 → 16 ms; dig 19.0 → 9.6 ms (60 fps regained). Steady state unchanged (± 0.3 ms, noise). Flow-field BFS 2.7–2.8 ms per rebuild remains (task-029-02).
- Distance fields: chunks with `bGridAffectsDistanceFields` on vs off at 67 uu, chain 9.35 / 11.94 vs 9.31 / 12.07, dig 9.60 / 11.97 vs 9.59 / 12.15: no difference, so the grid stays in distance fields (default on, lighting unchanged).

## After task-029-02 (2026-10-05)
Incremental flow field ([[sys-flow-field]], budget 0.3 ms). 67 uu, 100 robots, editor `-game`, 5 s settle, 20 s measure. Background check: no game running (Steam and Epic launchers idle), GPU at 19–21 % from the desktop before the runs. ms avg / p99; flow = flow-field time of frames with work, avg / max.

| Run | Frame | Game avg / p99 | Flow work frames | Flow avg / max |
|---|---|---|---|---|
| Dig (1440 cells) | 9.19 / 11.02 | 6.33 / 9.03 | 1451 | 0.015 / 0.37 |
| Chain (1838 cells, events 10.2 / 13.6) | 9.03 / 10.88 | 6.22 / 9.27 | 32 | 0.051 / 0.22 |
| Walk 600 uu/s, budget 0.3 | 9.25 / 11.20 | 7.16 / 9.94 | 1379 | 0.29 / 0.72 |
| Walk, budget 0 (goal BFS in one frame) | 9.27 / 11.31 | 7.03 / 10.72 | 168 | 2.17 / 3.54 |

- Openings: 2.7 ms per rebuild on every dig frame before, 0.015 ms avg now; dig game thread 7.77 → 6.33 ms avg (029-01 run, other hour).
- Miner moving: the sliced goal rebuild holds 0.3 ms per frame instead of 2.2 ms spikes ~8 times a second; frames stay render bound (draw ≈ 9.2 ms), so frame averages move little. The field goal lags at most 13 frames (headless test).

## Final check at 67 uu (task-029-07, 2026-10-05)
Game defaults (67 uu, `arena_m1_fine`, no subdivide), all task-029 work in, editor `-game` 1920 × 1080, 5 s settle, 20 s measure, one launch per chain and dig run. Background: no game running; Steam, Epic launcher, Discord, Zoom, Brave idle; GPU 22 % from the desktop. ms avg / p99 / max; budget 16.6 avg, 25 p99 ([[task-029-fine-grid]]).

| Run | 100 robots | 400 robots |
|---|---|---|
| S0 frame | 9.12 / 11.05 / 12.1 | 9.97 / 12.98 / 15.8 |
| S2 frame | 9.57 / 11.74 / 12.8 | 11.16 / 14.93 / 59.7 |
| Chain 30 frame (32 explosions, 1713 blocks; event avg / max) | 9.36 / 11.50 / 18.4 (10.8 / 17.7) | 10.08 / 13.70 / 19.4 (13.2 / 19.4) |
| Dig 72 cells/s frame (1426 blocks; flow avg / max) | 9.72 / 12.11 / 13.8 (0.013 / 0.31) | 10.33 / 13.83 / 15.7 (0.013 / 0.31) |

- All within budget. Against the 029-01 / 029-02 rows at 100 robots: S0 9.24 → 9.12, S2 9.39 → 9.57, chain 9.31 → 9.36, dig 9.19–9.59 → 9.72 (± 0.4 ms, noise and the arc dig and beam pool added since); against the task-023-01 67 uu row at 400: S0 10.47 → 9.97, S2 12.30 → 11.16.
- S2 400 max 59.7 ms is one frame (p99 14.9); its cause was not captured (likely a GC, see [[sys-stress]] gotchas).

## Perf session (task-063-01, 2026-10-09)
Same commands as task-029-07, HEAD 94abe5e. PC not quiet: Brave (many tabs), Discord, Steam, Epic launcher, Creative Cloud, uTorrent, NVIDIA Share overlay and other Claude sessions running; CPU load 53–61 % idle, GPU 23 % from the desktop, on AC, Performance plan. The game window presented at 60 Hz (an external 60 Hz monitor on the NVIDIA GPU, the internal panel is 360 Hz): every run is capped at 1200 frames per 20 s, also with `-fullscreen` or `-WinX=1920`; nothing in the project sets a cap (`t.MaxFPS 0`, `r.VSync 0`, `FrameRateLimit 0`). Frame and draw times are therefore the 16.67 ms present interval, not the cost; the game thread is the usable value. ms avg / p99 / max (game avg).

| Run | 100 robots | 400 robots |
|---|---|---|
| S0 frame | 17.18 / 20.93 / 27.1 (9.29) | 19.64 / 24.86 / 28.2 (17.26) |
| S2 frame | 16.71 / 17.10 / 68.7 (6.92) | 16.67 / 18.76 / 21.0 (12.12) |
| Chain 30 (32 explosions, 1713 blocks; event avg / max) | 16.67 / 17.20 / 21.1 (7.00; 16.9 / 21.1) | 16.71 / 20.15 / 26.2 (12.26; 18.4 / 26.2) |
| Dig (1425 blocks; flow avg / max) | 16.67 / 17.26 / 19.3 (7.63; 0.019 / 0.31) | 16.72 / 19.72 / 27.1 (12.68; 0.020 / 0.31) |

- p99 within 25 ms everywhere; averages sit on the 60 Hz cap, so the 16.6 avg check cannot be judged against the task-029-07 table. GPU avg 12.9–14.4 ms (9.4 before) is read at low clocks under the cap. Game thread at 100: chain 6.22 → 7.00, dig 6.33 → 7.63 (task-029-02 rows).
- `FlowField.BudgetAt67uu` alone: 0 / 5 pass (full rebuild 4.74–5.65 ms, work p95 0.32–0.35 ms, goal age max 33–37 frames > 30); in the full run 6.65 ms, 38 frames. `FlowField.DestroyedRockOpensPocket` alone: 5 / 5 pass, and in the full run.
- Full headless run: 193 pass, 2 fail: `BudgetAt67uu`, `Integration.PressureHordeBanner` (icon positions twice the expected x, from the alert bar doubled in task-062-01, unrelated to the flow field).
- Verdict, load: `RRFlowFieldSubsystem` and `RRFlowFieldTests.cpp` are unchanged since task-029-02 (git diff from 9f1fd69); the test spawns no robot and destroys cells directly, so tasks 036, 041, 046 and 060 are not in its path. The same BFS takes 4.7–6.7 ms against 1.8–2.6 ms at task-029-02, and the goal age scales with it (≈ 13 frames then): the 30-frame check is wall-clock bound and fails on a loaded CPU.

## Worker-thread flow field in game (task-066-02, 2026-10-09)
Same commands as task-063-01, HEAD 659833f, `rr.FlowField.Async 1` (default, [[sys-flow-field]]). Owner's normal player profile, nothing closed: Brave, Discord, Steam, Epic launcher, Creative Cloud, uTorrent, NVIDIA Share, Armoury Crate, Logitech G Hub, NordVPN, ShareX, WhatsApp, Parsec, McAfee and other Claude sessions running; CPU load 0–23 % idle, Performance plan, external 60 Hz monitor on the NVIDIA GPU. This time the frames were not paced at 60 Hz (1800–2230 frames per 20 s, cause not identified), so frame times are the cost again. The stress does not log the flow-field goal age (the miner is static in every mode); flow = game-thread flow-field time of frames with work, avg / max. ms avg / p99 / max (game avg; flow), task-063-01 values beside.

| Run | 100 robots | task-063 100 | 400 robots | task-063 400 |
|---|---|---|---|---|
| S0 frame | 8.96 / 11.20 / 21.0 (4.89) | 17.18 / 20.93 / 27.1 (9.29) | 10.69 / 13.61 / 15.6 (9.26) | 19.64 / 24.86 / 28.2 (17.26) |
| S2 frame | 9.23 / 11.52 / 12.2 (5.16) | 16.71 / 17.10 / 68.7 (6.92) | 11.10 / 14.05 / 15.3 (9.62) | 16.67 / 18.76 / 21.0 (12.12) |
| Chain 30 (event avg / max) | 9.19 / 11.69 / 14.0 (5.14; 9.5 / 12.8; flow 0.032 / 0.095) | 16.67 / 17.20 / 21.1 (7.00; 16.9 / 21.1) | 10.08 / 12.88 / 18.9 (8.62; 11.4 / 18.9; flow 0.030 / 0.092) | 16.71 / 20.15 / 26.2 (12.26; 18.4 / 26.2) |
| Dig (1425 blocks) | 9.23 / 11.50 / 20.2 (5.44; flow 0.047 / 0.50) | 16.67 / 17.26 / 19.3 (7.63; 0.019 / 0.31) | 10.44 / 13.17 / 57.9 (9.28; flow 0.046 / 0.27) | 16.72 / 19.72 / 27.1 (12.68; 0.020 / 0.31) |

`rr.FlowField.Async 0` (time-sliced), same hour, 400 robots:

| Run | Async 1 | Async 0 |
|---|---|---|
| S0 frame (game) | 10.69 / 13.61 / 15.6 (9.26) | 10.12 / 12.75 / 13.5 (8.70) |
| Dig frame (game; flow work frames, avg / max) | 10.44 / 13.17 / 57.9 (9.28; 1915, 0.046 / 0.27) | 10.23 / 13.03 / 14.5 (9.04; 1431, 0.012 / 0.31) |

- All runs within 16.6 avg / 25 p99. Dig 400 Async 1 max 57.9 ms is one frame whose game time stays ≤ 15.8 ms: not game-thread work, cause not captured.
- Game thread is 1.8–8 ms lower than task-063 everywhere, also in S0 and S2 where the flow field does no work: the PC load differs (idle CPU 0–23 % vs 53–61 %), so the gain cannot be credited to the worker.
- Worker path on the game thread: work on every dig frame (launch copies of the field), avg 0.046 ms vs 0.012 ms sliced, max 0.27 vs 0.31 ms; negligible against ≈ 9 ms of game thread. Async 1 vs 0 differences (0.2–0.6 ms) are within run noise.

## Verdict
- 200 uu: holds; the navigation fix would also help it (one destruction 0.25 ms here, 0.04–0.07 ms measured with the fix at the smaller sizes).
- 100 uu: steady state and dig hold. Explosion chains break (52 ms frames) and are fixed by turning off navigation relevance on the grid components (≈ 1 h with test), back to the 200 uu level. Recommended.
- 67 uu: steady state holds (+1.5 ms GPU). After the navigation fix, chains reach 19–21 ms and fast dig 18.7 ms average (60 fps lost). Needs HISMs per chunk, e.g. 16 × 16 cells, with blocks kept out of distance fields (≈ 10–12 h), and the flow field computed on a worker thread or repaired incrementally (2.6 ms per rebuild, every frame while digging; ≈ 6–10 h). Total ≈ 17–23 h, the render gain not yet measured.
- Any smaller size also needs cell-counted tunables rescaled (spawner band, drops per ore cell, dig health): gameplay task, not measured here.

## Relations
- [[performance-index]] — parent index.
- [[sys-stress]] — subdivide, bench, chain and dig tools used here.
- [[stress-opt-m1]] — the 200 uu tables of the same procedure.
- [[sys-grid]] — the instanced components whose costs are measured.
- [[task-023-01-cell-stress]] — the task that produced this report.
- [[sys-flow-field]] — the incremental field measured in the task-029-02 section.
- [[task-029-fine-grid]] — the global task whose budget the final 67 uu check measures.
- [[task-063-01-perf-session]] — the task that produced the 2026-10-09 perf session section.
- [[task-066-02-perf-check]] — the task that produced the worker-thread flow field section.
