---
id: task-130-01-xp-nut-mesh-fidelity
description: "Rebuild the XP hex nut mesh so it matches the validated image, judged by the fidelity loop."
parent: task-130-xp-nut-mesh-fidelity
kind: task
status: in-progress
---
# XP pickup mesh: hex nut, fidelity pass

- goal: Rebuild the XP hex nut mesh so it matches the validated image, judged by the fidelity loop.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: Blender route, no Meshy call. Rebuild Tools/art/blender/xp_nut_mesh.py, Art/Meshes/xp-nut-mesh/SM_XPNut.fbx and SM_XPNut.report.json (with the fidelity object) and a final render on a light-grey background; footprint 30 cm across corners, pivot bottom centre, centimetres, flat shading, two flat materials (grey body, emissive cyan hole), UV0 and LightMap UV1 as before
- done_when: All criteria C1 to C6 of the subtask pass in the comparison render, or the report lists the criteria still failing after 5 returns; tris_out at most 300; nothing imported in Unreal
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open
- max_returns: 5 (owner 2026-10-13, provisional)
- render: camera about 40 degrees elevation, yaw as in the source (two side faces visible), light-grey background, light of blender-asset-route; read the source image first and note the camera you estimate.
- fidelity_criteria (judged on the render against vault/assets/xp-nut.png):
  - C1 Shape: hexagonal prism, thick but low; height about 45 % of the across-flats width (tolerance 8 %).
  - C2 Hole: diameter about 60 to 65 % of the across-flats width at the top, conical with at least 3 visible stepped rings narrowing downward, and a visible floor; not a flat disc.
  - C3 Top edge: a chamfer around the hole mouth and a bevel at each of the 6 corners of the top face, visible as facets.
  - C4 Materials: one flat mid-grey for the body; every hole surface in the emissive cyan material (0, 0.9, 1); no other colour.
  - C5 Numbers: tris_out at most 300; 30 cm across corners; pivot bottom centre.
  - C6 Style: flat shading, large facets visible, no texture.

## Relations
- [[task-130-xp-nut-mesh-fidelity]] — parent global task.
