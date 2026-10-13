---
id: task-129-01-xp-chip-big-mesh
description: "Build the data chip mesh for the big XP block from the validated image, Blender route."
parent: task-129-xp-chip-big-mesh
kind: task
status: done
---
# Big XP block mesh: data chip

- goal: Build the data chip mesh for the big XP block from the validated image, Blender route.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), barrel-asset (vault/reference/tech/pipelines/barrel-asset.md)
- expected_output: Tools/art/blender/xp_chip_big_mesh.py, Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx (centimetres, bottom-centre pivot) and SM_XPChipBig.report.json; materials: dark gunmetal board and casing, grey legs, glowing magenta-pink core (emissive material, never cyan); flat shading, flat colour per material, UV0 and LightMap UV1 as in blender-asset-route; footprint equal to the current placeholder XP cube found in Source/RogueRigs (the 3D agent reads the code first and reports the value)
- done_when: Blender route, no Meshy call; SM_XPChipBig.fbx and report exist; tris_out at most 600; dimensions_cm match the placeholder cube footprint; flat shading; pivot bottom centre; materials as listed; UV checks pass; preview render produced with the barrel_preview camera for the Orchestrator to send to the owner; nothing imported in Unreal; image read from vault/assets/xp-chip-big.png
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 30, cache write 61327, cache read 567768, output 8286
- runs: 1
- milestone: M2
- result: Superseded by task-131 (first chip far from the image).

## Relations
- [[task-129-xp-chip-big-mesh]] — parent global task.
