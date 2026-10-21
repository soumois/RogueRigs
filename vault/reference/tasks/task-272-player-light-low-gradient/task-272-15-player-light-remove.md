---
id: task-272-15-player-light-remove
description: "The PlayerLight point light removed from BP_Miner, its last values documented for revert."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Player light removed

- goal: The PlayerLight point light removed from BP_Miner, its last values documented for revert.
- agent: bp
- depends_on: task-272-14-mask-owner-params
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), perf-risks (vault/reference/tech/performance/perf-risks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: PlayerLight component removed from BP_Miner; docs with the previous values for revert.
- done_when: Component gone (read back), compiled and saved, docs updated, lint clean, committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
