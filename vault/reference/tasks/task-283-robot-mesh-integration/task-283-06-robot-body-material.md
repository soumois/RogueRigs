---
id: task-283-06-robot-body-material
description: "Robot body material and instances (bp)."
parent: task-283-robot-mesh-integration
kind: task
status: in-progress
---
# Robot body material and instances

- goal: Import the four cleaned base colour textures to their Unreal folders, create one master M_RobotMeshy (base colour texture parameter, base colour only, constant roughness 0.6 and metallic 0, no Meshy normal, metallic or roughness map (provisional, owner 2026-10-22), static switch Animated off by default driving MF_RobotAnimWPO with AnimTex, ClipOffset, RunRate) with instances MI_Heavy_Body, MI_Kamikaze_Body, MI_Shooter_Body, MI_ChaserV2_Body, and the eye instances MI_RobotEye_Heavy, _Kamikaze, _Shooter, _ChaserV2 of M_RobotEye (UseEyeTexture on, Animated off).
- agent: bp
- depends_on: task-283-05-game-ready-chaser-v2
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md); files: the four *_clean_basecolor.png of task-282, /Game/RogueRigs/Materials/Robots/
- expected_output: assets in /Game/RogueRigs/Materials/Robots/ and the four textures in /Game/RogueRigs/Art/{Heavy,Kamikaze,Shooter,ChaserV2}/; new doc node bp-robot-meshy-material in vault/reference/tech/blueprints/enemies/ linked from blueprints-enemies-index
- done_when: every instance read back (texture, switches, scalars), master compiles without error, saved, doc node written; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
