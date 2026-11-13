---
id: task-536-01-filler-optimise
description: "Cpp: Time a slow seed, make RunFiller cheaper without changing its output, prove identical output on the seed set and time it again."
parent: task-536-layout-filler-cheaper
kind: task
status: done
---
# Layout generator: cheaper far-cell filler, identical output: cpp

- goal: Time a slow seed, make RunFiller cheaper without changing its output, prove identical output on the seed set and time it again.
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), Source/RogueRigs/Grid/RRLayoutGenerator.cpp, Source/RogueRigs/Grid/RRLayoutGenerator.h
- expected_output: C++ change committed; before/after timing table and identity comparison (seed, hash before, hash after) in the subtask result.
- done_when: A1 to A6 of the global hold: before and after times of the slow seed with the filler share recorded in the subtask result, reference seed 422704499 / hash 86c36f6d reproduced, -carveonly hash and JSON byte-identical before and after on the seed set of Q2, every tried seed of the reference search logs the same line, build and existing generator tests pass, no output-changing track implemented.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-536
- usage: included in the executor run of task-536
- runs: 1
- milestone: M2
- started: 2026-11-13T18:19:35Z
- waiting_for: none
- result: Commit 46003871. A1 before (instrumented, -carveonly, 3 runs, default params): slow seed 17 carve 4.88-4.96 s, filler 4.81-4.89 s = 99 % of carve, 5431 iterations of which 4971 carved nothing; arena_l3 seed 422704496 carve 4.20-4.28 s, filler 99 %, 4818 iterations (4597 idle). Hypothesis confirmed. A4 after (same method): seed 17 carve 0.80 s, filler 0.73 s (91 %); seed 422704496 carve 0.41 s, filler 0.37 s (89 %); per-seed Generate time 4.87 -> 0.79 s and 4.28 -> 0.42 s; searches 5.55 -> 1.45 s (from 17), 7.10 -> 2.94 s (from 3), 5.76 -> 1.77 s (reference). Remaining filler cost is the idle walks (WanderWalk, kept: it draws the random values). A2 reference 422704489 arena_l3 -> 422704499, hash 86c36f6d. A3 identical: -carveonly hash 3 d6b4a786, 1800 53b2a01a, 2900 98a7d029, 17 0bef55d5, 422704496 (l3) 01c42e39, same before/after with same iteration counts; JSON byte-identical (cmp) for searches from 3 (24, 27e18ef5), 1800 (1818, 18290511), 2900 (2903, e010db27), 17 (24) and the reference; every seed line of the 5 searches identical except times (64 lines). A5 build ok, no new warning; RogueRigs.LayoutLoader 3/3 pass. A6 no early exit, no HiddenGuard clearing. Change: RunFiller scratch buffers once per call (new BfsInto, ReachInto; Bfs, Reach wrap them), rebuild of dist/far/reach only when the walk carved (FloorCount changed), nearest scan over a reachable index list with squared ints and one sqrt; instrumentation FillerSeconds/Iterations/IdleIterations logged on a second -carveonly line.

## Relations
- [[task-536-layout-filler-cheaper]] — parent global task.
