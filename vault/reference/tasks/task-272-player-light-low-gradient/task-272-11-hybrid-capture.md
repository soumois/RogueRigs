---
id: task-272-11-hybrid-capture
description: "Final captures of the applied variant at the same frames, measured, with side by sides against the physical-light variant and DRGS."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Hybrid final captures

- goal: Final captures of the applied variant at the same frames, measured, with side by sides against the physical-light variant and DRGS.
- agent: cpp
- depends_on: task-272-10-hybrid-apply
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), Saved/LookTune/capture_cmd.txt
- expected_output: Saved/LookTune/r9/ frames A to H, measured; side by sides final_272_hybrid_*.png (r7 physical light | r9 hybrid | DRGS).
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
