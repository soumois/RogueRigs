---
id: bp-drone-materials
description: "M_Drone, M_DroneAntenna, textures and the three drone meshes: asset paths, slots, bounds, attachment offsets. Open to tune or attach the drone parts."
parent: blueprints-player-index
kind: doc
---
# M_Drone and M_DroneAntenna

- asset_path: /Game/RogueRigs/Art/Drone/M_Drone (material), MI_Drone (instance, no override); /Game/RogueRigs/Art/Drone/M_DroneAntenna (material), MI_DroneAntenna (instance, no override). Textures: T_Drone_BaseColor, T_Drone_Normal, T_Drone_MetalRough, same folder.
- parent_class: Material (not a Blueprint). Task [[task-252-03-drone-mesh-parts]]; meshes from [[task-252-drone-mesh-parts]], source art [[art-prompts-drone]].
- M_Drone: opaque, default lit, two sided off. BaseColor = TextureSample(T_Drone_BaseColor) RGB; Normal = TextureSample(T_Drone_Normal) RGB (sampler Normal); Roughness = TextureSample(T_Drone_MetalRough) G; Metallic = same sample B (sampler Masks). No parameters, no emissive (none baked in the textures). Wiring read back with `get_property_input`.
- M_DroneAntenna: opaque, default lit. BaseColor = VertexColor RGB (the SM_DroneAntenna FBX stores flat per-face colours: off-white 0.604, 0.554, 0.448, gunmetal 0.022, 0.0245, 0.030, rust 0.147, 0.033, 0.007, linear); Roughness = scalar parameter `Roughness` 0.7; Metallic left at its default 0. Instance exposes the single scalar `Roughness`. The mesh import must keep vertex colours (VertexColorImportOption Replace), otherwise the material reads white ([[bp-rock-slab-material]] met this).
- textures (read back): T_Drone_BaseColor 2048 x 2048 (source 5.8 MB PNG), sRGB on, TC_Default, mips from group (TEXTUREGROUP_World), no virtual texture. T_Drone_Normal 2048 x 2048, sRGB off, TC_Normalmap, mips from group (WorldNormalMap). T_Drone_MetalRough 4096 x 4096 (source 11.9 MB PNG), sRGB off (set after import), TC_Masks, mips from group (World), no virtual texture; MaxTextureSize capped to 2048 in task-252-04 (read back 2048; the source stays 4096). Sources in `Art/Meshes/drone/`.
- meshes (task [[task-252-04-drone-mesh-parts]], /Game/RogueRigs/Art/Drone/, from Art/Meshes/drone/*.fbx with StaticMeshTools.import_file: import_materials false, import_textures false, combine_meshes true; no collision, Nanite off, one LOD; LightMapCoordinateIndex set to 1 after import, the import had set 2):
  - SM_DroneBase, 4556 tris, slot meshy_drone_001 = MI_Drone, origin at the body centre. Bounds read back x -14.83..14.83, y -35..35, z -17.25..30.17.
  - SM_DroneLaser, 754 tris, slot meshy_drone_001 = MI_Drone, origin at the neck centre. Bounds x -7.08..5.56, y -8.20..8.55, z -19..0 (Y mirrored against the report: -8.55..8.20).
  - SM_DroneAntenna, 392 tris, slot meshy_drone_antenna = MI_DroneAntenna, origin at the foot of the flange on the Z spin axis. Bounds x -2.03..11.83, y -9.07..9.75, z 0..21.96 (report y -9.75..9.07, mirrored).
  - All bounds, triangle counts and slots match the reports (sizes unchanged); only Y changes sign, the Unreal import mirrors Y.
- attachment points in Unreal axes (Y mirror: the Blender left +Y becomes Unreal -Y; read from the bounds of the three meshes, not from single vertices): laser pivot offset from the base origin (1.606, 0, -17.248) (Y 0, no sign issue). Antenna foot offset from the base origin: Blender (-4.257, 0.837, 28.752) becomes Unreal (-4.257, -0.837, 28.752). Emitter tip in the laser frame: Blender (-0.054, -0.143, -19) becomes Unreal (-0.054, +0.143, -19). X and Z unchanged. The laser pitches about the Y axis through its pivot (Y axis keeps its direction up to sign).
- owner_agent: agent-bp
- verified: both materials recompiled with no error raised; saved; parameter lists read back (MI_Drone none, MI_DroneAntenna Roughness 0.7). Mesh channels (task-253-01, `check_mesh_channels.py`, [[tool-unreal-reimport]]): the 252-04 import had lost the antenna colours (source and render all white) and left Generate Lightmap UVs on (a third render UV channel, uv2, generated on all three meshes). Fixed: SM_DroneAntenna reimported with `--vertex-colors=replace`, then `static_mesh_build_settings.py` on the three (lightmap UVs off, full precision UVs, LightMapCoordinateIndex 1). Read back after: antenna source and render 1176 non-white corners in 3 colour classes (RGB rounded to 0.25: 0.75 grey 390, 0.25 grey 612, (0.5, 0.25, 0) rust 174); render uv0 and uv1 of the three identical to the source, no uv2; bounds and slots unchanged. Not verified: the look on the mesh (no capture allowed), the Y-mirror sign of the attachment offsets on single vertices.

## Relations
- [[blueprints-player-index]] — parent index.
- [[bp-drone]] — the Blueprint that will use these materials.
- [[task-252-03-drone-mesh-parts]] — the task that built them.
- [[task-252-04-drone-mesh-parts]] — the task that imported the meshes and assigned the materials.
- [[art-prompts-drone]] — Meshy prompts and the textures' origin.
