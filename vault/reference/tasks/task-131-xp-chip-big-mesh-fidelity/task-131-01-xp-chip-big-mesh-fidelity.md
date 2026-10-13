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
- max_returns: 5 (owner 2026-10-13, provisional)
- render: camera about 35 degrees elevation, the board turned about 45 degrees so a corner points at the viewer and the two board sides with legs face the viewer (as in the source), light-grey background, light of blender-asset-route; read the source image first and note the camera you estimate.
- fidelity_criteria (judged on the render against vault/assets/xp-chip-big.png):
  - C1 Board: thin square slab, thickness about 10 to 12 % of its width, split into four tiles by thin seams.
  - C2 Casing: chamfered square body about 60 % of the board width, rising about 20 % of the board width above the board, top edges sloping toward the core, a small notch at each of the 4 top corners.
  - C3 Core: a faceted stepped pyramid sunk in a square recess of the casing top, about 40 % of the board width, magenta emissive; not a flat quad.
  - C4 Legs: 3 light-grey L-shaped legs on each of two adjacent casing sides (6 total), curving down to the board, on the sides facing the viewer.
  - C5 Bolts: 4 hexagonal bolts at the 4 board corners, each about 10 % of the board width, clearly raised and visible.
  - C6 Details: 3 or 4 light-grey traces with right-angle bends inlaid on the board (thin geometry or a textured decal), and 3 box-shaped components (two larger on opposite corners, one smaller).
  - C7 Colours: board and casing dark gunmetal, legs, bolts and traces light grey, core magenta (#FF26BF); no cyan anywhere.
  - C8 Numbers: tris_out at most 1000; 30 x 30 cm footprint; pivot bottom centre; flat shading; UV checks pass.

## Relations
- [[task-131-xp-chip-big-mesh-fidelity]] — parent global task.
