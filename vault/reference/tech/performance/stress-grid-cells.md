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
