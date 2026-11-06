---
id: bp-floor-plates-material
description: "Arena floor material: M_FloorCartoon with the owner painted flagstones (current), M_FloorPlates of task-273 (kept unused); texture, size, parameters, revert."
parent: blueprints-world-index
kind: doc
---
# Floor material: M_FloorCartoon (current) and M_FloorPlates (task-273, unused)

- asset_path: /Game/RogueRigs/Environment/Floor/M_FloorCartoon (material) and MI_FloorCartoon (instance, no override, parent M_FloorCartoon), task-470-01. Texture T_Floor_Cartoon_D: imported from `Art/Source/floor-texture/T_Floor_Cartoon_D.png` (keep: lossless PNG of the owner image `owner-2026-11-05-floor-cartoon.webp`, 1254 px, seamless); sRGB, TC_Default, wrap, texture group World, mips from the group, PowerOfTwoMode StretchToPowerOfTwo (a 1254 px texture otherwise gets no mips and shimmers).
- parent_class: Material (not a Blueprint). Opaque, default lit; made as a copy of M_FloorPlates.
- purpose: floor of [[bp-arena-grid]] (`FloorMaterial` = MI_FloorCartoon), world-position UVs so the 16x16 floor chunk HISMs show one continuous flagstone pattern. Display only, no gameplay logic.
- graph: two Custom HLSL nodes. FloorBlend (unchanged from M_FloorPlates): value noise of world XY / NoiseScale, x = noise 0..1. FloorAlbedo: one sample of AlbedoTexture at uv = world XY / TileSize; value x (1 + m x NoiseAmount) and tint (1 + m x HueAmount, 1 - m x HueAmount, 1 + m x HueAmount), m = (noise - 0.5) x 2; times AlbedoBrightness. Normal output disconnected.
- removed (task-470-01): the second sample rotated 37 deg and its noise blend (it ghosted two crack patterns and blurred the painted strokes); the normal map, the FloorNormal node and NormalStrength (no relief: the light is painted in the texture). The slow value and hue modulation is kept: it does not blur.
- parameters (material defaults, group Floor): TileSize 1700 (uu per texture repeat; the texture holds about 4.5 stones across, so a stone is about 380 uu, 3.5 to 4 miner widths on screen as on the owner image 2), AlbedoBrightness 0.6, NoiseScale 3000, NoiseAmount 0.08, HueAmount 0.03, Roughness 0.8. AltScale and BlendContrast remain as parameters but no longer affect the colour. All provisional, the owner tunes by eye.
- verified (task-470-02): -game frames `Saved/Captures/task470-floor-capture/` (c_01, wide cw_00, compare_before_after_ref.png): flagstones over the whole floor, no seam or obvious grid, pink crack rims read in the miner light pool; lit floor mean sRGB (108, 29, 62) against (116, 34, 67) on image 2; far floor darker than image 2 (light, [[task-473-miner-light-calibration]]). No material error in the run logs.
- cost (estimated, not measured): 1 texture fetch, about 70 ALU instructions (noise about 40).
- previous material: M_FloorPlates, MI_FloorPlates, T_Floor_Plates_D and T_Floor_Plates_N of [[task-273-03-floor-material]] (two rotated samples, world-space normal, TileSize 800, AlbedoBrightness 0.27; textures baked in [[art-prompts-floor-texture-maps]]) stay in the repo, untouched and referenced by nothing.
- revert: set `FloorMaterial` of BP_ArenaGrid back to /Game/RogueRigs/Environment/Floor/MI_FloorPlates (or the grey box /Game/RogueRigs/Materials/GreyBox/MI_GB_Floor), compile, save, reload the map.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-arena-grid]] — Blueprint whose FloorMaterial uses it.
- [[bp-rock-slab-material]] — the other world-aligned texture material, same conventions.
- [[art-prompts-floor-texture-maps]] — how the task-273 textures were baked.
- [[task-273-03-floor-material]] — the task that built M_FloorPlates.
- [[task-470-cartoon-floor-texture]] — the task that switched the floor to the owner flagstones.
- [[visual-style]] — the floor rule this material implements.
