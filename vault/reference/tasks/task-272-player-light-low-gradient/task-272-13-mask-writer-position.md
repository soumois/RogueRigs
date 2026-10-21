---
id: task-272-13-mask-writer-position
description: "The mask writer pushes only the player position by default, so MPC_PlayerLight is the single place where the owner tunes the effect."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: opus
- tokens: input 30, cache write 40845, cache read 427679, output 7471
- runs: 1
- milestone: M1
- result: Commits b1224a62 (code), 678276b8 (sys-player). URRPlayerLightMaskComponent writes only PlayerPosition by default; bDriveTunables (false) pushes Enable 1, CenterBrightness 3, FarBrightness 0.4, Radius 225 uu, Exponent 2, TintStrength 1, WarmTint (1, 0.76, 0.62), FarTint (0.85, 0.55, 0.9) for console sweeps; old tunables removed. Build exit 0, 22/22 tests. Missing MPC names are a silent no-op.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
