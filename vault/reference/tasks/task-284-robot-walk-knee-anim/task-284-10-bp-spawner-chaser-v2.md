---
id: task-284-10-bp-spawner-chaser-v2
description: "Spawner uses BP_ChaserV2 (bp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: todo
---
# Spawner uses BP_ChaserV2

- goal: In BP_EnemySpawner WeightedEnemyClasses, replace BP_Chaser_C by BP_ChaserV2_C with the same weight and MaxAlive (and the EnemyClass fallback) (provisional, owner 2026-10-22); record the restore step (set BP_Chaser_C back) in the doc.
- agent: bp
- depends_on: task-284-09-bp-kamikaze-anim
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-enemy-spawner (vault/reference/tech/blueprints/world/bp-enemy-spawner.md), sys-spawner (vault/reference/tech/systems/enemies/sys-spawner.md); files: none
- expected_output: BP_EnemySpawner saved, bp-enemy-spawner doc updated with before, after, restore
- done_when: entries read back, compiles without error, the placed L_ArenaM1 instance has no override, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
