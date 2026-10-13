---
id: task-131-01-xp-chip-big-mesh-fidelity
description: "Rebuild the big XP data chip mesh so it matches the validated image, judged by the fidelity loop."
parent: task-131-xp-chip-big-mesh-fidelity
kind: task
status: in-progress
---
# Big XP block mesh: data chip, fidelity pass

- goal: Rebuild the big XP data chip mesh so it matches the validated image, judged by the fidelity loop.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: Blender route, no Meshy call. Rebuild Tools/art/blender/xp_chip_big_mesh.py, Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx and SM_XPChipBig.report.json (with the fidelity object) and a final render on a light-grey background; footprint 30 x 30 cm, the 1.8 scale is applied in game, pivot bottom centre, centimetres, flat shading; materials per colour: dark gunmetal board and casing, light grey legs, bolts and traces, emissive magenta-pink core (#FF26BF), never cyan; UV0 and LightMap UV1; traces may be a textured decal (A1)
- done_when: All criteria C1 to C8 of the subtask pass in the comparison render, or the report lists the criteria still failing after 5 returns; tris_out at most 1000; nothing imported in Unreal
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open
- run: 2 (owner 2026-10-13: first run left C2 failing and was judged too lenient; criteria tightened; 5 new returns, starting from the committed files)
- max_returns: 5 (owner 2026-10-13, provisional)
- render: camera about 35 degrees elevation, the board turned about 45 degrees so a corner points at the viewer and the two board sides with legs face the viewer, light-grey background, light of blender-asset-route. Use the camera of run 1 (orthographic 50 cm, azimuth 45, elevation 35) so renders stay comparable.
- strictness: a criterion passes only if someone comparing the render and vault/assets/xp-chip-big.png side by side would not call that part different. "Roughly there" is a fail. Name in the evidence what still differs even when you pass.
- fidelity_criteria (judged on the render against vault/assets/xp-chip-big.png):
  - C1 Board: thin square slab, thickness about 10 to 12 % of its width, split into four tiles by thin seams. (passed in run 1, keep)
  - C2 Casing: lower body about 60 % of the board width and low, about 15 % of the board width above the board (run 1 was too tall with vertical walls). Its top 40 % of height is a wide 45 degree chamfer sloping inward toward the core recess, with a thin flat rim around the recess. A thin darker ledge sits at its base. Each of the 4 top corners has a small triangular notch cut into the chamfer (an inset, not a raised block).
  - C3 Core: a faceted stepped pyramid sunk in a square recess about 40 % of the board width, magenta emissive. (passed in run 1, keep)
  - C4 Legs: 3 light-grey legs on each of the two casing sides facing the viewer (6 total); each leg is compact, about 7 % of the board width wide, leaves the casing wall at about half its height, goes out with a rounded elbow (at least 2 bevel segments, no sharp Z shape) and ends in a small flat foot on the board; the 3 legs are evenly spaced and identical.
  - C5 Bolts: 4 clean hex bolts at the board corners: a flat-topped regular hexagonal head (6 equal sides) on a short base, about 10 % of the board width, no irregular facets, light grey.
  - C6 Traces and components: at least 4 light-grey traces, each about 4 % of the board width wide and 20 to 35 % long, with one or two right-angle or 45 degree bends, flush or raised at most 0.3 cm, clearly visible as lines from the camera (thin geometry or a textured decal); 3 box components (two larger on opposite corners, one smaller), dark with a lighter band on top.
  - C7 Colours: board and casing dark gunmetal, legs, bolts and traces light grey, core magenta (#FF26BF); no cyan anywhere. (passed in run 1, keep)
  - C8 Numbers: tris_out at most 1000; 30 x 30 cm footprint; pivot bottom centre; flat shading; UV checks pass. (passed in run 1, keep)
- run: 3 (owner 2026-10-13: the sides work well, the volume of the middle is hard to understand; 5 new returns from the committed files; C1, C4, C5, C6, C7, C8 unchanged)
- fidelity_criteria run 3 (C2 and C3 replaced, C9 added):
  - C2 Casing: keep run 2 proportions (about 60 % of the board width, about 15 % high, 45 degree chamfer on the top 40 %, notched corners) AND the chamfer is a separate, lighter gunmetal material than the vertical wall (tone difference at least 20 % in the render), AND a thin lighter band (about 0.3 cm) runs along the top edge of the chamfer and the top rim (charter A1 edge wear).
  - C3 Core as a raised gem: a faceted stepped pyramid, magenta emissive, about 40 % of the board width, that rises at least 1 cm ABOVE the casing rim; the opening in the casing is shallow (at most 1 cm deep); facets use two magenta materials by orientation: bright (#FF26BF) and darker (#B0107F), alternating so at least 3 distinct tones are visible across the facets.
  - C9 Volume reads: looking at the render without the source, a viewer can tell the core is a raised faceted crystal sitting in a casing with a sloped top; no part of the core is hidden by a recess wall; the core does not read as a flat pink square.
- materials: board, casing_wall, casing_chamfer (lighter), edge_band, legs and bolts and traces (light grey), core_bright, core_dark. Triangle ceiling stays 1000.
- render: same camera as run 1 and 2 (ortho 50 cm, azimuth 45, elevation 35); in the preview only you may lower the emission so facet shading shows.
- run: 4 (owner 2026-10-13: the gem is liked; the light part around it was added as extra triangles instead of colouring the inside of the opening, so dark gunmetal still shows between the gem and the light rim; and the light is too bright; 5 returns maximum, a small change is expected to need 1 or 2)
- fidelity_criteria run 4 (C1 to C8 and C9 unchanged and must keep passing; C10 added):
  - C10 Opening colour: the faces that form the inside of the casing opening (its floor around the gem base and its inner walls) take the light edge_band material by recolouring those faces, not by adding geometry; from the render camera NO dark gunmetal is visible between the gem and the light rim. The edge_band colour is no brighter than the light grey of the legs, bolts and traces (#8A9099 to #959CA6, down from #A9B0BA). The thin band along the top edge of the chamfer uses the same colour. Triangle count does not go up (at most 872).
- render: same camera as runs 1 to 3 (ortho 50 cm, azimuth 45, elevation 35); also render one close crop on the gem base (ortho scale 16, same angles) and open it to judge C10.

## Relations
- [[task-131-xp-chip-big-mesh-fidelity]] — parent global task.
