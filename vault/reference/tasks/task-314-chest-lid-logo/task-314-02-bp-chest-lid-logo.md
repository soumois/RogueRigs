---
id: task-314-02-bp-chest-lid-logo
description: "Reimport the chest lid with its Logo slot and give that slot a flat yellow material (bp): yellow logo on the chest lid, step 02."
parent: task-314-chest-lid-logo
kind: task
status: todo
---
# Reimport the lid and colour the Logo slot

- goal: Reimport SM_Chest_Lid from the new chest_lid.fbx in place (same asset, scale 1); create M_ChestLogo (flat company yellow of color-code, roughness about 0.45, no emissive) and assign it to the Logo slot, M_Chest stays on the other slot; check the Lid component of BP_BonusChest keeps its relative transform; record the Logo slot in bp-bonus-chest.
- agent: bp
- depends_on: task-314-01-chest-lid-logo
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), color-code (vault/reference/gdd/art-audio/color-code.md); files: Art/Source/chest/clean/chest_lid.fbx
- expected_output: SM_Chest_Lid reimported, M_ChestLogo under /Game/RogueRigs/Art/Chest/, doc node bp-bonus-chest updated
- done_when: compiles without error; slots and materials read back; Lid transform unchanged; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-314-chest-lid-logo]] — parent global task.
