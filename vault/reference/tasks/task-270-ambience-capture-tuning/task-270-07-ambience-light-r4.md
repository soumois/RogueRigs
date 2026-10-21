---
id: task-270-07-ambience-light-r4
description: "PlayerLight attenuation radius and intensity raised so the floor pool is wider and brighter at height 450, set in the editor and documented."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
---
# Ambience round 4 light

- goal: PlayerLight attenuation radius and intensity raised so the floor pool is wider and brighter at height 450, set in the editor and documented.
- agent: bp
- depends_on: task-270-06-ambience-capture-r3
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: PlayerLight radius and intensity set as listed in the delegation; docs updated with before and after.
- done_when: Values read back; compiled and saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 50k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
