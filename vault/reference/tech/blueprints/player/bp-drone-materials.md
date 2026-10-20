---
id: bp-drone-materials
description: "M_Drone, M_DroneAntenna and their instances, Meshy drone textures: asset paths, wiring, texture settings. Open to tune or assign the drone look."
parent: blueprints-player-index
kind: doc
---
# M_Drone and M_DroneAntenna

- asset_path: /Game/RogueRigs/Art/Drone/M_Drone (material), MI_Drone (instance, no override); /Game/RogueRigs/Art/Drone/M_DroneAntenna (material), MI_DroneAntenna (instance, no override). Textures: T_Drone_BaseColor, T_Drone_Normal, T_Drone_MetalRough, same folder.
- parent_class: Material (not a Blueprint). Task [[task-252-03-drone-mesh-parts]]; meshes from [[task-252-drone-mesh-parts]], source art [[art-prompts-drone]].
- M_Drone: opaque, default lit, two sided off. BaseColor = TextureSample(T_Drone_BaseColor) RGB; Normal = TextureSample(T_Drone_Normal) RGB (sampler Normal); Roughness = TextureSample(T_Drone_MetalRough) G; Metallic = same sample B (sampler Masks). No parameters, no emissive (none baked in the textures). Wiring read back with `get_property_input`.
- M_DroneAntenna: opaque, default lit. BaseColor = VertexColor RGB (the SM_DroneAntenna FBX stores flat per-face colours: off-white 0.604, 0.554, 0.448, gunmetal 0.022, 0.0245, 0.030, rust 0.147, 0.033, 0.007, linear); Roughness = scalar parameter `Roughness` 0.7; Metallic left at its default 0. Instance exposes the single scalar `Roughness`. The mesh import must keep vertex colours (VertexColorImportOption Replace), otherwise the material reads white ([[bp-rock-slab-material]] met this).
- textures (read back): T_Drone_BaseColor 2048 x 2048 (source 5.8 MB PNG), sRGB on, TC_Default, mips from group (TEXTUREGROUP_World), no virtual texture. T_Drone_Normal 2048 x 2048, sRGB off, TC_Normalmap, mips from group (WorldNormalMap). T_Drone_MetalRough 4096 x 4096 (source 11.9 MB PNG), sRGB off (set after import), TC_Masks, mips from group (World), no virtual texture; the largest asset (10.9 MB uasset), cap its max size to 2048 if memory matters. Sources in `Art/Meshes/drone/`.
- owner_agent: agent-bp
- verified: both materials recompiled with no error raised; saved; parameter lists read back (MI_Drone none, MI_DroneAntenna Roughness 0.7). Not verified: the look on the mesh (no capture allowed), the vertex colours in the imported SM_DroneAntenna render data, the mesh slot assignment (not part of this task).

## Relations
- [[blueprints-player-index]] — parent index.
- [[bp-drone]] — the Blueprint that will use these materials.
- [[task-252-03-drone-mesh-parts]] — the task that built them.
- [[art-prompts-drone]] — Meshy prompts and the textures' origin.
