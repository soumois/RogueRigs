---
id: task-274-04-darkener-sweep
description: "-game sweep (round 1) of FarBrightness, Radius, Exponent and exposure offset, ring profile scored against DRGS; emissives compared with the final_272_flat frames."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
---
# Darkener sweep

- goal: -game sweep (round 1) of FarBrightness, Radius, Exponent and exposure offset, ring profile scored against DRGS; emissives compared with the final_272_flat frames.
- agent: cpp
- depends_on: task-274-03-exposure-raise
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), sys-player (vault/reference/tech/systems/core/sys-player.md), Saved/LookTune/capture_cmd.txt, Saved/LookTune/r9/
- expected_output: Saved/LookTune/r10/ frames, profiles, chosen values.
- done_when: Frames exist and are measured; chosen values stated with reasons; emissive comparison stated; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: opus
- tokens: input 50, cache write 68207, cache read 1073081, output 10652
- runs: 1
- milestone: M1
- result: Round 10 (Saved/LookTune/r10): asset values A 0.461 / D 0.254 (centre 141, falls too fast; centre r<120 vs Enable 0 frame -25 percent linear). Pass 1 one-knob, pass 2 Q1-Q10. Chosen Q4: FarBrightness 0.002, Radius 260, Exponent 4, exposure +0.5 (-3.4 EV100): A 0.230 rings 176 147 104 60 32 19 12 9 12 12 12 10 p99 172; D 0.264 p99 226 (r9 0.223 / 0.123). Criterion Q4 centre vs whole Enable 0 frame: A -2.7 percent sRGB luma (-8 linear), D -5.5 (-13 linear), hue 286 vs 284 deg. Emissives: XP same brightness (bluer), crystals same, robots and miner white-lilac instead of gold (tint gone); no material change. Differs: hue violet-magenta (sun colour 191/158/255) instead of DRGS warm; far rings 12-17 from bright rock tops.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
