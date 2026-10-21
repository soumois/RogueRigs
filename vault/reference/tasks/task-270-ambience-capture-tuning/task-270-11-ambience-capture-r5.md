---
id: task-270-11-ambience-capture-r5
description: "Round 5 captures at the same frames with a small radius sweep, judged by the Orchestrator; final side by sides for the owner."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
