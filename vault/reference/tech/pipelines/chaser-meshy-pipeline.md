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
6. `chaser_debris_pieces.py` (task-166-01, reads `chaser_meshy_parts.fbx` and the fixed base colour, no rerun of steps 2 to 4 needed while those files exist): 13 debris pieces for the Niagara death burst in `Art/Source/chaser-meshy/debris/` (`SM_ChaserDebris_*.fbx` + a copy of the base colour texture next to them + `debris_pieces.json`). Fixed order = Niagara mesh index: Torso (head-torso + pelvis), UpperArm_L/R, Forearm_L/R, Arm_L/R (upper arm + forearm), Thigh_L/R, LowerLeg_L/R, Leg_L/R (thigh + lower leg). Rest pose, cut caps kept, pivot at the bounds centre, UV0 only (LightMap, PartId and PartIndex dropped), 5578 tris over the 13 pieces (the merged ones repeat their parts). `debris_pieces.json` gives per piece the triangle count and `centre_offset_cm_unreal` = (x, -y, z) of the bounds centre relative to the floor origin: Unreal import keeps X and Z and mirrors Y, so the robot's left arm and leg have negative Y. Left and right pieces come from a slightly asymmetric Meshy mesh: offsets are mirrored in sign, within about 0.3 cm in magnitude, and triangle counts differ a little.
All FBX exports use `FBX_SCALE_UNITS` (UnitScaleFactor 100): `FBX_SCALE_NONE` imported 100 times too large.

## Unreal (`/Game/RogueRigs/Art/Chaser/`)
- `SM_Chaser_Meshy_Anim`: combined parts, extended bounds 40 cm, build settings Generate Lightmap UVs OFF, LightMapCoordinateIndex 1, full precision UVs (the engine had written a lightmap into channel 2 and shredded the part index). Check channel 2 on RENDER_DATA after any reimport.
- `T_ChaserAnim`: TC_HDR, no mips, Nearest, sRGB off. `M_Chaser_Meshy_Anim`: Custom node `ChaserAnimWPO` (Texture.Load); parameters ClipId (Custom Primitive Data 0), AnimStart (CPD 1), RunRate 0.7 (run clip only), GlowAmount 25 (emissive of red texels only). Y is mirrored inside the node for the Unreal import.
- `ARRChaser` writes the CPD on events ([[sys-enemies-chaser]]); `BP_Chaser` and `BP_ShieldChaser` BodyMesh use the mesh and material, no shadow ([[bp-chaser]]). Unused: SM_Chaser, SM_Chaser_Meshy, M_Chaser_Meshy.
- `/Game/RogueRigs/Art/Chaser/Debris/` (task-166-03): the 13 `SM_ChaserDebris_*` imported from `Art/Source/chaser-meshy/debris/` with `StaticMeshTools.import_file` (centimetres, no x100; no collision, Nanite off, one LOD, one slot `meshy_chaser`, no material or texture imported). Bounds X x Y x Z in cm: Torso 35 x 36 x 51, UpperArm 13 x 15.5 x 21-23, Forearm 19.7 x 13 x 41, Arm 20 x 21.5 x 57, Thigh 13 x 10.4 x 29.5, LowerLeg 27 x 15 x 60-63, Leg 27 x 16 x 84. `M_ChaserDebris`: opaque, default lit, one-sided, base colour only from the existing `T_Chaser_Meshy_BaseColor` (the texture of `M_Chaser_Meshy_Anim`, no duplicate imported), no WPO, no emissive, `bUsedWithNiagaraMeshParticles` on; assigned to the slot of every piece. Import generates a lightmap UV (LightMapCoordinateIndex 1): MCP does not expose the build setting, harmless for particles.
- Reimport an existing mesh: [[tool-unreal-reimport]].

## Relations
- [[pipelines-index]] — parent index.
- [[enemy-chaser]] — design and clips.
- [[chaser-model-pipeline]] — the abandoned Blender route.
- [[asset-pipeline]] — general rules.
- [[art-3d-lessons]] — pitfalls met on the way.
