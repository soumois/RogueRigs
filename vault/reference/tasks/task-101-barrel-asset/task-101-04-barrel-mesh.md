---
id: task-101-04-barrel-mesh
description: "Barrel mesh built in Blender from the owner-validated image, exported as a cleaned FBX with a report."
parent: task-101-barrel-asset
kind: task
status: todo
---
# Barrel mesh built in Blender from the owner-validated image, exported as a cleaned FBX with a report.

- goal: Barrel mesh built in Blender from the owner-validated image, exported as a cleaned FBX with a report.
- agent: 3d
- depends_on: task-101-03-barrel-warning (and the owner copying the retained image to vault/assets/)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), barrel (vault/reference/gdd/terrain/barrel.md), color-code (vault/reference/gdd/art-audio/color-code.md); image vault/assets/barrel.png (validated by the owner 2026-10-11: orange barrel, Kessler-Voss sticker, explosion pictogram)
- expected_output: Blender route, no Meshy (owner 2026-10-11): a bpy script Tools/art/blender/barrel.py builds the barrel from the validated image (8 flat sides, flat shading, two rings, lid with one cap), colours as per-material flat colours (orange body, darker orange rings, dark grey lid and cap), the sticker and the warning pictogram as two small decals with their textures cropped from the image; export Art/Meshes/barrel/SM_Barrel.fbx in cm and its .report.json. Values (owner 2026-10-11): about 600 triangles max, about 134 cm high and about 90 cm wide, pivot bottom centre. No Meshy call, no import into Unreal.
- done_when: report within 600 triangles, 134 cm x 90 cm, pivot bottom centre, UVs on the decals, materials listed; FBX and report committed; the owner validates the mesh.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-101-barrel-asset]] — parent global task.
