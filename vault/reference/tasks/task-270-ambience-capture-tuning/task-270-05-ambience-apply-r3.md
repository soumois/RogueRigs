---
id: task-270-05-ambience-apply-r3
description: "PlayerLight raised and recoloured so robots and rock tops near the miner are no longer burnt white, set in the editor and documented."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
---
# Ambience round 3 values

- goal: PlayerLight raised and recoloured so robots and rock tops near the miner are no longer burnt white, set in the editor and documented.
- agent: bp
- depends_on: task-270-04-ambience-after-capture
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: PlayerLight height, radius, intensity and colour set as listed in the delegation; docs updated with before and after.
- done_when: Values read back; compiled and saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
