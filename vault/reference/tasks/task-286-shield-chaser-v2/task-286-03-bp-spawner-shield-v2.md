---
id: task-286-03-bp-spawner-shield-v2
description: "Spawner uses BP_ShieldChaserV2 (bp)."
parent: task-286-shield-chaser-v2
kind: task
status: todo
---
# Spawner uses BP_ShieldChaserV2

- goal: In BP_EnemySpawner, replace BP_ShieldChaser_C by BP_ShieldChaserV2_C with the same weight and MaxAlive (director entry) (provisional, owner 2026-10-22); record the restore step in the doc.
- agent: bp
- depends_on: task-286-02-bp-shield-chaser-v2
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-enemy-spawner (vault/reference/tech/blueprints/world/bp-enemy-spawner.md), sys-spawner (vault/reference/tech/systems/enemies/sys-spawner.md), sys-enemies-shield-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md); files: none
- expected_output: BP_EnemySpawner saved, bp-enemy-spawner doc updated with before, after, restore
- done_when: entries read back, compiles without error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-286-shield-chaser-v2]] — parent global task.
