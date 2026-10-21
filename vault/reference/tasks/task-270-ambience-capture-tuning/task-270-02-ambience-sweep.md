---
id: task-270-02-ambience-sweep
description: "Second runtime sweep of light combinations on the same frames, to pick the values bp applies."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
---
# Ambience sweep, round 1

- goal: Second runtime sweep of light combinations on the same frames, to pick the values bp applies.
- agent: cpp
- depends_on: task-270-01-ambience-capture-tuning
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Saved/LookTune/capture_cmd.txt
- expected_output: Variant frames in Saved/LookTune/r1/, each named with its values, measured (pool radius, core and corner luminance).
- done_when: Frames exist and are measured; no game process left; Orchestrator picked values.
- editor_state: closed
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
