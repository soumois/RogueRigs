---
id: bp-floor-plates-material
description: "M_FloorPlates and MI_FloorPlates, the arena floor material: world-aligned plate textures, noise anti-tiling, parameters, cost, revert. Open to tune the floor look."
parent: blueprints-world-index
kind: doc
---
# M_FloorPlates and MI_FloorPlates

- asset_path: /Game/RogueRigs/Environment/Floor/M_FloorPlates (material), MI_FloorPlates (instance, defaults below). Textures T_Floor_Plates_D (sRGB, TC_Default, wrap, 2048, mips from texture group) and T_Floor_Plates_N (TC_Normalmap, sRGB off, wrap, 2048, DirectX, no green flip), baked in [[art-prompts-floor-texture-maps]]. Same folder convention as [[bp-rock-slab-material]].
- parent_class: Material (not a Blueprint). Opaque, default lit, flags `bUsedWithInstancedStaticMeshes` true and `bTangentSpaceNormal` false (the normal output is a world normal, see graph). Task-273-03.
- purpose: floor of [[bp-arena-grid]] (`FloorMaterial`), world-position UVs so the 16x16 floor chunk HISMs show one continuous plate pattern. Display only, no gameplay logic.
- graph: three Custom HLSL nodes (like M_RockSlab), no other texture node. FloorBlend: value noise (one octave, 4 hashes, smooth interpolation) of world XY / NoiseScale, output x = noise 0..1, y = blend weight = smoothstep(saturate((noise - 0.5) x BlendContrast + 0.5)). FloorAlbedo: sample 1 at uv = world XY / TileSize; sample 2 at uv rotated 37 degrees, times AltScale, plus (0.37, 0.61); lerp by the blend weight; value x (1 + m x NoiseAmount) and tint (1 + m x HueAmount, 1 - m x HueAmount, 1 + m x HueAmount) with m = (noise - 0.5) x 2 (toward magenta when m > 0); times AlbedoBrightness. FloorNormal: same two samples (UnpackNormalMap), the second one rotated back by the same angle, lerp, xy x NormalStrength, normalized; flat floor so texture x = world X and y = world Y (DirectX green = +V = +world Y), hence the world-space normal. Sign verified by measurement (task-273-04, r11 frames, sun toward world -X): relief reads as raised plates lit on the sun side, red sign confirmed, green consistent (one frame agrees, one inconclusive): no flip. Noise method: HLSL value noise, cheapest smooth option, no texture.
- parameters (MI_FloorPlates, group Floor): TileSize 800 (uu per texture repeat; the texture holds about 4 plates across, so a plate is about 200 uu = 3 cells of 67), NoiseScale 3000, AltScale 0.73, BlendContrast 2.5, NoiseAmount 0.08, HueAmount 0.03, AlbedoBrightness 0.27, NormalStrength 1, Roughness 0.8; textures AlbedoTexture, NormalTexture. Specular and Metallic stay at the defaults (0.5, 0). All provisional, the owner tunes by eye.
- brightness arithmetic: M_GreyBox Color of MI_GB_Floor = (0.045, 0.038, 0.032) linear, luminance 0.2126 x 0.045 + 0.7152 x 0.038 + 0.0722 x 0.032 = 0.0391. New albedo mean sRGB (95, 45, 95) = linear (0.1144, 0.0262, 0.1144), luminance 0.0513. Target 0.35 x 0.0391 = 0.01367 (far floor about 10.5 vs 17 in sRGB, (10.5/17)^2.2 = 0.35). AlbedoBrightness = 0.01367 / 0.0513 = 0.27. Checked by capture in the next subtask.
- cost (estimated, MCP exposes no material statistics): 4 texture fetches (2 on D, 2 on N, 2 samplers), about 130 to 170 ALU instructions in the base pass pixel shader (noise about 40, albedo about 30, normal about 40). No world position offset. Not measured.
- verified (task-273-03): material and instance compile with no log error, saved; parameters read back from MI_FloorPlates; BP_ArenaGrid class default and the L_ArenaM1 instance read back FloorMaterial = MI_FloorPlates (the open editor showed the stale MI_GB_Floor until the map was reloaded). Not verified: the look on screen (no capture allowed), normal orientation, instruction count, shader compile in a packaged build.
- revert: set `FloorMaterial` of BP_ArenaGrid back to /Game/RogueRigs/Materials/GreyBox/MI_GB_Floor (parent M_GreyBox, untouched), compile, save, reload the map.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-arena-grid]] — Blueprint whose FloorMaterial uses it.
- [[bp-rock-slab-material]] — the other world-aligned texture material, same conventions.
- [[art-prompts-floor-texture-maps]] — how the textures were baked and the target numbers.
- [[task-273-03-floor-material]] — the task that built it.
