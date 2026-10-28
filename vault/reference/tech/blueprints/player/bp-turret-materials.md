---
id: bp-turret-materials
description: "M_TurretMeshy, MI_TurretBody, MI_TurretCanon, turret textures and SM_TurretBase and SM_TurretHead: paths, parameters, slots, bounds, import route. Open to tune the turret look."
parent: blueprints-player-index
kind: doc
---
# M_TurretMeshy, instances and turret meshes

- asset_path: /Game/RogueRigs/Art/Turret/: material M_TurretMeshy; instances MI_TurretBody, MI_TurretCanon; textures T_Turret_Body_BaseColor, T_Turret_Body_Roughness, T_Turret_Body_Metallic, T_Turret_Canon_BaseColor, T_Turret_Canon_Roughness; meshes SM_TurretBase, SM_TurretHead.
- parent_class: Material (not a Blueprint). Task [[task-350-02-turret-import]]; used by [[bp-turret]]; source meshes and maps in [[art-prompts-turret]] (`Art/Meshes/turret/`).
- textures (read back): 2048 px each. Base colour: sRGB on, TC_Default. Roughness and metallic: sRGB off, TC_Masks. Normal maps not imported (owner Q5).
- M_TurretMeshy: opaque, default lit, no normal, no emissive. Parameters: texture `BaseColorTex`, `RoughnessTex`, `MetallicTex`, scalar `MetallicAmount` (default 1). Metallic = MetallicTex.R x MetallicAmount; roughness = RoughnessTex.R. Compile result not read back directly.
- MI_TurretBody: body maps, MetallicAmount 1. MI_TurretCanon: canon base colour and roughness, body metallic map kept, MetallicAmount 0.
- meshes (import: combine meshes, materials and textures not imported, lightmap UVs off, Nanite off, 1 LOD, simple collision removed):
  - SM_TurretBase: bounds X ±92.361, Y ±91.552, Z 0..65.2 (184.7 x 183.1 x 65.2; the "about 183 x 188" in the brief was approximate, the Blender report matches); 7,755 tris, 16,385 verts; slot turret_body = MI_TurretBody.
  - SM_TurretHead: bounds X -43.998..95.628, Y -67.605..67.825, Z -14.712..90.944; 8,219 tris, 16,702 verts; slots turret_body = MI_TurretBody, turret_canon = MI_TurretCanon.
  - Total 15,974 tris. The Unreal import mirrors Y, so the head offset in the base frame is (0.501, -0.518, 65.20).
- import_route: `StaticMeshTools.import_file` came out 100x too small; editor Python (`FbxImportUI` + `AssetImportTask`, run with `py "<script>"` in the status-bar Cmd box) gave the right scale; a mesh first imported through MCP must be deleted before reimport; slots set with `StaticMeshTools.set_material` per slot (Python `static_materials` edits did not stick). Recorded in [[unreal-mcp-quirks]].
- owner_agent: agent-bp
- verified: scale, bounds, triangle and vertex counts, slots and texture settings read back by the import agent. Not verified: material compile result, tangent warnings on import (harmless, no normal map).

## Relations
- [[blueprints-player-index]] — parent index.
- [[bp-turret]] — the Blueprint that uses these meshes and materials.
- [[art-prompts-turret]] — source meshes, maps and cut report.
- [[task-350-02-turret-import]] — the task that imported them.
- [[unreal-mcp-quirks]] — the import route quirks.
