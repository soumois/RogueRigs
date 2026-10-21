---
id: task-270-04-ambience-after-capture
description: "After captures at the frames of task-270-01 with the applied values, judged by the Orchestrator against the reference."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
---
# Ambience after captures

- goal: After captures at the frames of task-270-01 with the applied values, judged by the Orchestrator against the reference.
- agent: cpp
- depends_on: task-270-03-ambience-apply
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Saved/LookTune/capture_cmd.txt
- expected_output: Frames in Saved/LookTune/r2/ named as in r0, measured, side by side with r0.
- done_when: Frames exist and are measured; no game process left; Orchestrator judged them and sent them to the owner.
- editor_state: closed
- token_ceiling: 50k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
