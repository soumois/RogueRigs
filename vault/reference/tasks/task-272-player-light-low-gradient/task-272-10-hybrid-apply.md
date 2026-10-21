---
id: task-272-10-hybrid-apply
description: "The winning variant's values set on BP_Miner (PlayerLight and light mask defaults), read back and documented."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Hybrid values applied

- goal: The winning variant's values set on BP_Miner (PlayerLight and light mask defaults), read back and documented.
- agent: bp
- depends_on: task-272-09-hybrid-sweep
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: BP_Miner light and mask defaults as listed in the delegation (or the mask removed if the physical light wins); docs updated with before, after and revert.
- done_when: Values read back; compiled and saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
