---
id: task-550-01-generator-back
description: "Cpp: Bring the C++ generator back to its pre-537 maps (task-536 kept), check the old hashes and the 24 starts, report timings"
parent: task-550-layout-generator-back-pre-537
kind: task
status: done
---
# Générateur de layout : retour aux cartes d'avant task-537 (garde des poches): cpp

- goal: Bring the C++ generator back to its pre-537 maps (task-536 kept), check the old hashes and the 24 starts, report timings
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-537-layout-stale-pocket-guard (vault/reference/tasks/task-537-layout-stale-pocket-guard.md), task-537-01-guard-release (vault/reference/tasks/task-537-layout-stale-pocket-guard/task-537-01-guard-release.md), task-536-layout-filler-cheaper (vault/reference/tasks/task-536-layout-filler-cheaper.md), task-495-02-ld-review (vault/reference/tasks/task-495-generator-window-repair/task-495-02-ld-review.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md)
- expected_output: see done_when
- done_when: A1 to A4 of the global hold: pre-537 behaviour back with task-536 kept, proved by the pre-537 `-carveonly` hashes and reference line 422704499 / 86c36f6d (two runs; a differing hash is reported with its cause, nothing tuned to compensate), 24 starts with the pre-537 accepted seeds and validator OK in a table, timings stated, tests pass, Node script, committed layouts and params unchanged; code committed by path.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-550
- usage: included in the executor run of task-550
- runs: 1
- milestone: M2
- started: 2026-11-13T23:48:25Z
- waiting_for: none
- result: Done (code cc8a7dc8). Change: the task-537 guard release is removed (ReleaseStaleGuard and its calls from RefillRegions and PassageRepair, GuardReleased and its getter, the 'guard cells released' field of the -carveonly pockets line); HiddenGuard again keeps the guard of pocket cells put back to rock, as before 537. Task-536 kept unchanged (RunFiller scratch buffers, BfsInto/ReachInto, rebuild only after a carving walk, squared-int nearest scan, filler timing line). Counters kept, measure only (Q3): pockets placed/removed/partly removed, guard cells stale/missing (BuildGuard, CountGuardMismatch), idle walks that lost rock to a guard (LastWalkGuardDropped in the WanderWalk filter, same filter predicate). A1 hashes all equal to pre-537: -carveonly seed 3 d6b4a786, 1800 53b2a01a, 2900 98a7d029, 17 0bef55d5, arena_l3 422704496 01c42e39, same on rerun, narrow 0; reference 422704489 (arena_l3 params and brief) run twice: 422704499 accepted after 11, cells hash 86c36f6d both runs, JSON identical except the name field, search 3.08 / 2.95 s. A2 24 starts (default params, arena_m1_fine.gen.brief) accepted seed/tried/search s, all equal to the pre-537 column and validate_layout.js exit 0: 1000 1004/5 1.09; 1100 1100/1 0.11; 1200 1200/1 0.19; 1300 1303/4 0.81; 1400 1401/2 0.27; 1500 1502/3 0.54; 1600 1600/1 0.11; 1700 1705/6 1.11; 1800 1818/19 3.25; 1900 1901/2 0.35; 2000 2000/1 0.17; 2100 2101/2 0.34; 2200 2200/1 0.11; 2300 2300/1 0.17; 2400 2403/4 0.73; 2500 2505/6 0.85; 2600 2600/1 0.19; 2700 2702/3 0.76; 2800 2806/7 1.29; 2900 2903/4 0.80; 6450 6453/4 1.19; 7000 7002/3 1.45; 7900 7900/1 0.19; 7950 7950/1 0.13. Total search 16.2 s (16.6 before 537, 10.9 after 537). A3 -carveonly filler: seed 17 carve 1.30 s (3 runs 1.30-1.39), filler 1.19 s (91 %), 5431 iterations, 4971 idle (task-536 0.80/0.73 s, task-537 0.19/0.10 s, 55 it, 1 idle); 422704496 carve 0.67 s, filler 0.60 s (89 %), 4818 it, 4597 idle (task-536 0.41/0.37 s, task-537 0.18/0.10 s, 60 it, 2 idle). Iteration counts equal to task-536, so the work is the same; the slower seconds were measured while a game ran on the machine (not verified further). A4 build OK (one C4996 Cast<> warning from engine Casts.h, not in the touched files), RogueRigs.LayoutLoader 3/3; no test existed for the guard release; Node script, Content/RogueRigs/Data/Levels and params unchanged. Files Saved/GeneratedLevels/task-550 (temporary).

## Relations
- [[task-550-layout-generator-back-pre-537]] — parent global task.
