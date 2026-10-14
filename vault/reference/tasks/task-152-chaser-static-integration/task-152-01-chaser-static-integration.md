---
id: task-152-01-chaser-static-integration
description: "Put the modelled chaser in the game as a static mesh (no animation yet) so the owner sees it in play."
parent: task-152-chaser-static-integration
kind: task
status: in-progress
---
# Chaser: static in-game integration

- goal: Put the modelled chaser in the game as a static mesh (no animation yet) so the owner sees it in play.
- agent: bp
- depends_on: none
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: SM_Chaser (one static mesh, the ten parts combined, cm, from Art/Meshes/chaser/SM_Chaser_parts.fbx) in /Game/RogueRigs/Art/Chaser/ with four slots yellow_body, gunmetal, eye_red, logo_plate; the logo texture T_ChaserLogo imported next to it; four materials: yellow (instance of M_GreyBox, colour #F5BA20 in linear), gunmetal (M_GreyBox instance, #3A3A3A in linear), eye (instance of M_GreyBoxGlow, red, strong emissive GlowAmount 25, no pulse needed), logo (a small lit material with the texture as base colour)
- done_when: The mesh reads back with bounds x -12.4 to 32.5, y about plus or minus 43.3, z 0 to 123.0 and the four slots in order; the materials exist and read back; the assets saved and committed; nothing in BP_Chaser changed yet
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-152-chaser-static-integration]] — parent global task.
