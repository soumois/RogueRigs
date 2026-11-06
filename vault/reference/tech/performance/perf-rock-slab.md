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

## After the slab change (task-230-12, 2026-10-18, HEAD 76edbf29)
Captures in `Saved/RockSlab/after/` (not committed): spots A-F under the same names, `rs_f_digearly_00` (+0.2 s, cells mid-dig), `rs_g_far_00` (`fov 130` after the dig), `rs_h_crystals_00` (spot D, `fov 50`); side-by-side `sbs_rs_<spot>.png` (before left). `rs_d_heal_00` is from a second launch: in the first, a 3 s hitch left the camera blurred mid-move. 20 987 pieces in 633 piece HISMs (log).

Look: continuous slab outlines with a light rim band, striated wall faces, no seams or misaligned outlines at any spot (the `bRockPieceImportFlipsY` true setting looks right); no holes or missing rock seen. Heal crystals red and glowing; Duskite crystals are grey clusters without violet (default mesh material). Mid-dig the dug cells show a darker, rough patch; after the dig the notch edges look like intact edges (no visible sag at this camera). Far view: no visible artefact. Marked-cell pulse not seen: the console fell outside the view in both tries.

Perf: same procedure; times from the `Saved/Stress` summary, draws and triangles from the CSV (measure event to `csvprofile stop`). `RHI/PrimitivesDrawn` does not count Nanite triangles, so the triangle column no longer measures the rock.

| Run | Frame | Game | Render | GPU | Draws avg (max) | Triangles avg (non-Nanite) |
|---|---|---|---|---|---|---|
| Steady, 100 robots | 9.92 / 11.60 (13.2) | 4.72 / 6.70 | 9.90 / 11.52 | 8.58 / 8.84 | 887 (954) | 722 k |
| Chain 30, 100 robots (event frames 42, avg 151.9, max 5 701) | 14.14 / 15.02 (5 701) | 6.41 / 14.99 | 13.97 / 11.96 | 8.72 / 9.76 | 856 (1 131) | 742 k |
| No robots, spot A | 9.55 / 10.97 | 3.82 / 5.57 | 9.52 / 10.90 | 8.28 / 8.69 | 769 | 33 k |
| No robots, spot C | 9.62 / 11.31 | 3.77 / 5.35 | 9.60 / 11.24 | 8.35 / 8.78 | 735 | 32 k |

- Versus baseline: GPU +1.2 to +1.4 ms on every row, frame +1.0 to +1.3 ms (render-thread bound as before), draws −3 to −4 %. Game thread unchanged.
- Chain: first launch crashed (Lumen `LumenMeshCards.cpp:1332` assertion, after ensures `DoubleFloat.cpp:19` "precision loss converting matrix" and `Matrix.h:468` in `UpdateDistanceFieldObjectBuffers`, 0.4 s after the chain started); second launch hit the same ensures without crashing, and the ensure stack walks made the 5.7 s frame. An instance transform sent to the distance field scene is invalid during explosion destruction (cause not traced).
- Triangle budget ([[rock-render-counts]]): intact meshes are within it (full 2, edge 43 of 48, outer 42 of 48, inner 58 of 64, diagonal 86 of 96, from `SM_RockSlab.report.json`): about 302 k arena-wide against 335 k. Broken meshes run 120-420 triangles (inner 372-420, diagonal 360-408, edge 204-228) against the 80-150 estimate. Verdict: steady state holds the 16.7 / 25 ms budget with margin (+1.3 ms GPU); the chain crash blocks a verdict on destruction.

## After the ISM fix (task-230-14, 2026-10-18, HEAD 6838e0c5)
Rock pieces on plain ISMs instead of HISMs ([[sys-grid-rock-pieces]], cause of the 230-12 chain crash there). Same procedure through `Scripts/Run-StressTest.ps1 -Counts 100 -Settle 5 -Measure 20 -Mode chain -PreExec "csvprofile start"` (quit at the end, one launch): no crash, no ensure in the log. Draws and triangles from the CSV, which the quit left unfinished: 1 892 of the 2 081 measured frames.

| Run | Frame | Game | Render | GPU | Draws avg (max) | Triangles avg (non-Nanite) |
|---|---|---|---|---|---|---|
| Chain 30, 100 robots (32 explosions, 4 759 blocks; event frames 37, avg 11.61, max 17.79) | 9.61 / 11.94 (17.8) | 5.81 / 11.22 | 9.56 / 11.49 | 8.47 / 9.23 | 859 (973) | 738 k |

- Versus the 230-12 chain row: the 5.7 s ensure frame is gone (max 17.8 ms, as the baseline 17.9); frame and GPU close to the baseline chain (9.70 / 7.30) with GPU +1.2 ms, the steady-state slab cost. Steady rows not re-measured.

## Control captures (task-230-26, 2026-10-19, HEAD 348629dc)
Captures in `Saved/RockSlab/after4/` (not committed): `rs_a_spawn_00`, `rs_e_duskite_00`, `rs_h_crystals_00`, `rs_s_sag_game_00`, `rs_s_sag_close_00`, `rs_s_sag_low_00`; side-by-side `sbs_<name>.png` (`Saved/RockSlab/after3/` left, after4 right, `ffmpeg -i after3/<n> -i after4/<n> -filter_complex hstack`). Files land in `Saved/Screenshots/WindowsEditor/` and are moved by hand. The 230-19 command lines were not recorded; these are the ones to repeat from now on.

