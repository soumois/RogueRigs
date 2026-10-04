---
id: task-013-02-final
description: "Rerun the stress procedure after task-016: S0, S2 continuous fire, S7 mass death and explosions; compare with baseline; ranked optimization leads."
parent: task-013-stress-test
kind: task
status: done
---
# Stress final

- goal: final frame-time table, comparison with the baseline, ranked leads.
- agent: cpp
- depends_on: task-013-01-baseline, task-016-04-tests-integration
- inputs: [[stress-test-m1]], [[perf-risks]], task-013-01 result.
- expected_output: S0 (same steps), S2 (N robots with both weapons firing continuously), S7 (N kamikazes and barrels exploding at once through the explosion queue) results in [[stress-test-m1]] (Final section); comparison; ranked leads with expected gain.
- done_when: the -game runs completed and the tables exist with real numbers; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: 124750
- runs: 1
- milestone: M1
- result: Stress modes peace, fire, burst (rr.Stress.Run … [mode]; FireHealth), max frame and kill/barrel/explosion counts; 2 new Stress tests. S0 now 0.4 to 2.3 ms slower than baseline (about 1.4 ms fixed cost from new systems), 100 → 9.1 / 11.1 ms, 400 → 13.6 / 17.5, ceiling about 600 (avg 16.8). S2 both weapons: 100 → 9.7 / 11.8, 400 → 15.8 / 20.1, ceiling about 450–500. S7 one-frame deaths: max frame +0.23 ms per robot (100 → 29.5 ms max), long chains raise render p99 (200 → 23.4). Single 45–50 ms hitches every 61 s, probably GC (trace needed). M1 cap 100 safe: about 40 % margin on avg, 50 % on p99. Nine ranked leads in perf-risks; nothing optimized. New node stress-final-m1; stress-test-m1, perf-risks, sys-stress, sys-automation-tests. Commit d9ff38c, about 125k tokens.

## Relations
- [[task-013-stress-test]] — parent global task.
