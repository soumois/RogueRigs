---
id: task-283-08-bp-kamikaze-mesh
description: "BP_Kamikaze uses the Meshy mesh (bp)."
parent: task-283-robot-mesh-integration
kind: task
status: done
---
# BP_Kamikaze uses the Meshy mesh

- goal: Import Art/Meshes/kamikaze-meshy/kamikaze_meshy_game.fbx as SM_Kamikaze_Meshy into /Game/RogueRigs/Art/Kamikaze/ (centimetres, no x100, no collision, Nanite off, one LOD, no material or texture imported), assign MI_Kamikaze_Body and MI_RobotEye_Kamikaze to its slots, set it on the BodyMesh of BP_Kamikaze, sized to the capsule. Remove the grey-box EyeMesh; BodyMesh = body mesh, CoreMesh = the separate core mesh at its reported offset (the C++ pulse and growth unchanged); CoreLight kept, moved to the core centre. Check the C++ tests that name components (sys-automation-tests) before deleting any.
- agent: bp
- depends_on: task-283-06-robot-body-material
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-kamikaze (vault/reference/tech/blueprints/enemies/bp-kamikaze.md), sys-enemies-kamikaze (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-kamikaze.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/kamikaze-meshy/kamikaze_meshy_game.fbx, Art/Meshes/kamikaze-meshy/kamikaze_meshy_core.fbx and its report
- expected_output: BP_Kamikaze compiled and saved; doc node bp-kamikaze updated (before, after, revert)
- done_when: mesh bounds read back match the report (Y mirrored), slots and materials read back, BP_Kamikaze compiles without LogBlueprint error, events read back, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 44, cache write 78493, cache read 1181072, output 813
- runs: 1
- milestone: M2
- result: SM_Kamikaze_Meshy + SM_Kamikaze_Core_Meshy imported (bounds match, Y mirrored); slots body/robot_eye and core=MI_Kamikaze_Body; BodyMesh z -40 (VisualRoot hover 40), CoreMesh (-4.1, 0.6, 30.4), CoreLight at core centre; EyeMesh removed; compiles clean; bp-kamikaze updated. Core 102 cm at scale 1, armed growth 1.8 may exceed the body.

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
