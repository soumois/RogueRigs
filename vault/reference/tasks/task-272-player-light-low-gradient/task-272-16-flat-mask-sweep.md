---
id: task-272-16-flat-mask-sweep
description: "Runtime sweep of the flat sun level and the shader mask parameters, scored on the DRGS ring profile."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Flat light and mask sweep

- goal: Runtime sweep of the flat sun level and the shader mask parameters, scored on the DRGS ring profile.
- agent: cpp
- depends_on: task-272-15-player-light-remove
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-player (vault/reference/tech/systems/core/sys-player.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), Saved/LookTune/capture_cmd.txt
- expected_output: Saved/LookTune/r8/: flat sun levels and mask settings on A, D, G, H with ring profiles against DRGS; chosen defaults.
- done_when: Frames exist, measured; values read back; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
