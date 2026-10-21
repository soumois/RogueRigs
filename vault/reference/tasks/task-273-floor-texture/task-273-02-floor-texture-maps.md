---
id: task-273-02-floor-texture-maps
description: "Tileable albedo and normal map at 2048 px from the owner's validated floor image, checked by numbers (seams, size, mean value)."
parent: task-273-floor-texture
kind: task
status: done
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
- model: sonnet
- tokens: input 22, cache write 39160, cache read 282168, output 1244
- runs: 1
- milestone: M1
- result: Commits 315881e4, 0672d56b, split node: Tools/art/blender/floor_texture_maps.py; source seam wrap 3.42/3.52 vs inner 1.21/1.36 fixed by a 16 px cosine border ramp (after 0.01); Lanczos-3 1254 -> 2048 wrapped; T_Floor_Plates_D mean sRGB (95,45,95), lum 59.3 (asis 71.6); N DirectX, bake strength 12, mean bytes (127.5,127.6,253.8); H 16-bit; roughness 0.8 proposed; all maps wrap step below inner step; no Blender left.

## Relations
- [[task-273-floor-texture]] — parent global task.
