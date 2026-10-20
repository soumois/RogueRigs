---
id: task-252-01-drone-mesh-parts
description: "The Meshy drone is cleaned, split into base, laser and a modelled parabolic dish, and imported in Unreal with its material."
parent: task-252-drone-mesh-parts
kind: task
status: in-progress
---
# Drone mesh in parts

- goal: The Meshy drone is cleaned, split into base, laser and a modelled parabolic dish, and imported in Unreal with its material.
- agent: 3d
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts-rigs (vault/reference/gdd/art-audio/art-prompts-rigs.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: Zip and image kept in the repo and a vault node art-prompts-drone (prompt as used); Meshy FBX cleaned (about 6 000 triangles at most), about 70 uu wide, cm units, front +X, split into DroneBase (body, side pods) and DroneLaser (the bottom part) with pivots and a report; no AI at runtime.
- done_when: DroneBase and DroneLaser FBX exist with pivots (laser: pitch pivot at its top), at most 6 000 triangles together, FBX_SCALE_UNITS, cut check by numbers, Blender processes stopped; image and zip stored; art-prompts-drone node written; renders sent to the owner.
- editor_state: not needed (Blender headless)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-252-drone-mesh-parts]] — parent global task.
