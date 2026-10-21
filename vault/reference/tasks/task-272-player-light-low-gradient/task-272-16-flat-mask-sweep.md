---
id: task-272-16-flat-mask-sweep
description: "Runtime sweep of the flat sun level and the shader mask parameters, scored on the DRGS ring profile."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: opus
- tokens: input 74, cache write 195903, cache read 2511217, output 8973
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r8/ (centring cent16_*, flat sun S10-S40, pass 1 M1-M12, pass 2 M5/Q1-Q9 on A, D, G, H), sheets r8/sheets/best_{A,D,G,H}.png, profiles.txt. Centring OK (centroid within about 30 px of the miner at A and G). Flat sun 4.0 chosen (floor about 25, rock tops 40-50, even). Chosen Q9: Enable 1, CenterBrightness 30, FarBrightness 0.15, Radius 200 uu, Exponent 3, WarmTint (1, 0.9, 0.06), FarTint (1, 0.7, 0.55), TintStrength 1; sky 0.08, exposure 1.8. Score A 0.227, D 0.138 (r7 0.213/0.204); A rings 175 137 93 56 32 23 17 18 19 19 14 11; p99 173 (A), 197 (D); core (201,156,109), far (30,10,34). Seen: uniform non-directional brightening, smooth, no disc edge; miner and robots gold near the centre; blue barrel lavender; rock tops next to the miner glow (XY mask, no occlusion); no wall shadows. Mask pass 0.043 ms; ShadowDepths 0.03 ms now.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
