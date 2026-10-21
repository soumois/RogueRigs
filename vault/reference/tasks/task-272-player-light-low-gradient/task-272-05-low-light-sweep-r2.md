---
id: task-272-05-low-light-sweep-r2
description: "Second runtime sweep of the low player light matched to the measured DRGS ring profile: softer core, darker far ambient, DRGS colour."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Low light sweep round 2

- goal: Second runtime sweep of the low player light matched to the measured DRGS ring profile: softer core, darker far ambient, DRGS colour.
- agent: cpp
- depends_on: task-272-01-player-light-low-gradient
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), Saved/LookTune/capture_cmd.txt, Saved/LookTune/r6/macros/
- expected_output: Saved/LookTune/r6b/: variants on frames A, D, G, H with values read back and ring profiles (80 px rings, method of the main session's DRGS analysis) compared with the DRGS target.
- done_when: Frames exist, are described and measured against the target; values read back; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: opus
- tokens: input 60, cache write 90135, cache read 1926367, output 2174
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r6b/ (A, D, G, H; pass 1 N1-N12, pass 2 M1-M10), ring profiles r6b/sheets/profiles.txt (method validated on r5 A_idle: 149 114 61 23 14 14 14 19 24 27 21 20), sheet r6b/sheets/final_best.png. Chosen M10: Z -15, radius 3000, 400 cd inverse squared, sourceRadius 20, colour 255/194/158, sun 0.3, exposure -0.5 EV. Score (mean abs log ratio to DRGS rings) A 0.208, D 0.184 vs V0 0.606/0.487; A rings 207 162 100 54 33 21 23 14 18 14 11 12 vs target 164 140 84 57 39 30 22 17 12 10 9 9. Seen: no disc edge, walls shade the floor behind (G shadow 1 vs open 13), warm wedges through the dug corridor (H), rock tops dark violet. Misses: core rings 25% too bright and whitish (p99 231), rings 4-6 15-30% low, G/H edges near black (2-5). Unitless exponent falloff floods walls: rejected; fill light rejected in step 1. Cost ProfileGPU static D: frame 7.91 -> 8.69 ms, ShadowDepths 0.61 -> 1.26, Lights 0.29 -> 0.91.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
