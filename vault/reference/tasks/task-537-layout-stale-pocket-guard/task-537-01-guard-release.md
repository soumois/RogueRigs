---
id: task-537-01-guard-release
description: "Cpp: Release the guard of removed hidden pockets in the C++ generator, keep live pockets guarded, revalidate and measure before/after"
parent: task-537-layout-stale-pocket-guard
kind: task
status: done
---
# Layout generator: the guard of a removed hidden pocket no longer blocks carving: cpp

- goal: Release the guard of removed hidden pockets in the C++ generator, keep live pockets guarded, revalidate and measure before/after
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md), task-536-layout-filler-cheaper (vault/reference/tasks/task-536-layout-filler-cheaper.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md)
- expected_output: Before measures taken on the current code first (A3 table "before", A4, A5), then the change, then the same measures after; numbers written in the subtask result.
- done_when: A1 to A6 of the global hold; the before/after table (24 starts), the reference line and the seed 17 / 422704496 filler measures with the cause verdict are written in the result. Per Q3, a start with no passing seed stops the task in wait-answers.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-537
- usage: included in the executor run of task-537
- runs: 1
- milestone: M2
- started: 2026-11-13T19:32:29Z
- waiting_for: none
- result: Done (code c324273f). Change: RefillRegions and PassageRepair rebuild HiddenGuard from the cells still hidden (ReleaseStaleGuard) when they put a hidden cell back to rock; -carveonly logs pockets placed/removed/partly removed, guard cells released, stale and missing guard cells, idle walks that lost rock to a guard. RunFiller caches unaffected (the guard only changes outside RunFiller). A1: after the change stale 0 and missing 0 on every seed checked (5 carveonly seeds, 300-seed scan 1-300); released cells: seed 17 1280 (2 pockets removed, 5 partly), 422704496 1811 (2 removed, 3 partly), seed 3 627, 1800 500, 2900 330. Every seed of the 300 had pocket cells removed (passage repair trims pocket edges; min 45 guard cells released), so no seed without removal exists at default params. A2: two -carveonly runs identical on all 5 seeds, reference JSON identical over two runs; same cells hash as before on 65 of 300 scanned seeds and on starts 1000 (1004, 045a990e) and 2600 (2600, 7a0b0c1b), JSON byte-identical. A3 24 starts (default params, arena_m1_fine.gen.brief), before -> after as accepted seed/tried, window, lane, farthest pocket, search s; all 24 pass and validate_layout.js OK both times: 1000 1004/5 0.099-0.884 22 9 1.19 -> same 1.03; 1100 1100/1 0.075-0.910 18 10 0.12 -> same 0.12; 1200 1200/1 0.095-0.927 29 7 0.19 -> 1200/1 0.095-0.929 29 7 0.21; 1300 1303/4 0.094-0.925 18 7 0.85 -> 1303/4 0.094-0.918 20 7 0.78; 1400 1401/2 0.085-0.918 18 8 0.30 -> 1401/2 0.113-0.924 23 8 0.27; 1500 1502/3 0.073-0.927 25 10 0.57 -> 1500/1 0.092-0.903 20 8 0.18; 1600 1600/1 0.109-0.901 19 5 0.14 -> same 0.14; 1700 1705/6 0.089-0.924 15 10 1.14 -> 1702/3 0.062-0.873 24 7 0.54; 1800 1818/19 0.085-0.925 23 10 3.31 -> 1818/19 0.108-0.929 23 10 3.09; 1900 1901/2 0.099-0.915 24 9 0.36 -> 1900/1 0.080-0.924 19 6 0.15; 2000 2000/1 0.083-0.929 20 5 0.19 -> 2006/7 0.137-0.920 28 8 1.10; 2100 2101/2 0.075-0.920 17 10 0.40 -> 2101/2 0.075-0.929 17 10 0.30; 2200 2200/1 0.075-0.922 21 7 0.10 -> same 0.11; 2300 2300/1 0.076-0.913 14 7 0.18 -> 2300/1 0.080-0.913 15 7 0.16; 2400 2403/4 0.073-0.915 29 8 0.73 -> 2403/4 0.099-0.915 29 8 0.40; 2500 2505/6 0.075-0.929 22 6 0.90 -> 2505/6 0.161-0.929 22 6 0.55; 2600 2600/1 0.125-0.913 29 5 0.18 -> same 0.11; 2700 2702/3 0.083-0.929 24 8 0.77 -> 2700/1 0.071-0.927 22 10 0.13; 2800 2806/7 0.082-0.906 21 6 1.28 -> 2803/4 0.076-0.925 23 9 0.43; 2900 2903/4 0.075-0.905 24 9 0.76 -> 2903/4 0.075-0.905 24 9 0.43; 6450 6453/4 0.115-0.906 17 8 1.15 -> 6453/4 0.115-0.906 17 6 0.40; 7000 7002/3 0.080-0.880 20 6 1.44 -> 7000/1 0.080-0.929 23 8 0.09; 7900 7900/1 0.094-0.917 19 7 0.21 -> 7900/1 0.094-0.920 21 7 0.13; 7950 7950/1 0.073-0.918 24 6 0.13 -> 7950/1 0.116-0.918 24 6 0.08. Total search 16.59 -> 10.93 s. A4 reference (level seed 422704489, arena_l3 params and brief, two runs): accepted seed 422704499 unchanged (11 tried), cells hash 86c36f6d -> 34b074db (new, same in both runs, every tried-seed line identical), search 3.13 -> 1.85 s. A5 -carveonly before -> after: seed 17 carve 0.81 -> 0.19 s, filler 0.74 -> 0.10 s, 5431 -> 55 iterations, idle 4971 -> 1; 422704496 carve 0.72 -> 0.18 s, filler 0.64 -> 0.10 s, 4818 -> 60 iterations, idle 4597 -> 2. Verdict YES: on the unchanged trajectory (instrumented build, same hashes as before) 4968 of 4971 idle walks of seed 17 and 4494 of 4597 of 422704496 lost a rock cell to a stale guard; over seeds 1-300 idle walks 48000 -> 8739. A6 LayoutLoader 3/3, build OK; Node script, committed layouts and params unchanged. Files Saved/GeneratedLevels/task-537 (temporary).

## Relations
- [[task-537-layout-stale-pocket-guard]] — parent global task.
