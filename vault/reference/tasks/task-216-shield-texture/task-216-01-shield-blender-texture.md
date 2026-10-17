---
id: task-216-01-shield-blender-texture
description: "3D agent: give the raw Meshy shield flat materials and a stripe texture in headless Blender, scale it, export FBX, report and fidelity renders."
parent: task-216-shield-texture
kind: task
status: in-progress
---
# Shield: Blender materials, stripes and export

- goal: `SM_ChaserShield.fbx` textured like the four validated views, geometry from Meshy unchanged (no remodel, no decimation).
- agent: agent-3d (Blender route on an existing Meshy mesh: no Meshy call, no credit)
- depends_on: none
- inputs: raw mesh `Art/Source/shield-meshy/meshy-raw/Meshy_AI__1017052835_model-edit.fbx` (one object `low_poly_unwrapped`, 932 tris, one island, UVMap present, no material, size 1.02 x 0.49 x 1.90 in file units, Z up); views `vault/assets/shield-meshy-{front,left,back,right}.webp` and sheet `vault/assets/shield-turnaround.webp`; nodes `vault/reference/gdd/art-audio/art-prompts-shield.md`, `vault/reference/tech/pipelines/blender-asset-route.md`, `vault/reference/tech/pipelines/mesh-fidelity-loop.md`, `vault/reference/agents/art-3d-lessons.md`; style A1 and A5 in `vault/reference/gdd/art-audio/art-style-guide.md`.
- method: script `Tools/art/blender/shield_texture.py`. Find the front side (yellow plate face, the side opposite the grip). Assign faces to flat materials by classification (normal, position, distance from the plate): `yellow` (company yellow, sample it from the front view), `gunmetal` (back plate, grip bar, bracket, bolts; dark grey sampled from the back view), `stripes` (the bevel bands of the two cut corners and of the top edge, as in the front view: top-right corner and the top edge, bottom-left corner). Stripes = one small generated texture `T_ChaserShield_Stripes.png` (yellow and black diagonal bands, about 6 cm wide, 45 degrees) on a planar UV projection of the stripe faces in UV0; other faces keep flat colours (their UV0 may be planar too, scale 100 cm per unit for the shared wear mask). LightMap UV1 as in [[blender-asset-route]]. Leave the front centre plain yellow (emblem decal later). No painted base texture, no wear chips (A5).
- transform: height 82 cm (Z), proportions kept, centimetres, `FBX_SCALE_UNITS`; front faces +X; pivot bottom-centre; report the centre of the forearm bracket (cm, Unreal axes x, -y, z) as `grip_offset_cm` for the later attachment.
- fidelity_criteria: C1 front: yellow plate, top-right and bottom-left corners cut, raised inner border visible; C2 stripes: yellow and black diagonal bands on the top edge, the top-right cut corner and the bottom-left cut corner, nowhere else on the front; C3 front centre plain yellow, no dark blob; C4 the four front bolts dark; C5 back: whole back dark gunmetal, grip bar vertical with two mounts, horizontal bracket toward one side as in the back view; C6 profiles: yellow plate edge, dark grip sticking out on the back side; C7 height to width ratio 1.86 +/- 0.1 from the front render.
- max_returns: 5
- render: four orthographic views matching the input views (front, left, back, right) plus one three-quarter top view (azimuth -35, elevation 28), light-grey background, the sun of [[blender-asset-route]]; PNGs in `Art/Meshes/ChaserShield/preview/`.
- expected_output: `Art/Meshes/ChaserShield/SM_ChaserShield.fbx`, `SM_ChaserShield.report.json` (tris, dimensions_cm, materials, uv_layers, uv_checks, grip_offset_cm, fidelity), `T_ChaserShield_Stripes.png`, the script, the previews.
- done_when: report tris 932 +/- 2, height 82 +/- 1 cm, three materials, UV0 and UV1 checks pass, every criterion pass (or open failures reported after 5 returns); the Orchestrator opens the renders.
- editor_state: any (no Unreal import in this subtask)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-216-shield-texture]] — parent global task.
