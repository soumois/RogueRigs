---
id: task-018-05-final-stress
description: "Rerun S0, S2, S7 after task-018 changes; compare with stress-final-m1; new ceiling and remaining leads."
parent: task-018-robot-perf
kind: task
status: todo
---
# Final stress after optimization

- goal: final numbers after optimization.
- agent: cpp
- depends_on: task-018-04-hitch
- inputs: [[stress-final-m1]], [[perf-risks]], [[sys-stress]], task-018 results.
- expected_output: S0 (50–600, plus higher counts until the ceiling), S2 (100–600), S7 (50–200) in a new node `stress-opt-m1` with the comparison; remaining leads in [[perf-risks]].
- done_when: the -game runs completed with real numbers; lint clean.
- editor_state: closed
- token_ceiling: 70000
- milestone: M1
- result: open

## Relations
- [[task-018-robot-perf]] — parent global task.
