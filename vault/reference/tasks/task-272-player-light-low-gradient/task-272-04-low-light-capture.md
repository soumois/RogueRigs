---
id: task-272-04-low-light-capture
description: "Final -game captures A to H at the applied values, measured, compared with r5 and DRGS; final side by sides for the owner."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Low light final captures

- goal: Final -game captures A to H at the applied values, measured and compared with r5 and the DRGS reference; final side by sides for the owner.
- agent: cpp
- depends_on: task-272-02-player-light-low-apply, task-272-03-ambient-level-apply
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-risks (vault/reference/tech/performance/perf-risks.md), Saved/LookTune/capture_cmd.txt
- expected_output: Frames in Saved/LookTune/r7/, measured; GPU cost of the light stated; Saved/LookTune/final_272_<frame>.png (r5 | r7).
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: opus
- tokens: input 52, cache write 76822, cache read 1257953, output 6530
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r7/ (A-H at the asset values, read back), profiles r7/sheets/profiles.txt, side by sides Saved/LookTune/final_272_{A_idle,B_wall,D_robots_xp,E_xp,G_wallside,H_dig}.png. A rings 208 167 105 60 37 25 27 18 20 14 11 12 (score 0.213 vs r5 0.604; DRGS 164 140 84 57 39 30 22 17 12 10 9 9); D 0.204. Seen: no disc edge; G floor behind the wall black (shadow 1 vs open 13, r5 values 12 vs 11); H after the dig two warm wedges through the corridor; no rock-top crescent; core cream-white and clipped (p99 232), miner top dark; rock tops dark violet-grey, floor warm brown to black at edges; robots readable, XP less saturated. Cost frame D: shadows on vs off +2.5 to 3.0 ms (ShadowDepths 1.95 vs 0.11, Lights 1.35 vs 0.24), moving = static. Physical-light variant done; hybrid variant requested next.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
