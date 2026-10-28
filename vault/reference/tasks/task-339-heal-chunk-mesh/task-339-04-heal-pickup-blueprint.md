---
id: task-339-04-heal-pickup-blueprint
description: "BP: BP_PickupHeal shows one of SM_HealChunk_01..03 per drop instead of the red cube, slot defaults, no shadow; other defaults unchanged."
parent: task-339-heal-chunk-mesh
kind: task
status: in-progress
---
# Heal pickup chunks: BP_PickupHeal wiring

- goal: in `/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal`, as BP_PickupOre in task-189-05 ([[bp-pickup-ore]]): `PickupMesh` StaticMesh = `/Game/RogueRigs/Art/HealChunk/SM_HealChunk_01`, RelativeScale3D (1, 1, 1) (was the engine cube at 0.3); OverrideMaterials cleared (the slot 0 override MI_GB_PickupHeal goes, so each variant uses its own slot defaults; asset MI_GB_PickupHeal kept); `MeshVariants` = [SM_HealChunk_01, _02, _03] (one drawn per life, [[sys-pickups-lifecycle]] look per life); `RestOrientation` Full kept; CastShadow false like the ore chunk and the XP nut (global `gdd_check` Q3, provisional). Unchanged: PickupType Heal, pop and physics defaults, OnCollected spawning BP_HealRing, no trail and no OnMagnetStart handler (no trail requested), `HealPickupClass` reference in BP_ArenaGrid.
- agent: bp
- depends_on: task-339-03-heal-chunk-collision
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md)
- expected_output: BP_PickupHeal saved by explicit path; change summary for the doc subtask (every default set, values read back). No doc node written (doc subtask).
- done_when: Blueprint compiles with warnings as errors; class default object read back: mesh SM_HealChunk_01, scale 1, OverrideMaterials empty, MeshVariants the three chunks, RestOrientation Full, CastShadow false, pop and physics defaults unchanged; BP_ArenaGrid still has `HealPickupClass` = BP_PickupHeal_C; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T02:38:38Z
- waiting_for: none
- result: open

## Relations
- [[task-339-heal-chunk-mesh]] — parent global task.
