---
id: task-272-07-light-mask-writer
description: "C++ writer that pushes the miner position and the mask tunables into MPC_PlayerLight each tick, without per-frame allocation."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
---
# Light mask writer

- goal: C++ writer that pushes the miner position and the mask tunables into MPC_PlayerLight each tick, without per-frame allocation.
- agent: cpp
- depends_on: task-272-06-light-mask-material
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-player (vault/reference/tech/systems/core/sys-player.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ component on ARRMiner writing MPC_PlayerLight each tick from tunable properties; sys-player doc updated.
- done_when: Build exits 0 with the editor closed; tests still pass; doc updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
