---
id: task-270-11-ambience-capture-r5
description: "Round 5 captures at the same frames with a small radius sweep, judged by the Orchestrator; final side by sides for the owner."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience round 5 captures

- goal: Round 5 captures at the same frames with a small radius sweep, judged by the Orchestrator; final side by sides for the owner.
- agent: cpp
- depends_on: task-270-10-ambience-radius-r5
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), Saved/LookTune/capture_cmd.txt
- expected_output: Frames in Saved/LookTune/r5/, measured; final side by sides r0 / r5.
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 28, cache write 70181, cache read 542026, output 1044
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r5/ and final side by sides Saved/LookTune/final_270_{A_idle,B_wall,D_robots_xp,E_xp}.png (r0 | r5); radius sweep 620/640/660 on D. Seen at 640: warm amber-cream pool core L 155, half radius 210 px (0.22 of width, target 0.18), violet rock tops readable (L 39), chasers golden yellow and identifiable, XP small cyan. Still off: lit rock tops a cream-white crescent (L 242), floor outside the pool very dark (L 10-12 vs about 40 in DRGS), miner top and drone white-hot. 640 kept.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
