---
id: task-284-09-bp-kamikaze-anim
description: "BP_Kamikaze plays the leg sway (bp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: todo
---
# BP_Kamikaze plays the leg sway

- goal: Import Art/Meshes/kamikaze-meshy/kamikaze_meshy_parts.fbx as SM_Kamikaze_Meshy_Anim and T_KamikazeAnim (TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp) into /Game/RogueRigs/Art/Kamikaze/; build settings Generate Lightmap UVs off, full precision UVs, LightMapCoordinateIndex 1, bounds extended to cover the clip; MI_Kamikaze_Body and MI_RobotEye_Kamikaze get Animated on with AnimTex T_KamikazeAnim; set the mesh on the BodyMesh of BP_Kamikaze.
- agent: bp
- depends_on: task-284-05-cpp-anim-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-kamikaze (vault/reference/tech/blueprints/enemies/bp-kamikaze.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/kamikaze-meshy/kamikaze_meshy_parts.fbx, T_KamikazeAnim.exr, Art/Meshes/kamikaze-meshy/kamikaze_meshy_anim_report.json
- expected_output: BP_Kamikaze compiled and saved, doc node bp-kamikaze updated; chaser-meshy-pipeline (Unreal section) updated with before, after, revert
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
