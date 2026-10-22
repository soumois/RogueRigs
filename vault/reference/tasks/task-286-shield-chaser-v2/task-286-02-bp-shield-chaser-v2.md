---
id: task-286-02-bp-shield-chaser-v2
description: "BP_ShieldChaserV2 with its materials (bp)."
parent: task-286-shield-chaser-v2
kind: task
status: in-progress
---
# BP_ShieldChaserV2 with its materials

- goal: Import the 286-01 mesh and anim texture into /Game/RogueRigs/Art/ShieldChaserV2/ (same build and texture settings as v1), create new instances for body, plain body, shield, emblem and hidden (no v1 asset modified), the eye instances MI_RobotEye_ShieldChaserV2 and _ShieldChaserV2Plain (same RunRate and ClipOffset as their body), and BP_ShieldChaserV2 duplicated from BP_ShieldChaser with the new mesh and materials in its OnShieldBroken swaps; new doc node bp-shield-chaser-v2.
- agent: bp
- depends_on: task-286-01-shield-fit-bake
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: SM_ShieldChaserV2_Meshy_Anim.fbx, T_ShieldChaserV2Anim.exr, report of 286-01
- expected_output: assets in /Game/RogueRigs/Art/ShieldChaserV2/, BP_ShieldChaserV2 compiled and saved, doc node written
- done_when: git diff shows no v1 asset changed, UV2 on RENDER_DATA clean, instance parameters and OnShieldBroken read back, compiles without error; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-286-shield-chaser-v2]] — parent global task.
