---
id: task-272-04-low-light-capture
description: "Final -game captures A to H at the applied values, measured, compared with r5 and DRGS; final side by sides for the owner."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
