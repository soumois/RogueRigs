---
id: task-273-04-floor-capture
description: "-game captures of the floor before and after the new material at the task-272 frames, ring luminance measured against the DRGS profile."
parent: task-273-floor-texture
kind: task
status: in-progress
---
# Floor capture

- goal: Side by side before and after `-game` captures of the floor at the same frames as task-272, with the ring luminance (far floor and 350 uu) measured against the DRGS ring profile.
- agent: cpp
- depends_on: task-273-03-floor-material
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), the task-272 capture procedure given in the delegation
- expected_output: `Saved/LookTune/floor_273_*.png` before, after and side by side; numbers in the result.
- done_when: Captures exist; far floor and 350 uu values reported; no light, exposure or post-process changed; images sent to the owner by the Orchestrator.
- editor_state: closed
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-273-floor-texture]] — parent global task.
