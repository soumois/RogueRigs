---
id: bp-chaser-v2
description: "BP_ChaserV2, duplicate of BP_Chaser with the animated Meshy v2 mesh SM_ChaserV2_Meshy_Anim; open for its mesh, instances, RunRate 1.5 and revert."
parent: blueprints-enemies-index
kind: doc
---
# BP_ChaserV2

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_ChaserV2
- parent_class: ARRChaser (/Script/RogueRigs.RRChaser), same as [[bp-chaser]]
- purpose: the chaser v2 robot (task-284-08, 2026-10-22, provisional, owner 2026-10-22); visuals only, behaviour in C++ ([[sys-enemies]]). Not spawned yet: the spawner still uses BP_Chaser (task-284-10).
- components: duplicate of BP_Chaser (`AssetTools.duplicate`), so inherited components, EyeMesh (hidden), debris and puff settings are identical (DebrisSystem NS_RobotDebris, pool 80, 13 DebrisPieces; DeathPuffSystem NS_RobotDeathPuff; XPPickupClass BP_PickupXP_C; StatsRowName Chaser). BodyMesh: StaticMesh /Game/RogueRigs/Art/ChaserV2/SM_ChaserV2_Meshy_Anim, relative location (0, 0, -62) (unchanged from BP_Chaser: v2 geometry is also z 0 to 123, so the mesh origin sits on the capsule bottom), OverrideMaterials emptied (slots body = MI_ChaserV2_Body, robot_eye = MI_RobotEye_ChaserV2 carry the materials), shadow flags as BP_Chaser. The component keeps the name BodyMesh (C++ writes Custom Primitive Data on it).
- exposed_defaults: as BP_Chaser (C++ defaults kept).
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Art/ChaserV2/ SM_ChaserV2_Meshy_Anim (imported by MCP from Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx: 6156 render triangles, geometry x -31.15 to 24.00, y -48.80 to 48.91, z 0 to 123, bounds extension 60 cm each side, no collision, Nanite off; build settings Generate Lightmap UVs off, full precision UVs on, LightMapCoordinateIndex 1) and T_ChaserV2Anim (20 x 31, TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp). Instances MI_ChaserV2_Body and MI_RobotEye_ChaserV2: Animated on, AnimTex T_ChaserV2Anim, RunRate 1.5, ClipOffset 0 ([[bp-robot-meshy-material]]).
- animation: clip 0 run (rows 0..15, baked for RunRate 1.5), clip 1 attack (rows 16..30, not scaled by RunRate), same contract as v1 ([[sys-enemies-chaser]]).
- verified: UV channel 2 (PartId) read back on RENDER_DATA: body 6098 triangles on bins 0.05 to 0.95, robot_eye 58 triangles at 0.15, off_bin 0, mixed 0; the render data had a generated lightmap in a fourth UV set before the build settings fix, 3 UV sets after (uv2 u 0.05 to 0.95, 10 distinct values). Compiled: LogBlueprint logged only the Compiling lines, no error or warning. Saved. Properties read back.
- gotcha: the MCP import leaves Generate Lightmap UVs on; the settings were fixed by a second headless UnrealEditor-Cmd process rewriting the saved .uasset, so never save SM_ChaserV2_Meshy_Anim again from the open editor (stale copy).
- revert: delete BP_ChaserV2, SM_ChaserV2_Meshy_Anim and T_ChaserV2Anim, and restore the two instances (Animated off, RunRate 0.7, AnimTex default) with `git checkout 98a046e6 -- Content/RogueRigs/Materials/Robots/MI_ChaserV2_Body.uasset Content/RogueRigs/Materials/Robots/MI_RobotEye_ChaserV2.uasset`. BP_Chaser and all v1 assets untouched.
- not_verified: PIE, render, run and attack look, bounds sufficiency in game, spawn.
- owner_agent: agent-bp

## Relations
- [[blueprints-enemies-index]] — parent index.
- [[bp-chaser]] — the v1 Blueprint this duplicates, kept for restore.
- [[bp-robot-meshy-material]] — the body and eye instances and RunRate.
- [[robot-parts-bake-pipeline]] — bake and Unreal recipe of this mesh.
- [[sys-enemies]] — the C++ class.
