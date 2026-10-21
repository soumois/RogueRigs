---
id: task-270-09-ambience-capture-r4
description: "Round 4 captures at the same frames with the round 4 values, judged by the Orchestrator and sent to the owner."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