Launch, one per spot (editor closed): `UnrealEditor.exe "<path>/RogueRigs.uproject" L_ArenaM1 -game -windowed -ResX=1920 -ResY=1080 -log -ExecCmds="t.MaxFPS 0, r.VSync 0, rr.Spawner.Enabled 0, rr.Stress.Peace 1, <spot commands>"`

| Spot | `<spot commands>` |
|---|---|
| A spawn | `rr.Debug.Delayed 5 rr.Debug.Shots 3 1 1 rs_a_spawn 0, rr.Debug.Delayed 12 quit` |
| E Duskite | `rr.Debug.Delayed 5 BugItGo 8341.5 7604.5 90.15 0 0 0, rr.Debug.Delayed 5.3 Walk, rr.Debug.Delayed 5.5 rr.Debug.Shots 3 1 1 rs_e_duskite 0, rr.Debug.Delayed 12 quit` |
| H crystals (spot D) | `rr.Debug.Delayed 5 BugItGo 10083.5 2043.5 90.15 0 0 0, rr.Debug.Delayed 5.3 Walk, rr.Debug.Delayed 5.4 fov 50, rr.Debug.Delayed 5.5 rr.Debug.Shots 3 1 1 rs_h_crystals 0, rr.Debug.Delayed 12 quit` |
| S sag | `rr.Debug.Delayed 5 BugItGo 8140.5 8911 90.15 0 0 0, rr.Debug.Delayed 5.3 Walk, rr.Debug.Delayed 5.5 rr.Debug.DamageCell 119 136 0.3, rr.Debug.Delayed 5.5 rr.Debug.DamageCell 120 136 0.7, rr.Debug.Delayed 5.5 rr.Debug.DamageCell 121 136 0.3, rr.Debug.Delayed 5.5 rr.Debug.DamageCell 122 136 0.7, rr.Debug.Delayed 5.5 rr.Debug.DamageCell 123 136 0.7, rr.Debug.Delayed 5.5 rr.Debug.DamageCell 123 137 0.3, rr.Debug.Delayed 5.6 rr.Debug.Shots 3 1 1 rs_s_sag_game 0, rr.Debug.Delayed 9.5 rr.Debug.Camera -35 900 90, rr.Debug.Delayed 9.6 rr.Debug.Shots 3 1 1 rs_s_sag_close 0, rr.Debug.Delayed 13.5 rr.Debug.Camera -10 1000 90, rr.Debug.Delayed 13.6 rr.Debug.Shots 3 1 1 rs_s_sag_low 0, rr.Debug.Delayed 18 quit` |

Look versus after3: heal crystals pink-red instead of orange. Broken-piece tops near the spot D notch carry the grain, but one flat lavender square remains there and the 0.7 sag cells still show flat lavender tops; thin piece outlines are more visible on the broken area. Tops barely darker (mean RGB of a top patch at spot D 189 182 197 to 185 177 195): still near white. No rim band on the top outline; a thin light line appears along the foot of wall faces at spot A.

## Chamfer and facets (task-480, 2026-11-06)
Triangles per piece, before -> after: full 2 -> 2, edge 43 -> 35, outer 42 -> 34, inner 58 -> 44, diagonal 86 -> 70 (each variant); broken unchanged, 120 to 420. Piece ISMs 633 -> 1078 for 20 987 pieces (2 variant slots per shape).

`Scripts/Run-StressTest.ps1 -Counts 100 -Settle 5 -Measure 20 -Mode peace`, ms avg / p99. Before = pre-task assets restored from git for one run; the runs are not from the fixed spots above.

| Run | Frame | Game | GPU |
|---|---|---|---|
| Before | 13.13 / 15.80 | 4.78 / 7.54 | 11.37 / 13.13 |
| Final material, run 1 | 12.25 / 14.70 | 4.32 / 6.40 | 10.71 / 12.49 |
| Final material, run 2 | 12.63 / 15.51 | 4.46 / 6.67 | 10.96 / 12.90 |

Not worse. The lower figures are within what one baseline run can show, so no gain is claimed.

## Relations
- [[performance-index]] — parent index.
- [[rock-state-render-options]] — the slab rock design measured here.
- [[perf-stress-grid-cells]] — the stress procedure and earlier grid costs.
- [[sys-stress]] — run modes and samples used.
- [[task-230-01-rock-baseline-capture]] — the task that produced the baseline.
- [[task-230-12-rock-slab-check]] — the task that produced the after section.
- [[task-230-14-barrel-chain-crash]] — the task that produced the ISM-fix chain row.
- [[task-230-26-rock-control-capture-2]] — the task that produced the control captures and command lines.
- [[rock-render-counts]] — the provisional triangle budget compared in the after section.
- [[task-480-rock-slab-chamfer-facets]] — the task that produced the chamfer and facets section.
