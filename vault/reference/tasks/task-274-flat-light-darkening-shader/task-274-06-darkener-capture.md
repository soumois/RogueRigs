---
id: task-274-06-darkener-capture
description: "Final -game frames A, B, D, E, F, H with Enable 1 and Enable 0, centre-versus-unmasked proof numbers, DRGS profile, side by sides final_274_*.png."
parent: task-274-flat-light-darkening-shader
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
