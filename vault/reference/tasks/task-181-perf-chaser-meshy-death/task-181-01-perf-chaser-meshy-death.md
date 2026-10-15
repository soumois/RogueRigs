---
id: task-181-01-perf-chaser-meshy-death
description: "Measure the frame cost of the Meshy chaser (animated material) and its death effects (puff, debris) with the stress tool, compared with the M1 tables."
parent: task-181-perf-chaser-meshy-death
kind: task
status: done
---
# Performance test: Meshy chaser, animation and death effects

- goal: Measure the frame cost of the Meshy chaser (animated material) and its death effects (puff, debris) with the stress tool, compared with the M1 tables.
- agent: cpp
- depends_on: none
- inputs: sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), perf-stress-test-m1 (vault/reference/tech/performance/perf-stress-test-m1.md), perf-stress-final-m1 (vault/reference/tech/performance/perf-stress-final-m1.md), perf-stress-grid-cells (vault/reference/tech/performance/perf-stress-grid-cells.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: Small C++ addition: each Stress step line and CSV row also logs, per Niagara system (NS_RobotDeathPuff, NS_RobotDebris), the plays and the skips of URRNiagaraPoolSubsystem during the measure window (pool counters already exist). Runs with Scripts/Run-StressTest.ps1, editor closed, editor binary -game, owner everyday PC profile: S0 peace chasers 50,100,200,300,400,600; S2 fire 50,100,200,300,400; S7 burst -Class Chaser 50, 100, 200 (one launch per count, MinCells 8, window long enough to cover the 3.4 s debris); game mode caps 100, 200, 300 (one launch per cap). Frame, game, draw, GPU avg/p99/max per step. A new perf node perf-stress-chaser-meshy under performance-index with the tables, the comparison with perf-stress-final-m1 (S0, S2, S7) and perf-stress-grid-cells (game mode), the puff/debris skips, and a verdict; an Insights trace only on a step clearly above the M1 numbers.
- done_when: All scenario steps ran and their CSV/log are under Saved/Stress; tables and comparison written in the new perf node; build passes with the editor closed; existing stress tests still pass; report sent to the owner first; committed by path
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 108, cache write 233519, cache read 4240670, output 36800
- runs: 1
- milestone: M3
- result: Pool play/skip counters in Stress step and CSV; S0 50-600 avg 7.99-11.13 ms, S2 50-400 8.32-10.55 (faster than M1, game thread -32 to -51 %); new draw/GPU +5.6 us per robot (trace S0 600: Lumen card captures likely); S7 chasers 50-200 max 27-28 ms (M1 49.6), cap 40 holds; game caps 100/200/300 avg 9.73/10.75/12.28, p99 12.3/15.4/18.2; debris skipped 1/30/45 % of deaths; GC spikes 39-47 ms as in M1; report perf-stress-chaser-meshy; commits c719274c, bc42ec48

## Relations
- [[task-181-perf-chaser-meshy-death]] — parent global task.
