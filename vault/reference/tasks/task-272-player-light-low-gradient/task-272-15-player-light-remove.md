---
id: task-272-15-player-light-remove
description: "The PlayerLight point light removed from BP_Miner, its last values documented for revert."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: sonnet
- tokens: input 38, cache write 45190, cache read 646639, output 413
- runs: 1
- milestone: M1
- result: Commit caa159c8. PlayerLight removed from BP_Miner (last values Z -15, radius 3000, 400 cd, 255/194/158, sourceRadius 20, shadows on, parent CollisionCylinder: revert recipe in bp-miner); no graph reference; no level override. No light casts shadows now; bBlocksCastShadows stays on without visible effect. Docs bp-miner, lights-dark-ambient, lights-no-shadows, perf-risks updated.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
