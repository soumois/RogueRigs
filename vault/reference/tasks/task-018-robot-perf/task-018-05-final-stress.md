---
id: task-018-05-final-stress
description: "Rerun S0, S2, S7 after task-018 changes; compare with stress-final-m1; new ceiling and remaining leads."
parent: task-018-robot-perf
kind: task
status: done
---
# Final stress after optimization

- goal: final numbers after optimization.
- agent: cpp
- depends_on: task-018-04-hitch
- inputs: [[perf-stress-final-m1]], [[perf-risks]], [[sys-stress]], task-018 results.
- expected_output: S0 (50–600, plus higher counts until the ceiling), S2 (100–600), S7 (50–200) in a new node `stress-opt-m1` with the comparison; remaining leads in [[perf-risks]].
- done_when: the -game runs completed with real numbers; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: opus
- tokens: input 38, cache write 162433, cache read 538382, output 16541
- runs: 1
- milestone: M1
- result: Editor -game, same procedure as stress-final-m1: S0 400 13.6 → 9.8 ms frame (game 7.9), 600 16.8 → 10.1, ceiling about 600 → 1500 (1600: 16.8 avg / 26.9 p99); S2 400 15.8 → 12.3, ceiling about 450–500 → 700; S7 200 game max 47.5 → 41.9, draw p99 23.2 → 18.1, 0.20 ms per robot dying at once. Staged Development: S0 ceiling about 1600 (24.9 p99), S2 about 900, no frame over 31 ms. Below about 600 robots the frame is render-bound (draw 8–9 ms, GPU 7–9 ms): next lead is rendering (Lumen, virtual shadows, ray tracing). New nodes stress-opt-m1 and perf-leads (leads split out of perf-risks). Commit b1d3aa9, about 56k tokens.

## Relations
- [[task-018-robot-perf]] — parent global task.
