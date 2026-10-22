---
id: task-283-09-bp-shooter-mesh
description: "BP_Shooter uses the Meshy mesh (bp)."
parent: task-283-robot-mesh-integration
kind: task
status: done
---
# BP_Shooter uses the Meshy mesh

- goal: Import Art/Meshes/shooter-meshy/shooter_meshy_game.fbx as SM_Shooter_Meshy into /Game/RogueRigs/Art/Shooter/ (centimetres, no x100, no collision, Nanite off, one LOD, no material or texture imported), assign MI_Shooter_Body and MI_RobotEye_Shooter to its slots, set it on the BodyMesh of BP_Shooter, sized to the capsule. Remove the grey-box EyeLeft, EyeRight, LogoPlate, UpperArm, ElbowJoint and Cannon; MuzzlePoint moved to the reported cannon tip. Check the C++ tests that name components (sys-automation-tests) before deleting any.
- agent: bp
- depends_on: task-283-06-robot-body-material
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/shooter-meshy/shooter_meshy_game.fbx and its report
- expected_output: BP_Shooter compiled and saved; doc node bp-shooter updated (before, after, revert)
- done_when: mesh bounds read back match the report (Y mirrored), slots and materials read back, BP_Shooter compiles without LogBlueprint error, events read back, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 40, cache write 69879, cache read 937825, output 734
- runs: 1
- milestone: M2
- result: SM_Shooter_Meshy imported (bounds match, Y mirrored, 7263 tris), slots body/robot_eye; BodyMesh z -88; EyeLeft/Right, LogoPlate, UpperArm, ElbowJoint, Cannon removed; MuzzlePoint (45.7, 50.4, -17.4); compiles clean; bp-shooter updated.

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
