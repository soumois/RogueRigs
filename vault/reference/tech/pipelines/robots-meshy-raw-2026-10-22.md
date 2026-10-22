---
id: robots-meshy-raw-2026-10-22
description: "Raw Meshy meshes of heavy, kamikaze, shooter and chaser v2 given by the owner 2026-10-22: files, measures, texture defects. Open before processing them."
parent: pipelines-index
kind: doc
---
# Raw Meshy meshes of four robots (owner, 2026-10-22)

Made by the owner in Meshy from the sheets of [[art-prompts-robots-meshy]]; Meshy optimisation done by Meshy. Not processed yet: tasks 281 to 285.

## Files (copied from the owner's zips, renamed short)
| Robot | Folder | Meshy name |
|---|---|---|
| heavy | `Art/Source/heavy-meshy/meshy-raw/heavy_meshy_raw*` | Hazard_Core_Mech 1022010937 (`Meshy_AI_Gros.zip`) |
| kamikaze | `Art/Source/kamikaze-meshy/meshy-raw/kamikaze_meshy_raw*` | Ember_Sentinel 1022010852 (`Meshy_AI_Kamikaze.zip`) |
| shooter | `Art/Source/shooter-meshy/meshy-raw/shooter_meshy_raw*` | Lime_Sentinel 1022010829 (`Meshy_AI_Tireur.zip`) |
| chaser v2 | `Art/Source/chaser-meshy/meshy-v2-raw/chaser_v2_meshy_raw*` | Yellow_Sentinel 1022010953 (`Meshy_AI_Chasseur.zip`) |

Each folder: `<prefix>_raw.fbx`, `_raw_basecolor.png`, `_raw_metallic.png`, `_raw_metallic_roughness.png`, `_raw_normal.png`, `_raw_roughness.png` (byte copies of the zip files).

## Measured (Blender 5.2 headless import, main session 2026-10-22)
Each FBX: one mesh `output_unwrapped`, one UV, one material; base colour and normal 2048 px, metallic and roughness 4096 px; all about 1.9 m tall (Meshy normalisation).
- chaser v2: 5120 tris, 2500 verts, 1.51 x 0.85 x 1.90 m
- heavy: 10361 tris, 5136 verts, 1.90 x 0.94 x 1.69 m
- kamikaze: 10172 tris, 5087 verts, 1.84 x 1.25 x 1.90 m
- shooter: 7263 tris, 3600 verts, 1.41 x 0.89 x 1.90 m

## Defects reported by the owner
- Surfaces that should be flat are bulged by the Meshy optimisation (all four).
- `vault/assets/heavy-meshy-defect-back.webp` (heavy, back): arm, lower-leg and back panels look translucent, glass-like, with grey camouflage blotches; the sheet `heavy-turnaround.webp` shows flat opaque blue panels.
- `vault/assets/kamikaze-meshy-defect-leg.png` (kamikaze, leg): the yellow tip has dark smeared blotchy edges; the sheet `kamikaze-turnaround.webp` shows a clean yellow tip on dark gunmetal.
- Whether a defect lies in the base colour or in the metallic and roughness maps is not checked yet.

## Relations
- [[pipelines-index]] — parent index.
- [[art-prompts-robots-meshy]] — the sheets these meshes come from.
- [[chaser-meshy-pipeline]] — the chain applied to chaser v1, reused for these meshes.
