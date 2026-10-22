---
id: task-284-06-bp-heavy-anim
description: "BP_Heavy plays the walk (bp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: todo
---
# BP_Heavy plays the walk

- goal: Import Art/Meshes/heavy-meshy/heavy_meshy_parts.fbx as SM_Heavy_Meshy_Anim and T_HeavyAnim (TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp) into /Game/RogueRigs/Art/Heavy/; build settings Generate Lightmap UVs off, full precision UVs, LightMapCoordinateIndex 1, bounds extended to cover the clip; MI_Heavy_Body and MI_RobotEye_Heavy get Animated on with AnimTex T_HeavyAnim; set the mesh on the BodyMesh of BP_Heavy.
- agent: bp
- depends_on: task-284-05-cpp-anim-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/heavy-meshy/heavy_meshy_parts.fbx, T_HeavyAnim.exr, Art/Meshes/heavy-meshy/heavy_meshy_anim_report.json
- expected_output: BP_Heavy compiled and saved, doc node bp-heavy updated; chaser-meshy-pipeline (Unreal section) updated with before, after, revert
- done_when: RENDER_DATA UV channel 2 read back clean on every corner, build settings and texture settings read back, eye RunRate and ClipOffset equal to the body instance, compiles without error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
