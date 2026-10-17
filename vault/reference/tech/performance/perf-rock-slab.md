---
id: perf-rock-slab
description: "Rock look and grid cost around the dual-grid slab rock: fixed camera spots, captures, frame, GPU, triangle, draw counts. Open to compare before and after."
parent: performance-index
kind: doc
---
# Rock slab — captures and grid cost

Design: [[rock-state-render-options]]. Same PC, owner's everyday profile (nothing closed), editor binary `-game` on `L_ArenaM1`, layout `arena_m1_fine` (67 uu cells, grid at world origin: cell (x, y) center = ((x+0.5)·67, (y+0.5)·67)), 1920 × 1080 windowed, `t.MaxFPS 0`, `r.VSync 0`. Screen up = +X, right = +Y.

## Fixed spots (reuse unchanged after the change)
Miner placed by `BugItGo <X> <Y> 90.15 0 0 0` (cheat manager, ghost mode), `rr.Spawner.Enabled 0`, `rr.Stress.Peace 1`, shot `rr.Debug.Shots <t> 1 1 <prefix> 0` 3 s after the move, files moved to `Saved/RockSlab/<before|after>/`.

| Spot | Miner X, Y (cell) | Shows |
|---|---|---|
| A spawn | 8100, 8100 (start, no move) | open floor, rock masses all sides, Duskite squares top left |
| B intact wall | 8140.5, 8911 (121, 133) | straight rock face 3 cells right of the miner |
| C corridor corner | 11423.5, 4589.5 (170, 68) | 6-cell corridor turning into a cross passage, barrel, heal vein, many Duskite |
| D heal vein | 10083.5, 2043.5 (150, 30) | red 2 × 2 heal vein (150–151, 24–25) left of the miner, void edge |
| E Duskite | 8341.5, 7604.5 (124, 113) | rock mass on the left with several Duskite 2 × 2 deposits |
| F dig | 8140.5, 9076 (121, 135.5; touches cell (121, 136)) | drone digs on contact; shots at +0.4, +0.9, +1.4 s |

## Baseline (task-230-01, 2026-10-17, HEAD d1b43a25)
Captures in `Saved/RockSlab/before/` (not committed): `rs_a_spawn_00`, `rs_b_wall_00`, `rs_c_corner_00`, `rs_d_heal_00`, `rs_e_duskite_00`, `rs_f_dig_00` (lasers on the face, blocks cut), `_01` and `_02` (notch about 2 × 3 cells, drone back in orbit). Rock: one grey cube per cell, grid lines on top, Duskite violet, heal red.

Perf: [[perf-stress-grid-cells]] procedure, `rr.Stress.Run 100 5 20 0 <peace|chain>` at spot A, one launch each, with `csvprofile start/stop` (works from the console; `Saved/Profiling/CSV/`), measure window sliced on the `Stress_100_Measure` event. Triangles = `RHI/PrimitivesDrawn`. ms avg / p99 (max).

| Run | Frame | Game | Render | GPU | Draws avg (max) | Triangles avg (max) |
|---|---|---|---|---|---|---|
| Steady, 100 robots | 8.96 / 11.06 (14.5) | 4.62 / 6.49 | 8.95 / 11.04 | 7.34 / 7.82 | 928 (992) | 807 k (1 024 k) |
| Chain 30, 100 robots (32 explosions, 4 759 blocks; event frames 51, avg 13.3, max 17.9) | 9.70 / 13.82 (17.9) | 6.77 / 13.94 | 9.59 / 11.74 | 7.30 / 7.78 | 901 (1 009) | 813 k (993 k) |
| No robots, spot A | 8.33 / 10.11 | 3.74 / 5.60 | 8.32 / 10.11 | 6.85 / 7.31 | 796 | 107 268 |
| No robots, spot C | 8.27 / 10.50 | 3.84 / 5.42 | 8.25 / 10.32 | 6.95 / 7.38 | 763 | 107 160 |

- Render thread ≈ frame: the frame is render-thread bound. Shadow depth draws ≈ 650 of the draws (frames reporting per-pass counts).
- Grid-only triangles barely change between spots A and C (107 k; cause not checked); compare the slab change on these rows.

## Relations
- [[performance-index]] — parent index.
- [[rock-state-render-options]] — the slab rock design measured here.
- [[perf-stress-grid-cells]] — the stress procedure and earlier grid costs.
- [[sys-stress]] — run modes and samples used.
- [[task-230-01-rock-baseline-capture]] — the task that produced the baseline.
