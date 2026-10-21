---
id: task-273-02-floor-texture-maps
description: "Tileable albedo and normal map at 2048 px from the owner's validated floor image, checked by numbers (seams, size, mean value)."
parent: task-273-floor-texture
kind: task
status: todo
---
# Floor texture maps

- goal: From the owner-validated image, a seamless albedo (base value chosen for the luminance targets) and a matching normal map, 2048 x 2048, roughness constant (a value, no map).
- agent: 3d
- depends_on: task-273-01-floor-texture, owner image
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-prompts-rock-top-texture (vault/reference/gdd/art-audio/art-prompts-rock-top-texture.md) for the seam method, blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), the prompt node art-prompts-floor-texture, the owner image path given in the delegation
- expected_output: `Art/Source/floor-texture/` albedo and normal PNG 2048, 3 x 3 tiled previews, a numbers report (wrap step versus inner step per map, mean albedo, normal mean close to (128,128,255)); `Tools/art/` script if a new one is needed.
- done_when: Both maps 2048 square; wrap step close to inner step; mean albedo in the stated range; no Blender process left; node updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-273-floor-texture]] — parent global task.
