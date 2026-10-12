---
id: task-101-06-barrel-edge-wear
description: "Barrel wear becomes light edge wear plus lid scratches: vertex-colour edge wear and the scratch mask on the lid (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: in-progress
---
# Barrel wear becomes light edge wear plus lid scratches

- goal: Barrel wear becomes light edge wear plus lid scratches: vertex-colour edge wear and the scratch mask on the lid (owner 2026-10-11).
- agent: 3d
- depends_on: task-106-02-scratch-mask, task-101-05-barrel-wear-mask
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json, Art/Textures/T_WearMask.png, vault/assets/barrel.png
- expected_output: Blender route. (1) Edge wear: a vertex colour attribute "EdgeWear" (0..1 in the red channel) on the mesh, high only on and near the outer convex edges and corners (ring edges, lid rim, cap top, body vertical corners), zero on flat faces; add the extra edge loops needed (budget 600 triangles) so that, with a hard threshold in the material, the wear shows as a few irregular light chips along the edges, light and restrained (about 10 to 15 % of the edge length, more on the lid rim and rings than on the body corners), not a uniform band; deterministic seed. (2) Scratches: the new T_WearMask.png (scratch lines) applies to M_Barrel_lid and M_Barrel_cap only, UV channel 0, 100 cm per tile; none on the body and rings. (3) Final colour = mix(paint, bare metal #8A9099, worn) with worn = max(hardEdgeWear(EdgeWear) on body, rings, lid, cap; (1 - mask) on lid and cap); decals unmasked. (4) Update barrel_preview.py to build this graph (vertex colour attribute EdgeWear with a threshold around 0.5, mask on lid and cap) and keep the render setup unchanged; run it once to the scratchpad without opening it. (5) Re-export SM_Barrel.fbx and update SM_Barrel.report.json: tris, materials, UV channels UVMap and LightMap re-checked numerically (no overlap, margin, bounds), vertex colour attribute, the updated mask contract for the later Unreal master material. Keep dimensions 90 x 90 x 134 cm and pivot. Commit by name.
- done_when: tris under 600, dimensions and pivot unchanged, UVMap and LightMap checks pass after the new edge loops, EdgeWear attribute present with its share of high vertices reported, mask contract updated, barrel_preview.py runs; committed by name.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-101-barrel-asset]] — parent global task.
