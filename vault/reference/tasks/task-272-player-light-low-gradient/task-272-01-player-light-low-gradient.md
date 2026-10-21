---
id: task-272-01-player-light-low-gradient
description: "Baseline frames G and H and a runtime sweep of player light height, radius, intensity, fill light, sun and exposure."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: opus
- tokens: input 108, cache write 189396, cache read 5753662, output 17683
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r6/ (A-F baseline, new G_wallside at BugItGo 7939.5 7537.5, H_before/H_after dig at 8006.5 7336.5), sweeps V0-V10 and W0-W6 on D, G, H, B, measures r6/sheets/meas1-2.txt, commands in capture_cmd.txt round 6. Runtime height works: RelativeLocation by object path then teleport + Walk + recreate. Seen: low key light (Z -15, R 1800-2400, 250-400 cd) removes the disc edge, walls shade the floor behind (G 19 vs 30), a dug wall lets a beam through (H), no crescent on rock tops; core saturates at the feet (about 245); shadowless fill light (cheat halo) greys rock tops and fills wall shadows: rejected. Cost ProfileGPU static: frame 7.84 -> 8.57 ms (D), 7.98 -> 8.71 ms (B), shadow depths 0.62 -> 1.16 ms; moving case not measured. No code, no commit by the agent.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
