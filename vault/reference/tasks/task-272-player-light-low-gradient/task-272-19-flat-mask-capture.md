---
id: task-272-19-flat-mask-capture
description: "Final captures of the flat light plus shader mask at the asset values, measured against DRGS, side by sides for the owner."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Flat mask final captures

- goal: Final captures of the flat light plus shader mask at the asset values, measured against DRGS, side by sides for the owner.
- agent: cpp
- depends_on: task-272-17-mask-defaults, task-272-18-flat-level-light
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), Saved/LookTune/capture_cmd.txt
- expected_output: Saved/LookTune/r9/ frames A to H measured; side by sides Saved/LookTune/final_272_flat_*.png (r5 | r7 | r9, DRGS for A).
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 34, cache write 53631, cache read 604912, output 1774
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r9/ (A-H at the asset values, read back: no PlayerLight, sun 4.0, sky 0.08, exposure 1.8, blendable on, bDriveTunables false), profiles r9/sheets/profiles.txt; side by sides Saved/LookTune/final_272_flat_{A_idle,B_wall,D_robots_xp,E_xp,G_wallside,H_dig}.png. A rings 175 137 93 56 32 23 17 18 19 19 14 11 (score 0.223, r7 0.213), D 0.123 (r7 0.204); p99 173-197, no clipped core; core gold-yellow (201,156,109), far plum (29,10,33). Seen: smooth round centre brightening, uniform on floor, robots and miner, no disc edge; rock tops beside the miner glow pale lilac (XY mask, no occlusion); no shadows; chasers near the miner pale peach, low contrast; XP and crystals readable. Cost frame D 8.17 ms, mask pass 0.043 ms, ShadowDepths 0.03 ms.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
