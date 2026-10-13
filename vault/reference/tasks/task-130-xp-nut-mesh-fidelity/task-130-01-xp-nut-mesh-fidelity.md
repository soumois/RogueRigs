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
- run: 2 (owner 2026-10-13: the hole reads as a filled cyan dome, not as an empty hole; run 1 passed C2 too leniently; 5 new returns from the committed files)
- strictness: a criterion passes only if someone comparing the render and vault/assets/xp-nut.png side by side would not call that part different. Name in the evidence what still differs even when you pass.
- fidelity_criteria run 2 (C1, C3, C4, C5, C6 unchanged and passed in run 1; C2 replaced, C7 added):
  - C2 Hole reads as EMPTY and deep: depth at least 65 % of the nut height (floor well below the top face); at least 3 stepped bands on the walls; the far wall is lit and the near wall is hidden by the rim; walls and floor are two different materials (walls saturated cyan (0, 0.9, 1), floor a paler cyan about (0.55, 0.95, 1)); the bands alternate in brightness with a visible difference (at least 25 %); seen from the render camera the hole must read as a hollow with a floor, never as a filled dome or a flat disc.
  - C7 Rim glow cue: the hole mouth chamfer is the brightest ring, so the eye reads rim, then walls, then floor, as in the source.
- render: same camera as run 1 (azimuth 11, elevation 52, ortho 42). In the preview only you may lower the emission strength so the shading of the walls shows; the mesh materials keep their emissive colours.
- run: 3 (owner 2026-10-13: a nut has a real through hole; the blue inside will be Unreal bloom, not a cyan floor; the source image shows a floor, the owner overrides it; 5 new returns from the committed files)
- fidelity_criteria run 3 (C1, C3, C5, C6 unchanged; C2, C4, C7 replaced):
  - C2 Through hole: the hole goes through the whole nut, no floor, the ground shows through it; diameter about 60 to 65 % of the across-flats width at the top; at least 3 stepped bands (thread-like) on the wall; the wall is grey, not cyan.
  - C4 Materials: exactly two: M_XPNut_body (grey, body, hole wall and chamfer) and M_XPNut_glow (emissive cyan (0, 0.9, 1)). The pale floor material is removed.
  - C7 Glow source: M_XPNut_glow is ONE thin ring about 1.2 cm tall, inset in the hole wall at mid height, seen as a glowing seam; nothing else is cyan. In Unreal the bloom spreads this glow over the inside of the hole.
- render: same camera as run 1 (azimuth 11, elevation 52, ortho 42); in the preview you may lower the emission so the ring reads as a band; the ground plane under the nut is the light-grey background.

## Relations
- [[task-130-xp-nut-mesh-fidelity]] — parent global task.
