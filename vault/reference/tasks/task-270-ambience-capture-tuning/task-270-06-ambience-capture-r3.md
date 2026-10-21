---
id: task-270-06-ambience-capture-r3
description: "Round 3 captures at the same frames, with a runtime intensity and colour mini-sweep, judged by the Orchestrator."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
