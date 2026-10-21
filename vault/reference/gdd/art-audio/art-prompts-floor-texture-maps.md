---
id: art-prompts-floor-texture-maps
description: "Bake method and numbers of the floor albedo, normal and height maps (task-273-02): seam fix, resample, darkening, normal convention, material values."
parent: art-prompts-floor-texture
kind: note
---
# Floor texture maps: bake method and numbers

Script `Tools/art/blender/floor_texture_maps.py` (headless Blender, bpy and numpy), from the owner image listed in [[art-prompts-floor-texture]].

## Bake method and numbers (levels of 255)
- Seam: source wrap step 3.42 (x) and 3.52 (y) against inner 1.21 and 1.36, above 1.5 x, so a correction: the border mismatch is split over 16 px each side (cosine ramp), no content shift; after 0.01 and 0.01. Then Lanczos-3 resample to 2048 with wrapped taps, no sharpening.
- D_asis: mean RGB (123, 52, 113), mean luminance 71.6, p1 25.0, p99 104.0.
- D: linear space, saturation x0.8, per-channel gain to mean RGB (95, 45, 95); luminance 59.3, p1 20.5, p99 86.2. Wrap vs inner: 0.13 vs 0.65 (x), 0.12 vs 0.73 (y).
- Height: luminance low-pass (sigma 20, faint), cracks as grooves (blur-8 minus luminance, depth 0.38), rims raised (0.12); mean 0.49.
- Normal: DirectX (green down, Unreal default), wrapped central differences, bake strength 12 (height in 0..1 per pixel step). Mean bytes (127.5, 127.6, 253.8), length 0.993 to 1.006, wrap vs inner 0.57 vs 1.34 (x), 0.50 vs 1.49 (y).

## Material values
Roughness constant 0.8, no map. Brightness (far floor 9-12/255, mid 39/255) is tuned in the material; NormalStrength starts at 1.

## Relations
- [[art-prompts-floor-texture]] — parent: prompts, owner image, files.
- [[task-273-02-floor-texture-maps]] — task that baked these maps.
