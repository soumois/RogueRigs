---
id: task-023-01-cell-stress
description: "Generate test-only arenas with 100 uu and about 67 uu cells, measure rendering, destruction, flow field, spawn and line of sight in -game."
parent: task-023-grid-perf
kind: task
status: done
---
# Smaller-cell stress test

- goal: cost of smaller cells measured.
- agent: cpp
- depends_on: none
- inputs: [[arena-grid]], [[sys-grid]], [[sys-flow-field]], [[sys-spawner]], [[sys-explosions]], [[sys-stress]], [[perf-stress-opt-m1]], [[perf-trace-m1]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: a test-only way to run the stress map with a cell size parameter (200, 100, 67 uu) keeping the same world size and rock ratio (e.g. scaled copy of the M1 layout, test map or stress command; nothing shipped changes); measurements in -game (and staged build if quick): S0 and S2 at 100 and 400 robots, instance counts and draw/GPU ms, mass destruction (explosion chains breaking many blocks, a dig sweep), flow-field rebuild ms per destroyed block, spawn candidate scan ms, LOS cost, memory; Insights trace on the worst case; new node `stress-grid-cells` with tables, the bottlenecks per size and what would need optimizing (e.g. HISM per chunk, incremental flow field) with hour estimates. No gameplay or default change.
- done_when: the -game runs completed with real numbers; report node written; lint clean.
- editor_state: closed
- token_ceiling: 120000
- model: opus
- tokens: input 130, cache write 457557, cache read 7854099, output 67250
- runs: 2
- milestone: M2
- result: Test-only rr.Stress.Subdivide (80², 160², 240² cells of 200/100/67 uu on L_ArenaM1), rr.Stress.GridBench, chain and dig modes. Normal play holds at all sizes (S0 400: 10.1 / 10.6 / 10.5 ms; S2 400: 12.7 / 12.4 / 12.3). Destruction breaks: one explosion 1.7 / 22 / 92 ms, chain frames max 38 / 52 / 208 ms; cause: instanced-mesh navigation update recomputing bounds on each add/remove (map has no navmesh) + distance-field uploads. With navigation off on the grid (what-if): 100 uu back to baseline; 67 uu still 19–21 ms chains and 18.7 ms dig (render thread, flow-field rebuild 2.6 ms). Recommendation: 100 uu + 1 h navigation fix; 67 uu needs HISM chunking (10–12 h) and async/incremental flow field (6–10 h). Memory +35 / +115 MB. New node stress-grid-cells; sys-stress. Commit a416610.

## Relations
- [[task-023-grid-perf]] — parent global task.
