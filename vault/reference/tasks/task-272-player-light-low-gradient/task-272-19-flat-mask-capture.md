---
id: task-272-19-flat-mask-capture
description: "Final captures of the flat light plus shader mask at the asset values, measured against DRGS, side by sides for the owner."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Flat mask final captures

- goal: Final captures of the flat light plus shader mask at the asset values, measured against DRGS, side by sides for the owner.
- agent: cpp
- depends_on: task-272-17-mask-defaults, task-272-18-flat-level-light
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), Saved/LookTune/capture_cmd.txt
- expected_output: Saved/LookTune/r9/ frames A to H measured; side by sides Saved/LookTune/final_272_flat_*.png (r5 | r7 | r9, DRGS for A).
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
