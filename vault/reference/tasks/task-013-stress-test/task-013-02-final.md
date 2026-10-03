---
id: task-013-02-final
description: "Rerun the stress procedure after task-016: S0, S2 continuous fire, S7 mass death and explosions; compare with baseline; ranked optimization leads."
parent: task-013-stress-test
kind: task
status: todo
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
- milestone: M1
- result: open

## Relations
- [[task-013-stress-test]] — parent global task.
