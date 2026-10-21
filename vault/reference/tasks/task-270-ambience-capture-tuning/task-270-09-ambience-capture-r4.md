---
id: task-270-09-ambience-capture-r4
description: "Round 4 captures at the same frames with the round 4 values, judged by the Orchestrator and sent to the owner."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience round 4 captures

- goal: Round 4 captures at the same frames with the round 4 values, judged by the Orchestrator and sent to the owner.
- agent: cpp
- depends_on: task-270-07-ambience-light-r4, task-270-08-chaser-brightness-r4
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), Saved/LookTune/capture_cmd.txt
- expected_output: Frames in Saved/LookTune/r4/ named as in r3, measured, side by side with r0 and r3.
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 36, cache write 93440, cache read 808727, output 2259
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r4/ with sbs3_* (r0 | r2 | r4) and a SpecularScale/exposure sweep. Seen: chasers now read yellow-orange (identifiable), miner top and drone still white-hot; pool warm and bright (core L 177) but too wide again (half radius 260 px, 0.27 of width); rock crescent still cream-white (L 244) and larger; SpecularScale 0 changes neither the crescent nor the white tops (diffuse, not specular); exposure -0.3 darkens everything without fixing the crescent. Next: radius about 640.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
