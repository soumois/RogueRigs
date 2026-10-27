---
id: bp-asteroid-material
description: "M_Asteroid, MI_Asteroid (lit flat rock, world-noise tint) and the four SM_Asteroid meshes: parameters, import settings, reimport trap, revert; open to tune."
parent: blueprints-world-index
kind: doc
---
# M_Asteroid / MI_Asteroid / SM_Asteroid_01..04

- asset_path: /Game/RogueRigs/Environment/Space/Asteroids/ (M_Asteroid, MI_Asteroid, SM_Asteroid_01 to 04; task-334-02 and 03, provisional)
- parent_class: UMaterial (M_Asteroid), UMaterialInstanceConstant (MI_Asteroid), UStaticMesh (meshes)
- purpose: lit, flat-coloured, darker-than-arena rock for the asteroids of [[bp-asteroid-field]]; no texture.
- components: none (material and mesh assets).
- exposed_defaults: M_Asteroid is Default Lit, Opaque, one-sided, bUsedWithInstancedStaticMeshes true, no texture, no WPO, no emissive. BaseColor = BaseColor x (1 + Noise x NoiseTintStrength), Noise = ALU gradient Noise(world position x NoiseScale, 3 levels, signed); Roughness direct. Nanite off.

| MI_Asteroid parameter | Value (all provisional) |
|---|---|
| BaseColor | (0.10, 0.095, 0.14, 1), about 62% of the arena SideColor (0.16, 0.15, 0.22) |
| Roughness | 0.9 |
| NoiseTintStrength | 0.25 |
| NoiseScale | 0.0006 (noise cell about 1670 uu) |

- events: none.
- logic_in_bp: none.
- assets: four meshes SM_Asteroid_01..04 (Blender route, task-334-01): bounds about 100 uu (e.g. 100 x 75.75 x 91.13), 320 tris, 960 vertices (flat shading), 1 LOD, slot 0 = MI_Asteroid, simple collision 0, Nanite off, lightmap UV generation off, FBX normals imported.
- import: MCP `import_file` and Interchange gave 1 uu (100x too small, FBX unit scale 100). Imported instead with editor Python `FbxImportUI` through `AssetImportTask`, scale 1.0. Reimport trap: the import_task fallback of tool-unreal-reimport resets Generate Lightmap UVs to on; run `static_mesh_build_settings.py` after any reimport.
- revert: delete the assets after removing [[bp-asteroid-field]] from the map; nothing else references them.
- owner_agent: agent-bp
- not_verified: tint noise look on screen apart from the capture below; other lighting conditions; packaged build.
- capture (task-334-05): asteroid mean luma about 85 against rock tops 99 to 118 (one crop at 98.8 was below asteroid #1's 107.5 in shot 2), so the asteroids read darker in most crops, not all. Contact sheet Saved/Captures/task334_asteroids/contact_sheet.png.

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-asteroid-field]] — actor that places the meshes.
- [[bp-space-background]] — space layers the asteroids sit in front of.
- [[task-334-near-asteroids]] — global task with the decided values.
