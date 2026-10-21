---
id: task-270-06-ambience-capture-r3
description: "Round 3 captures at the same frames, with a runtime intensity and colour mini-sweep, judged by the Orchestrator."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience round 3 captures

- goal: Round 3 captures at the same frames, with a runtime intensity and colour mini-sweep, judged by the Orchestrator.
- agent: cpp
- depends_on: task-270-05-ambience-apply-r3
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), Saved/LookTune/capture_cmd.txt
- expected_output: Frames in Saved/LookTune/r3/ named as in r2, measured, side by side with r2.
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 50, cache write 91013, cache read 1376932, output 2326
- runs: 1
- milestone: M1
- result: Frames Saved/LookTune/r3/ (base + sweep s1-s5 on D and B, sbs r2/r3). Live values confirmed. Seen: chasers more golden but tops still clipped (612 px; already 355 at r0 with 120 cd, so the cause is the bright chaser albedo against a dark floor, not the light height), rock crescent still L 235, pool smaller and dimmer (core L 99, half radius 150 px); intensity 1800-3000 and colours 205/150 or 175/110 do not fix the clipping; sun 1.0 only darkens unlit areas. The lime cylinder is the Shooter, green-yellow by design (MI_GB_Shooter). Miner light set by object path in setnopec to spare the kamikaze lights.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
