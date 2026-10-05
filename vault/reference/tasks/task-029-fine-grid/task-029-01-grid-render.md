---
id: task-029-01-grid-render
description: "Grid components no longer navigation-relevant, instanced meshes split into chunks, blocks out of distance fields; chain and dig measured at 67 uu."
parent: task-029-fine-grid
kind: task
status: todo
---
# Grid rendering at fine cells

- goal: destruction no longer stalls game or render thread at fine cells.
- agent: cpp
- depends_on: task-025-06-numbers-visual-check
- inputs: [[sys-grid]], [[sys-explosions]], [[perf-trace-m1]], [[cpp-standard]], [[sys-automation-tests]], [[stress-grid-cells]], [[sys-stress]]
- expected_output: grid HISM components with navigation relevance off (map has no navmesh), instanced meshes split into chunks (e.g. 16 × 16 cells per component per block type, only the touched chunk updated), blocks excluded from distance fields if that removes the upload spikes (check lighting impact and report), same visuals; measured with rr.Stress.Subdivide at 200 and 67 uu: chain of 30 explosions and dig sweep, avg/p99/max, compared with stress-grid-cells; docs sys-grid, stress-grid-cells (after section).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean. Measurements recorded.
- editor_state: closed
- token_ceiling: 130000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
