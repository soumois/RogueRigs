---
id: task-283-07-bp-heavy-mesh
description: "BP_Heavy uses the Meshy mesh (bp)."
parent: task-283-robot-mesh-integration
kind: task
status: done
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
- model: sonnet
- tokens: input 68, cache write 94288, cache read 2119740, output 958
- runs: 1
- milestone: M2
- result: SM_Heavy_Meshy imported (bounds match report, Y mirrored, Nanite off, 1 LOD), slots body=MI_Heavy_Body, robot_eye=MI_RobotEye_Heavy; BodyMesh z -124; grey-box StripeA-D, EyeLeft/Right, LogoPlate removed (no test names them); OnTelegraphStart/OnRecover swap slot 1 to new MI_RobotEye_Heavy_Glow (GlowAmount 60, EyeColor (1,0.35,0), provisional) and back, pitch 14 kept; compiles clean; bp-heavy updated.

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
