---
id: task-272-13-mask-writer-position
description: "The mask writer pushes only the player position by default, so MPC_PlayerLight is the single place where the owner tunes the effect."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
---
# Mask writer position only

- goal: The mask writer pushes only the player position by default, so MPC_PlayerLight is the single place where the owner tunes the effect.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-player (vault/reference/tech/systems/core/sys-player.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: URRPlayerLightMaskComponent writes only PlayerPosition by default; renamed tunables pushed only when bDriveTunables (console sweeps); sys-player updated.
- done_when: Build exits 0, tests pass, doc updated, lint clean, committed by path.
- editor_state: closed
- token_ceiling: 50k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
