---
id: task-274-07-far-floor-sweep
description: "Capture round 3: FarBrightness and Exponent swept on floor-only ring profiles so the far floor reaches 9-12/255; chosen values captured on the six frames with Enable 1 and 0; side by sides final_274_*.png rebuilt."
parent: task-274-flat-light-darkening-shader
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
