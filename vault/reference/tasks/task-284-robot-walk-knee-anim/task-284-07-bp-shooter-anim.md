---
id: task-284-07-bp-shooter-anim
description: "BP_Shooter plays the walk (bp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: done
---
# BP_Shooter plays the walk

- goal: Import Art/Meshes/shooter-meshy/shooter_meshy_parts.fbx as SM_Shooter_Meshy_Anim and T_ShooterAnim (TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp) into /Game/RogueRigs/Art/Shooter/; build settings Generate Lightmap UVs off, full precision UVs, LightMapCoordinateIndex 1, bounds extended to cover the clip; MI_Shooter_Body and MI_RobotEye_Shooter get Animated on with AnimTex T_ShooterAnim; set the mesh on the BodyMesh of BP_Shooter.
- agent: bp
- depends_on: task-284-05-cpp-anim-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/shooter-meshy/shooter_meshy_parts.fbx, T_ShooterAnim.exr, Art/Meshes/shooter-meshy/shooter_meshy_anim_report.json
- expected_output: BP_Shooter compiled and saved, doc node bp-shooter updated; chaser-meshy-pipeline (Unreal section) updated with before, after, revert
- done_when: RENDER_DATA UV channel 2 read back clean on every corner, build settings and texture settings read back, eye RunRate and ClipOffset equal to the body instance, compiles without error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 48, cache write 61676, cache read 1098475, output 13555
- runs: 1
- milestone: M2
- result: SM_Shooter_Meshy_Anim + T_ShooterAnim imported; RENDER_DATA UV2 clean; build/texture settings read back; bounds +60 cm; MI_Shooter_Body, MI_RobotEye_Shooter Animated on, RunRate 1.25; BP_Shooter BodyMesh = anim mesh, compiled and saved; docs updated.

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
