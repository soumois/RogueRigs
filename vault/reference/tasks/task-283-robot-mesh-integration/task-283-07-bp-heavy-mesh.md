---
id: task-283-07-bp-heavy-mesh
description: "BP_Heavy uses the Meshy mesh (bp)."
parent: task-283-robot-mesh-integration
kind: task
status: todo
---
# BP_Heavy uses the Meshy mesh

- goal: Import Art/Meshes/heavy-meshy/heavy_meshy_game.fbx as SM_Heavy_Meshy into /Game/RogueRigs/Art/Heavy/ (centimetres, no x100, no collision, Nanite off, one LOD, no material or texture imported), assign MI_Heavy_Body and MI_RobotEye_Heavy to its slots, set it on the BodyMesh of BP_Heavy, sized to the capsule. Remove the grey-box StripeA-D, EyeLeft, EyeRight and LogoPlate; OnTelegraphStart and OnRecover swap the robot_eye slot material (glow) instead of the eye spheres and stripes; BodyMesh pitch kept until task-285. Check the C++ tests that name components (sys-automation-tests) before deleting any.
- agent: bp
- depends_on: task-283-06-robot-body-material
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/heavy-meshy/heavy_meshy_game.fbx and its report
- expected_output: BP_Heavy compiled and saved; doc node bp-heavy updated (before, after, revert)
- done_when: mesh bounds read back match the report (Y mirrored), slots and materials read back, BP_Heavy compiles without LogBlueprint error, events read back, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
