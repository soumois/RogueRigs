---
id: task-274-07-far-floor-sweep
description: "Capture round 3: FarBrightness and Exponent swept on floor-only ring profiles so the far floor reaches 9-12/255; chosen values captured on the six frames with Enable 1 and 0; side by sides final_274_*.png rebuilt."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
---
# Far floor sweep

- goal: Capture round 3: FarBrightness and Exponent swept on floor-only ring profiles so the far floor reaches 9-12/255; chosen values captured on the six frames with Enable 1 and 0; side by sides final_274_*.png rebuilt.
- agent: cpp
- depends_on: task-274-06-darkener-capture
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), Saved/LookTune/capture_cmd.txt, Saved/LookTune/r11/
- expected_output: Saved/LookTune/r12/ frames and profiles; Saved/LookTune/final_274_*.png rebuilt.
- done_when: Far floor (floor pixels) about 9-12/255 and 350 uu about 39/255 measured; criterion numbers; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: opus
- tokens: input 86, cache write 134775, cache read 1858185, output 3340
- runs: 2
- milestone: M1
- result: Round 12 (Saved/LookTune/r12, finals r12/x4): reused 33 interrupted-run sweep frames, measured floor-only rings; 2 game launches (finals at 0.025/220/3, rejected: centre disc on vs off -8 to -14 percent linear; then 0.03/250/4). Chosen FarBrightness 0.03, Radius 250, Exponent 4 (runtime override only, assets unchanged). Floor only 350 uu / far: A 40.0/10.0, B 33.5/9.6, D 40.2/10.2, E 40.4/10.2, F 38.5/10.0, H 48.1/10.0 (target 39 / 9-12). Floor rings A 154 140 98 55 29 19 15 13 11 10 9 7. Centre r<120 on vs whole Enable 0, sRGB/linear: A -5.3/-12.5, B -3.6/-8.5, D -6.3/-14.3, E -3.6/-9.2, F -12.1/-24.9, H -2.7/-8.9 percent; disc on vs off -5.2 to -9.5 percent linear; hue 284-287 vs 283-284 deg; floor p99 r<240 172-213. XP top-20 luma 195-196 (r11 196-197), crystals 154 (150). Side effect: far rock tops lift to about 75/255 (r11 about 23). Side by sides Saved/LookTune/final_274_{A_idle,B_wall,D_robots_xp,E_xp,F_deposits,H_dig}.png rebuilt (r9 | r12 on | r12 Enable 0). No game process left. Owner 2026-10-21: values 0.03 / 250 / 4 validated, brighter far rock tops (about 75/255) accepted.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
