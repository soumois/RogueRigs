---
id: task-272-01-player-light-low-gradient
description: "Baseline frames G and H and a runtime sweep of player light height, radius, intensity, fill light, sun and exposure."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
---
# Low light runtime sweep

- goal: Baseline captures of the new frames G and H and a runtime sweep of the player light height, radius, intensity, fill light, sun and exposure, judged by the Orchestrator.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), perf-risks (vault/reference/tech/performance/perf-risks.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: Saved/LookTune/r6/: baseline frames at current values for the new frames G (miner beside a rock wall, open floor behind it) and H (dig, wall falling, before/after), then a runtime sweep (light height, radius, intensity, shadowless fill light, sun, exposure) on frames B, D, G, H with values read back, luminance profiles measured, commands appended to Saved/LookTune/capture_cmd.txt.
- done_when: Frames exist, are described and measured; values read back; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
