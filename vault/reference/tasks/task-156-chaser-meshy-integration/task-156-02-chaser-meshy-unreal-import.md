---
id: task-156-02-chaser-meshy-unreal-import
description: "Import the game-ready Meshy chaser with its texture and material, and swap it into BP_Chaser and BP_ShieldChaser."
parent: task-156-chaser-meshy-integration
kind: task
status: todo
---
# Chaser Meshy: Unreal import and Blueprint swap

- goal: Import the game-ready Meshy chaser with its texture and material, and swap it into BP_Chaser and BP_ShieldChaser.
- agent: bp
- depends_on: task-156-01-chaser-meshy-integration
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In /Game/RogueRigs/Art/Chaser/: SM_Chaser_Meshy (from Art/Meshes/chaser-meshy/chaser_meshy_game.fbx, import scale 1, no auto collision, no material or texture auto-import, one slot), T_Chaser_Meshy_BaseColor (2048 px, sRGB, default compression, from chaser_meshy_fixed_basecolor.png), M_Chaser_Meshy (lit: Base Color = the texture, Roughness 0.6, Metallic 0, Emissive = Base Color x red mask x GlowAmount where red mask = saturate((R - max(G, B)) x 4) so only the red eye and red parts glow; scalar parameter GlowAmount default 25 like the old eye); BP_Chaser and BP_ShieldChaser BodyMesh = SM_Chaser_Meshy with OverrideMaterials [M_Chaser_Meshy] (set the array to [] first, see unreal-mcp-quirks), relative location (0, 0, -62), scale 1, EyeMesh stays hidden; the shield shell of BP_ShieldChaser scaled so that it encloses the new mesh bounds (read them back; 74 x 35 x 123 cm expected); the old SM_Chaser and its four materials are kept in the project; docs bp-chaser and bp-shield-chaser updated
- done_when: Both Blueprints compile and save; values read back (mesh path, material, location, scale, bounds); the docs updated; no C++ change; the editor is left open; the owner is told how to see it (open L_ArenaM1 and play, rr.Stress.Spawn 10 for a crowd) and that the look is lit by the scene: roughness 0.6 and no normal map
- rules: save only the named assets; no SlateInspector, no Ctrl+S, no UI automation; never CaptureEditorImage; commit by name with message `task-156-02-chaser-meshy-unreal-import: <summary>`
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-156-chaser-meshy-integration]] — parent global task.
