---
id: task-270-10-ambience-radius-r5
description: "PlayerLight attenuation radius reduced so the floor pool is about 0.2 of the screen width again, set in the editor and documented."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
---
# Ambience round 5 radius

- goal: PlayerLight attenuation radius reduced so the floor pool is about 0.2 of the screen width again, set in the editor and documented.
- agent: bp
- depends_on: task-270-09-ambience-capture-r4
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: PlayerLight radius set as listed in the delegation; docs updated with before and after.
- done_when: Value read back; compiled and saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
