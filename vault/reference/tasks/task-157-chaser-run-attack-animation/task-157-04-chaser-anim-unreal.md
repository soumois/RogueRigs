---
id: task-157-04-chaser-anim-unreal
description: "Import the part mesh and the animation texture, build the animation material and swap it into BP_Chaser and BP_ShieldChaser."
parent: task-157-chaser-run-attack-animation
kind: task
status: in-progress
---
# Chaser: animated mesh and material in Unreal

- goal: Import the part mesh and the animation texture, build the animation material and swap it into BP_Chaser and BP_ShieldChaser.
- agent: bp
- depends_on: task-157-03-chaser-anim-cpp-hook
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), perf-xp-pickups-glow (vault/reference/tech/performance/perf-xp-pickups-glow.md)
- expected_output: In /Game/RogueRigs/Art/Chaser/: SM_Chaser_Meshy_Anim (one static mesh, the ten parts combined, imported from Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx with the vertex colour kept: red channel = part index / 9; one slot; scale 1; extended bounds border about 40 cm so the moving parts are never culled), T_ChaserAnim (from T_ChaserAnim.exr: 20 x 31, RGBA16F, no mips, filter Nearest, sRGB off, never stream), M_Chaser_Meshy_Anim (the look of M_Chaser_Meshy plus a World Position Offset: part index from the vertex colour red x 9, two point samples of T_ChaserAnim at the current frame and the next one lerped by the fraction, clip id and start time read from Custom Primitive Data 0 and 1 (clip frames: run starts at frame 0 with 16 frames looping, attack starts at frame 16 with 15 frames not looping and clamped at its last frame), time = Time - start at 30 frames per second, quaternion rotation of the vertex plus the translation texel, the WPO returned as the new position minus the old position; the normals follow only if it is cheap, otherwise report it); BP_Chaser and BP_ShieldChaser BodyMesh = SM_Chaser_Meshy_Anim with OverrideMaterials [M_Chaser_Meshy_Anim]; docs bp-chaser and bp-shield-chaser updated (components, assets, the Custom Primitive Data contract)
- done_when: Both Blueprints compile and save; values read back; the material compiles without errors and its instruction count and the texture format are written in the doc; the docs updated; the editor is left open; the owner is told how to see it (play L_ArenaM1, rr.Stress.Spawn 10) and that the perf cost of the animated material at 100, 200 and 400 robots is not measured yet
- rules: save only the named assets; no SlateInspector, no Ctrl+S, no UI automation; never CaptureEditorImage; MCP cannot set console variables; commit by name with message `task-157-04-chaser-anim-unreal: <summary>`
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-157-chaser-run-attack-animation]] — parent global task.
