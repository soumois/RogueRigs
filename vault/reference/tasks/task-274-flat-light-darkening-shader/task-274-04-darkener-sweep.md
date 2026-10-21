---
id: task-274-04-darkener-sweep
description: "-game sweep (round 1) of FarBrightness, Radius, Exponent and exposure offset, ring profile scored against DRGS; emissives compared with the final_272_flat frames."
parent: task-274-flat-light-darkening-shader
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
