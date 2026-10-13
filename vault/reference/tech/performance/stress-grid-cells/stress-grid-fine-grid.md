---
id: stress-grid-fine-grid
description: "Measurements after the fine grid: task-029-01, 029-02 and the final check at 67 uu (2026-10-05). Open for fine-grid costs and budgets."
parent: stress-grid-cells
kind: doc
---
# Grid stress — fine grid

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

## Relations
- [[stress-grid-cells]] — parent hub: summary and verdict.
- [[sys-grid]] — referenced above.
- [[sys-flow-field]] — referenced above.
- [[task-029-fine-grid]] — referenced above.
- [[sys-stress]] — referenced above.
