---
id: task-536-layout-filler-cheaper
description: "Global task: layout generator far-cell filler made cheaper per iteration with byte-identical output per seed; slow seed timed before and after."
parent: tasks-done-index
kind: task
status: done
---
# Layout generator: cheaper far-cell filler, identical output

- request: Owner (2026-11-13, verbatim): "Filler moins coûteux". Picked as recommended among the improvement tracks the main session proposed after reading Source/RogueRigs/Grid/RRLayoutGenerator.cpp. Track as proposed: make FRRLayoutGenerator::RunFiller cheaper WITHOUT changing the generated output (same params and seed give byte-identical cells). Ideas proposed: reuse scratch buffers instead of allocating per iteration; compare squared distances in the nearest-cell scan and take one square root at the end (same strict < order, same pick); skip recomputing the BFS, the far list and the reach mask when the previous walk carved nothing, keeping the random draws identical in number and order. Main session reading (not measured): each iteration runs BfsFromWalkable (Source, Dist, Queue arrays of 57 600), Reach (mask plus stack), then a nearest-cell scan with one square root per reachable cell; up to filler.max_iter (600) iterations; RunFiller called up to 10 times per seed. Hypothesis to time first: the ~6 s of seeds whose far cells the filler cannot reach is spent in this loop. Explicitly NOT in this request (they change the output, separate tracks): early exit from the loop when no far cell can be carved; clearing HiddenGuard when a pocket is refilled. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2 accepted.
- goal: The far-cell filler of the C++ layout generator does less work per iteration, and every seed still gives exactly the same layout as before; the time gained on a slow seed is measured.
- acceptance: A1 Before any change, a slow seed (one whose far cells the filler cannot reach, ~6 s) is timed with the RRLayoutGen commandlet, with the share of carve time spent in RunFiller, and the figures are recorded in the task result; A2 the reference holds after the change: level seed 422704489 with the arena_l3 params and brief gives accepted seed 422704499, cells hash 86c36f6d, as in sys-layout-generator; A3 output identical before and after: on the slow seed and on seeds 3, 1800 and 2900 with the default params, the -carveonly hash and the written JSON are byte-identical before and after, and every seed tried during the reference search logs the same line (same fails and measures) before and after; A4 the same slow seed is timed after the change with the same method, and the before and after times and filler share are given side by side; A5 the build succeeds and the existing layout generator automation tests pass unchanged; A6 the two output-changing tracks (early loop exit, HiddenGuard clearing) are not implemented; A7 sys-layout-generator records the measured times and what the filler now does (the ~6 s gotcha updated with the measured figure).
- gdd_check: Read sys-layout-generator (reference seed and hash, determinism, ~6 s gotcha) and verification-levels. No gdd node touched: pure performance change with byte-identical output, no rule or look changes, so no GDD conflict and no open value used. Milestone: the generator is M2 work already in runs (sys-run-layout); no scope issue. No code read by the Planner: the code facts in request come from the main session reading. Questions Q1 (what if the filler is not where the time goes) and Q2 (seed set for the identity check) asked with defaults.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-13T18:19:35Z
- waiting_for: none
- executor_tokens: input 84, cache write 126991, cache read 3929784, output 23080
- executor_usage: runs 1, turns 42, context start 16k peak 127k, resumes 0, 11 min, units 0.67 M, USD 1.88
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Hypothesis confirmed: on slow seeds the filler is 99 % of the carve (default seed 17: carve 4.88 s, filler 4.81 s, 5431 iterations, 4971 carved nothing; arena_l3 seed 422704496: 4.25 s, 4818 iterations, 4597 idle). After (commit 46003871: buffers once per RunFiller call, rebuild of dist/far/reach only after a carving walk, squared-int nearest scan): seed 17 carve 0.80 s (filler 0.73 s, 91 %), 422704496 0.41 s (89 %); searches 5.55->1.45 s from 17, 7.10->2.94 s from 3, 5.76->1.77 s reference. Output identical: reference 422704499 / 86c36f6d; -carveonly hashes and JSON byte-identical on seeds 3, 17, 1800, 2900 and the reference search, every tried-seed line identical except times; LayoutLoader tests 3/3; no early exit, no HiddenGuard clearing. Doc b8b51ff0. Remaining slow-seed cost is the idle WanderWalk calls (kept: they draw the random values).

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 If the timing before the change shows that RunFiller is not where most of the ~6 s goes, what does the Executor do? Default: it still applies the three output-preserving filler changes, reports the measured split of the time, and optimises nothing else (any other hot spot becomes a separate track proposed to you).
- Q2 Seed set for the "identical output" check. Default: the reference search (level seed 422704489, arena_l3 params and brief, every tried seed compared), plus the slow seed and seeds 3, 1800 and 2900 with the default params, -carveonly hash and JSON compared before and after.

## Subtasks
1. task-536-01-filler-optimise (vault/reference/tasks/task-536-layout-filler-cheaper/task-536-01-filler-optimise.md) — cpp: Time a slow seed, make RunFiller cheaper without changing its output, prove identical output on the seed set and time it again.
2. task-536-02-doc (vault/reference/tasks/task-536-layout-filler-cheaper/task-536-02-doc.md) — doc: Update sys-layout-generator with the measured filler cost and what the filler now reuses or skips.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-536-01-filler-optimise]] — step 1, cpp.
- [[task-536-02-doc]] — step 2, doc.
