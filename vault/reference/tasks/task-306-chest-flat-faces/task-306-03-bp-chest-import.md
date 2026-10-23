---
id: task-306-03-bp-chest-import
description: "Import the chest parts into BP_BonusChest (bp): bonus chest mesh with flat faces, step 03."
parent: task-306-chest-flat-faces
kind: task
status: todo
---
# Import the chest parts into BP_BonusChest

- goal: Import chest_body, chest_lid and chest_latch with the texture (material from the Meshy maps); in BP_BonusChest replace the engine cube by the body on the inherited Mesh, add static mesh components Lid and Latch at their pivots (NoCollision, no dynamic shadow), LockLight slot keeps the texture material until task-308; remove the GlowAmount calls of OnHackProgress and OnRewardGranted (default Q5); record component, slot and pivot data in bp-bonus-chest (task-308 and task-309 drive them by these names).
- agent: bp
- depends_on: task-306-02-chest-cut
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md); files: Art/Source/chest/clean/
- expected_output: meshes, texture and material under /Game/RogueRigs/; BP_BonusChest updated; doc node bp-bonus-chest updated
- done_when: compiles without error; meshes, components, pivots and slots read back; footprint 2 x 2 kept; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-306-chest-flat-faces]] — parent global task.
