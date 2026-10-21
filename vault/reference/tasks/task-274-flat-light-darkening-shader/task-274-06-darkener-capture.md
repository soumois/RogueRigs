---
id: task-274-06-darkener-capture
description: "Final -game frames A, B, D, E, F, H with Enable 1 and Enable 0, centre-versus-unmasked proof numbers, DRGS profile, side by sides final_274_*.png."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
---
# Darkener final captures

- goal: Final -game frames A, B, D, E, F, H with Enable 1 and Enable 0, centre-versus-unmasked proof numbers, DRGS profile, side by sides final_274_*.png.
- agent: cpp
- depends_on: task-274-05-darkener-defaults
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), Saved/LookTune/capture_cmd.txt, Saved/LookTune/r9/
- expected_output: Saved/LookTune/r11/ frames and profiles; Saved/LookTune/final_274_*.png.
- done_when: Frames exist and are measured; proof numbers reported; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 40, cache write 54448, cache read 707897, output 1250
- runs: 1
- milestone: M1
- result: Round 11 (Saved/LookTune/r11) at asset values (no override; Enable 0 frames via bDriveTunables in a second run). Scores A 0.277, B 0.710, D 0.224, E 0.230, F 0.414, H 0.550 (r9 A 0.223, D 0.123). A rings 167 140 99 54 26 16 11 14 15 15 10 7; D 166 150 103 70 36 17 11 14 12 12 8 7. Centre r<120 (on) vs whole Enable 0 frame, sRGB luma / linear: A -6.0/-13.8, B -2.7/-7.4, D -6.6/-15.6, E -5.0/-12.2, F -12.5/-25.9, H +6.1/+10.3 percent; hue 282-286 vs 283-284 deg. Centre disc on vs same disc off: -3 to -8 percent linear. p99 162-238 (rock tops, robots). XP cyan kept (top-20 luma 197 vs 204), far XP dim; crystals hue kept, luma 150 vs 168; robots white-cream instead of gold. Differs: rock tops near the miner much brighter than the darker task-273 floor (white in B, F, H); centre violet-magenta (sun colour) instead of DRGS warm; tail past ring 4 steeper than DRGS. Side by sides Saved/LookTune/final_274_{A_idle,B_wall,D_robots_xp,E_xp,F_deposits,H_dig}.png.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
