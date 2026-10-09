---
id: task-066-02-perf-check
description: "In-game -game measurement of the worker-thread flow field on the owner's normal PC profile, compared with task-063."
parent: task-066-async-flow-field
kind: task
status: in-progress
---
# Flow field worker: in-game measurement

- goal: confirm in game that the worker-thread flow field keeps robots reacting within 1–2 frames and frees the game thread.
- agent: cpp
- depends_on: task-066-01-async-flow-field
- inputs: stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), sys-flow-field (vault/reference/tech/systems/sys-flow-field.md)
- expected_output: on the owner's normal PC profile (background apps open, Armoury Crate Performance mode, 60 Hz external monitor; owner 2026-10-09: this is a normal player profile, nothing to close): -game stress at 67 uu with the same scenarios and commands as task-063 (S0 and S2 at 100 and 400 robots, chain of 30, dig), reporting avg / p99 / max frame ms, game-thread ms and flow-field goal age (frames) where the stress logs it; one comparison run with rr.FlowField.Async 0 on S0 400 and dig 400; table next to task-063; no code change, no new test; results in stress-grid-cells.
- done_when: table reported and written in stress-grid-cells; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-066-async-flow-field]] — parent global task.
