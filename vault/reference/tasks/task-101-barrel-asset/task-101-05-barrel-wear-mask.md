---
id: task-101-05-barrel-wear-mask
description: "Barrel body gets UVs and the shared wear mask (level 1 charter) plus a lightmap UV channel, replacing the wear made in geometry."
parent: task-101-barrel-asset
kind: task
status: in-progress
---
# Barrel wear mask and lightmap

- goal: Barrel body gets UVs and the shared wear mask (level 1 charter) plus a lightmap UV channel, replacing the wear made in geometry.
- agent: 3d
- depends_on: task-101-04-barrel-mesh, task-106-01-wear-mask-texture
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), barrel (vault/reference/gdd/terrain/barrel.md); files Tools/art/blender/barrel.py, Art/Meshes/barrel/SM_Barrel.report.json, Art/Textures/T_WearMask.png, vault/assets/barrel.png
- expected_output: Blender route, no Meshy. (1) UV channel 0 "UVMap" on the body (body, rings, lid, cap, chamfers): planar or box projection per face with constant texel density so that T_WearMask.png (tileable) covers about 100 cm per tile; the decal quads keep their own 0..1 UV on UVMap. (2) A second UV channel "LightMap": every triangle (decals included) unwrapped without overlap in 0..1 with a margin of at least 2 px at 256 px; check numerically (no overlap, bounds inside 0..1). (3) Remove the wear made in geometry: the lid and cap chamfers keep their bevel but take M_Barrel_lid / M_Barrel_cap; M_Barrel_edge_wear and the chip triangles go; the wear now comes from the mask. (4) Wear is a blend, not a multiply (a multiply only darkens): the final colour of body, rings, lid and cap is mix(bare metal colour #8A9099, flat paint colour, mask), so dark mask = worn light metal; the decals are not masked. The FBX cannot hold this node graph: write the contract in the report (mask file, UV channel 0, tile = 100 cm, bare metal colour, which materials are masked) for the later Unreal master material. (5) Tools/art/blender/barrel_preview.py <fbx> <out.png>: loads the FBX, builds the mix graph above on those materials with Art/Textures/T_WearMask.png on UV channel 0, renders ONE image headless (EEVEE, 1000x1000) with the same setup as the owner has seen: orthographic camera, ortho scale 200, azimuth -35 degrees, elevation 28 degrees, distance 400 cm, target (0,0,67); one sun light energy 4.0 rotated (50, 10, -60) degrees; white world at strength 0.35, Standard view transform. The Orchestrator runs it; do not look at the render yourself. (6) Re-export SM_Barrel.fbx and update SM_Barrel.report.json (tris, materials, UV channels, mask contract). Budget 600 triangles. Commit by name.
- done_when: report lists UV channels UVMap and LightMap with numeric overlap and bounds checks, no overlap, tris under 600, dimensions 90 x 90 x 134 cm and pivot unchanged, M_Barrel_edge_wear removed, the mask contract written; barrel_preview.py runs headless and writes the PNG; committed by name.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-101-barrel-asset]] — parent global task.
