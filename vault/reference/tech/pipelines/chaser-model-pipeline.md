---
id: chaser-model-pipeline
description: "Chaser Blender model split by part: files, commands, contract per part, part preview, how an agent edits one part. Open before editing a chaser part."
parent: pipelines-index
kind: doc
---
# Chaser model pipeline

The chaser ([[enemy-chaser]]) is ten rigid objects built by six part modules, so six agents can work in parallel. Route: [[blender-asset-route]]; validation: [[mesh-fidelity-loop]]. Geometry of the split equals the monolithic model of commit bc01b6b (git tag `backup-chaser-monolithic`).

## Files (`Tools/art/blender/chaser/`)
- `common.py`: palette, four materials (slot order yellow_body, gunmetal, eye_red, logo_plate), logo texture, `Part` builder (loft, abox, prism, secs, oct_y, profile), `finish` (normals, UV0 decal only, UV1 LightMap, flat shading), reports, and the joint contract.
- `part_headtorso.py`, `part_pelvis.py`, `part_upperarm.py`, `part_forearmhand.py`, `part_thigh.py`, `part_shinfoot.py`: each exposes `build(side)` (None for HeadTorso and Pelvis, "L" or "R" otherwise; R is the mirror of L through the same code) and returns the object, pivot at the joint, plus `PREVIEW` (close-up window).
- `assemble.py`: builds the ten objects, parents them, writes the six part reports, merges them into `Art/Meshes/chaser/SM_Chaser_parts.report.json` (keeps `fidelity` and `refinement`), exports `SM_Chaser_parts.fbx`. `Tools/art/blender/chaser_model.py` is a wrapper with the same command line.
- `part_preview.py`: one part alone.

## Commands
- Assemble: `"$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser/assemble.py -- --out Art/Meshes/chaser/SM_Chaser_parts.fbx [--render-dir Art/Meshes/chaser]`
- Part: `"$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser/part_preview.py -- --only <headtorso|pelvis|upperarm|forearmhand|thigh|shinfoot> [--side L|R]`

## Contract per part (do not change)
| part | pivot | parent | tris today |
|---|---|---|---|
| Pelvis | hip height, centre line | none | 88 |
| HeadTorso | waist | Pelvis | 168 |
| UpperArm_L/R | shoulder | HeadTorso | 72 each |
| ForearmHand_L/R | elbow | UpperArm | 396 each |
| Thigh_L/R | hip | Pelvis | 132 each |
| ShinFoot_L/R | knee | Thigh | 300 each |

Joint positions, slot order and parents are in `common.py` and in the `joints_cm`, `hierarchy` and `parts` entries of the whole report. The attachment section is the vertex slab within 1 cm of the pivot height, written in each part report. Total budget 3000 tris; units cm, forward +X, left +Y, flat shading, no rig.

## Part preview
Renders the part alone, orthographic front, side and back at 11.7 px per cm on light grey, plus one close-up (the window is `PREVIEW["close"]` of the module), into `Art/Meshes/chaser/parts/<part>_<view>.png` (not committed), and writes `parts/<part>.report.json` (tris, bounds, pivot, attachment, lightmap check; an existing `fidelity` object is kept).

## Editing one part
Edit only your `part_<part>.py` and its report. Do not touch `common.py`, `assemble.py`, other parts, the pivots or the parent: report a needed change instead. Parts depend on each other only through the contract. After editing, run the part preview, judge against the crops, then run assemble before the assembly step.

## Relations
- [[pipelines-index]] — parent index.
- [[blender-asset-route]] — general Blender route this asset follows.
- [[mesh-fidelity-loop]] — loop used per part and on the assembly.
- [[enemy-chaser]] — design of the chaser this model shows.
