---
id: task-284-08-bp-chaser-v2-anim
description: "BP_ChaserV2 plays the run and attack (bp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: in-progress
---
# BP_ChaserV2 plays the run and attack

- goal: Import Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx as SM_ChaserV2_Meshy_Anim and T_ChaserV2Anim (TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp) into /Game/RogueRigs/Art/ChaserV2/; build settings Generate Lightmap UVs off, full precision UVs, LightMapCoordinateIndex 1, bounds extended to cover the clip; MI_ChaserV2_Body and MI_RobotEye_ChaserV2 get Animated on with AnimTex T_ChaserV2Anim; create BP_ChaserV2 in /Game/RogueRigs/Blueprints/Enemies/ as a duplicate of BP_Chaser (same parent class ARRChaser, same debris and puff settings) and set the mesh on its BodyMesh; BP_Chaser and every chaser v1 asset stay untouched (owner 2026-10-22: v1 kept for restore); new doc node bp-chaser-v2.
- agent: bp
- depends_on: task-284-05-cpp-anim-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx, T_ChaserV2Anim.exr, Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_anim_report.json
- expected_output: BP_ChaserV2 compiled and saved, doc node bp-chaser-v2 created; chaser-meshy-pipeline (Unreal section) updated with before, after, revert
- done_when: git diff shows no chaser v1 asset changed; RENDER_DATA UV channel 2 read back clean on every corner, build settings and texture settings read back, eye RunRate and ClipOffset equal to the body instance, compiles without error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
