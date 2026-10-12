---
id: task-101-07-barrel-edge-highlights
description: "Barrel edge wear as in the validated image: thin lighter edge bands, bolts and stepped base on the cap, no metal patches."
parent: task-101-barrel-asset
kind: task
status: in-progress
---
# Barrel edge highlights, bolts and stepped cap

- goal: Barrel edge wear as in the validated image: thin lighter edge bands, bolts and stepped base on the cap, no metal patches.
- agent: 3d
- depends_on: task-101-06-barrel-edge-wear
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json, Art/Textures/T_WearMask.png, vault/assets/barrel.png
- expected_output: Owner 2026-10-11: the wear in vault/assets/barrel.png is NOT patches or bare metal: it is a thin lighter band (about 1 cm) along some edges of the lid chamfer, the cap top and the top edge of the rings, in a lighter tone of the surface colour (not grey metal #8A9099), continuous but interrupted in places. Blender route. (1) Remove the EdgeWear vertex colour, the hard-threshold chips and the mix with bare metal; remove the extra edge loops that only served EdgeWear. (2) Add thin geometric edge bands (about 1 cm) with their own flat materials, lighter tones of the base colours: lid chamfer band lighter lid grey, cap top band lighter cap grey, ring top edge band lighter ring orange; each covers about 70 to 80 % of the edge length with 2 or 3 seeded gaps per loop (deterministic seed), body and flat faces stay clean. (3) Cap: 4 small hex bolts on its top (as in the image) and a stepped base (two stacked rings, the lower one slightly wider). (4) Keep the scratch mask on M_Barrel_lid and M_Barrel_cap only, UV channel 0, 100 cm per tile, but faint: colour = lerp(lid colour, lighter lid tone, (1 - mask) * 0.6). (5) Keep dimensions 90 x 90 x 134 cm, pivot, sticker, pictogram, UV channels UVMap and LightMap re-checked numerically (no overlap, margin >= 2 px at 256 px, bounds 0..1); budget 600 triangles. (6) Update barrel_preview.py (no vertex colour; faint scratch lerp on lid and cap; same render setup), run it once to the scratchpad without opening it; re-export SM_Barrel.fbx; update SM_Barrel.report.json (tris, materials, UV checks, updated mask_contract for the Unreal master material). Commit by name.
- done_when: no EdgeWear attribute and no bare-metal mix left; thin lighter edge bands with seeded gaps, 4 bolts and stepped cap base present; tris under 600; dimensions and pivot unchanged; UV and lightmap checks pass; mask contract updated; barrel_preview.py runs; committed by name.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-101-barrel-asset]] — parent global task.
