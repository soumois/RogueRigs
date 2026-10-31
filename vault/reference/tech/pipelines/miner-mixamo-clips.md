---
id: miner-mixamo-clips
description: "Owner Mixamo clips of the miner (speed, direction, in-place method, FBX export) and the three upper rifle variants. Open before importing them."
parent: pipelines-index
kind: doc
---
# Miner Mixamo clips

- Owner, 2026-10-30: "Voilà quelques animations mixamo (avec les mains déjà bien posées, plus ou moins, pour l'arme)". Route: Mixamo clips on the miner skeleton, played in place, movement from gameplay ([[task-386-miner-mixamo-animation]]).
- Originals, byte-identical (SHA-256 checked against the downloads): `Art/Source/miner-mixamo/<name>.fbx`, nine files, 30 fps, one action each, not in place.
- Script: `Tools/art/blender/miner_clips.py` (`build` then `verify`); report `Art/Source/miner-mixamo/miner_mixamo_clips.report.json`.
- Skeleton: `SmartRigArmature`, 66 bones (65 `mixamorig:*` plus `headfront`), root `mixamorig:Hips`, identical hierarchy in the ten exports. Each clip is put on a fresh copy of the miner armature, so all share its exact rest pose.

## Clips (character frame: 0 = forward, 90 = right, +-180 = back, -90 = left)
Mixamo -X is the character right, so `Strafe` moves right. Speed and travel are the Hips first to last frame before the in-place step. Last frame repeats the first (loop).

| Export | Source | Frames | Seconds | Direction deg | Speed cm/s | Travel cm | Hips bob cm |
|---|---|---|---|---|---|---|---|
| Idle | Rifle Aiming Idle | 94 | 3.10 | none | 0 | 0 | 0.33 |
| RunFwd | Rifle Run | 23 | 0.733 | 0 | 278.8 | 204.5 | 13.76 |
| WalkFwd | Walk With Rifle | 44 | 1.433 | 0 | 224.7 | 322.0 | 8.58 |
| RunBwd | Run Backwards | 17 | 0.533 | 180 | 243.7 | 130.0 | 4.99 |
| WalkBwd | Backwards Rifle Walk | 39 | 1.267 | 180 | 88.0 | 111.5 | 4.46 |
| RunLeft | Run Left | 16 | 0.500 | -90 | 427.1 | 213.5 | 8.01 |
| StrafeLeft | Strafe Left | 32 | 1.033 | -90 | 122.7 | 126.8 | 6.73 |
| RunRight | Run Right | 16 | 0.500 | 90 | 427.1 | 213.6 | 11.39 |
| StrafeRight | Strafe | 21 | 0.667 | 90 | 292.2 | 194.8 | 8.83 |

Walk With Rifle covers 322 cm in 1.43 s, likely two strides: the speed is the whole-clip value.

## In place and export
- In place (Q2): the Hips location keys are converted to the armature frame, their horizontal part is set to the first-frame value, the vertical part and every rotation are untouched. Result: Hips horizontal displacement 0.0 cm over every frame of every clip (re-read from the exports).
- Export: Blender `export_scene.fbx`, `apply_scale_options=FBX_SCALE_UNITS` (centimetres), no leaf bones, forward -Z, up Y, smoothing FACE, clips baked with no simplify, one action `A_Miner_<Name>`, armature only: `Art/Meshes/Miner/Anims/A_Miner_<Name>.fbx`. Exported frame range is shifted by one (starts at 2): the frame count is unchanged.
- `Art/Meshes/Miner/SK_Miner.fbx`: mesh plus armature, no animation, no mesh edit: 25,744 triangles, 185.5 x 46.5 x 170 cm. Textures are referenced (copied to `SK_Miner.fbm/`), not embedded.

## Upper rifle variants (task-408-02)
Owner 2026-10-31: new animations based on the idle with the right elbow raised, three proposals, the idle untouched ([[task-408-miner-right-arm-in-animation]]). Elbow only, right hand unchanged (Q7 default).
- Files: `Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V1.fbx`, `_V2.fbx`, `_V3.fbx`: 66 bones, 94 frames, 30 fps, same export settings as above. Script `Tools/art/blender/miner_upper_rifle.py` (`build`, `verify`, `render`); report `Art/Source/miner-mixamo/upper_rifle_variants.report.json`.
- Method: from A_Miner_Idle.fbx, on every frame RightArm and RightForeArm are rotated rigidly about the shoulder to hand axis (the hand head does not move) by the smallest angle that raises the RightForeArm head by E in world Z; RightHand is restored to its idle world orientation (fingers follow); keys baked on every frame.
- Measured against the idle on all 94 frames (re-read from the FBX):

| Variant | Elbow raise cm | Arm rotation about the axis deg | Hand head deviation cm | Hand orientation deviation deg | Other bones deviation | Hips horizontal cm |
|---|---|---|---|---|---|---|
| V1 | 3.00 | about 13.6 to 13.9 | 0.0001 | 0 | 0.0001 cm, 0 deg | 0 |
| V2 | 6.00 | about 25.2 to 25.7 | 0.0001 | 0 | 0.0001 cm, 0 deg | 0 |
| V3 | 9.00 | about 35.9 to 36.4 | 0.0001 | 0 | 0.0001 cm, 0 deg | 0 |

## Relations
- [[pipelines-index]] — parent index.
- [[task-386-miner-mixamo-animation]] — the global task that made them.
- [[art-prompts-miner]] — the rigged miner mesh these clips fit.
- [[miner]] — the entity they animate.
- [[bp-miner]] — imports the variants and picks the active one (UpperBodySequence).
