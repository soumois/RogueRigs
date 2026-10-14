---
id: chaser-meshy-pipeline
description: "How the animated chaser is made from a Meshy mesh: Blender scripts, bake format, Unreal assets and parameters. Open before changing its look or animation."
parent: pipelines-index
kind: doc
---
# Chaser Meshy pipeline (owner decision 2026-10-14, tasks 154 to 165)

Replaces the Blender part-by-part model ([[chaser-model-pipeline]], abandoned). Spec: [[enemy-chaser]].

## Chain (repo root, headless Blender `$BLENDER_EXE`, stage files in a short path: long Windows paths fail)
1. Views: `vault/assets/chaser-meshy-{front,left,back,right}.png` (GPT Image 2, prompts in `Art/Source/chaser-meshy/prompts/`) are the Meshy multi-view input. Meshy output (optimised by Meshy, 3115 tris, one fused mesh, painted 2048 px texture): `Art/Source/chaser-meshy/meshy-optimised/`.
2. `Tools/art/blender/meshy_texture_fix.py`: shoulder-to-torso joints repainted dark (face classification), hood logo = company emblem redrawn yellow on black. Geometry and UV unchanged. Result `chaser_meshy_fixed_basecolor.png` (the intermediate FBX is not kept: rerun the script).
3. `meshy_game_ready.py`: 123 cm, centimetres, front +X, origin at the floor, LightMap UV1 -> `chaser_meshy_game.fbx`.
4. `meshy_cut_parts.py`: 10 rigid parts (names and parents as in [[chaser-model-pipeline]]), pivots at joints found on the mesh, caps on cuts (3263 tris), part index stored twice: vertex colour PartIndex and UV channel 2 `PartId` = (index + 0.5) / 10 -> `chaser_meshy_parts.fbx`. `meshy_cut_check.py` checks the cut.
5. `meshy_bake_anim.py`: run (16 frames loop, 30 fps) and attack (15 frames, wind-up 3), baked to `T_ChaserAnim.exr` (20 x 31 RGBA half float; frame = row from the top; texel 2p quaternion xyzw, 2p+1 translation cm; new = R v + t); report `anim_report.json`. Option `--thigh-deg` (35 now; 18 stops foot sliding).
All FBX exports use `FBX_SCALE_UNITS` (UnitScaleFactor 100): `FBX_SCALE_NONE` imported 100 times too large.

## Unreal (`/Game/RogueRigs/Art/Chaser/`)
- `SM_Chaser_Meshy_Anim`: combined parts, extended bounds 40 cm, build settings Generate Lightmap UVs OFF, LightMapCoordinateIndex 1, full precision UVs (the engine had written a lightmap into channel 2 and shredded the part index). Check channel 2 on RENDER_DATA after any reimport.
- `T_ChaserAnim`: TC_HDR, no mips, Nearest, sRGB off. `M_Chaser_Meshy_Anim`: Custom node `ChaserAnimWPO` (Texture.Load); parameters ClipId (Custom Primitive Data 0), AnimStart (CPD 1), RunRate 0.7 (run clip only), GlowAmount 25 (emissive of red texels only). Y is mirrored inside the node for the Unreal import.
- `ARRChaser` writes the CPD on events ([[sys-enemies-chaser]]); `BP_Chaser` and `BP_ShieldChaser` BodyMesh use the mesh and material, no shadow ([[bp-chaser]]). Unused: SM_Chaser, SM_Chaser_Meshy, M_Chaser_Meshy.
- Reimport an existing mesh: [[tool-unreal-reimport]].

## Relations
- [[pipelines-index]] — parent index.
- [[enemy-chaser]] — design and clips.
- [[chaser-model-pipeline]] — the abandoned Blender route.
- [[asset-pipeline]] — general rules.
- [[orchestrator-lessons-2026-10-14]] — pitfalls met on the way.
