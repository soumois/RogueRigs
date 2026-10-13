---
id: task-128-01-xp-nut-mesh
description: "Build the hex nut mesh for the basic XP pickup from the validated image, Blender route."
parent: task-128-xp-nut-mesh
kind: task
status: in-progress
---
# XP pickup mesh: hex nut

- goal: Build the hex nut mesh for the basic XP pickup from the validated image, Blender route.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), barrel-asset (vault/reference/tech/pipelines/barrel-asset.md)
- expected_output: Tools/art/blender/xp_nut_mesh.py, Art/Meshes/xp-nut-mesh/SM_XPNut.fbx (centimetres, bottom-centre pivot) and SM_XPNut.report.json; materials: dark grey body, glowing cyan hole (emissive material); flat shading, flat colour per material, UV0 and LightMap UV1 as in blender-asset-route; footprint equal to the current placeholder XP cube found in Source/RogueRigs (the 3D agent reads the code first and reports the value)
- done_when: Blender route, no Meshy call; SM_XPNut.fbx and report exist; tris_out at most 300; dimensions_cm match the placeholder cube footprint; flat shading; pivot bottom centre; materials as listed; UV checks pass; preview render produced with the barrel_preview camera for the Orchestrator to send to the owner; nothing imported in Unreal; image read from vault/assets/xp-nut.png
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-128-xp-nut-mesh]] — parent global task.
