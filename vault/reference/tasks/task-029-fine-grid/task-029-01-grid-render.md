---
id: task-029-01-grid-render
description: "Grid components no longer navigation-relevant, instanced meshes split into chunks, blocks out of distance fields; chain and dig measured at 67 uu."
parent: task-029-fine-grid
kind: task
status: done
---
# Grid rendering at fine cells

- goal: destruction no longer stalls game or render thread at fine cells.
- agent: cpp
- depends_on: task-025-06-numbers-visual-check
- inputs: [[sys-grid]], [[sys-explosions]], [[perf-trace-m1]], [[cpp-standard]], [[sys-automation-tests]], [[stress-grid-cells]], [[sys-stress]]
- expected_output: FIRST explain the GPU regression: S2 at 100 robots measured 22–26 ms frame, GPU-bound, in task-025-06 (numbers on and off) versus about 9.7 ms in stress-opt-m1; find the cause with a GPU trace or by bisecting recent content (turret, pellets, Niagara, materials, lighting, settings) and report it before optimizing; then grid HISM components with navigation relevance off (map has no navmesh), instanced meshes split into chunks (e.g. 16 × 16 cells per component per block type, only the touched chunk updated), blocks excluded from distance fields if that removes the upload spikes (check lighting impact and report), same visuals; measured with rr.Stress.Subdivide at 200 and 67 uu: chain of 30 explosions and dig sweep, avg/p99/max, compared with stress-grid-cells; docs sys-grid, stress-grid-cells (after section).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean. Measurements recorded.
- editor_state: closed
- token_ceiling: 130000
- model: opus
- tokens: input 170, cache write 598050, cache read 10948435, output 75415
- runs: 2
- milestone: M2
- result: GPU regression explained: a background game (GraveyardKeeper2.exe) used the GPU during the 025 runs; same commit back to 8.5 ms once closed; rule recorded: close other GPU apps before measuring. Grid: one HISM per block type per 16 × 16-cell chunk (ChunkCells) with per-chunk collision ISM, only touched chunks update; no grid component navigation-relevant; bGridAffectsDistanceFields kept on (no gain off with chunks). At 67 uu: chain 30 frames 121/189 → 10.8/16.4 ms, dig 19.0 → 9.6 ms avg; steady state unchanged. Flow-field rebuild still 2.7–2.8 ms (029-02). Test Dig.ChunkedInstances; missing include in RRRunFlowTests fixed; 150/150. Docs sys-grid, sys-stress, stress-grid-cells. Note: re-save L_ArenaM1 to drop old template instances (029-06). Commits 93c6585, 9b37922, 68e2a8a.

## Relations
- [[task-029-fine-grid]] — parent global task.
