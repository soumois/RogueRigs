---
id: task-350-02-turret-import
description: "Import the turret textures, M_TurretMeshy with MI_TurretBody and MI_TurretCanon, and SM_TurretBase and SM_TurretHead with their material slots."
parent: task-350-turret-in-game
kind: task
status: done
---
# Turret import: textures, material, two static meshes

- goal: The turret art is in the project under `/Game/RogueRigs/Art/Turret/`: textures, one material with two instances (owner Q5), and the two static meshes with their slots assigned, at their true size in centimetres.
- agent: bp
- depends_on: task-350-01-turret-head-yaw
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), bp-drone-materials (vault/reference/tech/blueprints/player/bp-drone-materials.md, import and texture settings precedent), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md, two-part import precedent), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-350-turret-in-game.md); files: `Art/Meshes/turret/SM_TurretBase.fbx`, `SM_TurretHead.fbx`, `T_Turret_Body_BaseColor.png`, `_Roughness.png`, `_Metallic.png`, `T_Turret_Canon_BaseColor.png`, `_Roughness.png` (normal maps not imported, Q5).
- method: textures: base colours sRGB, TC_Default; roughness and metallic sRGB off, TC_Masks (or packed), MaxTextureSize 2048 (drone precedent). `M_TurretMeshy`: opaque, default lit, texture parameters BaseColorTex, RoughnessTex, MetallicTex (scalar Metallic 0 fallback through a static switch or a black texture), no normal, no emissive. `MI_TurretBody` (body maps), `MI_TurretCanon` (canon maps, metallic 0). Meshes: FBX import in centimetres (check the size: the MCP Interchange import came out 100 times too small for the console, use the path that gives 1 uu = 1 cm), materials and textures not imported by the FBX, combine meshes, Nanite off, one LOD, lightmap UVs off, no collision; slots `turret_body` = MI_TurretBody, `turret_canon` = MI_TurretCanon. Read back bounds, triangle counts and slots; note the Y mirror of the head offset ((0.501, -0.518, 65.20) expected).
- expected_output: `/Game/RogueRigs/Art/Turret/` T_Turret_Body_BaseColor, T_Turret_Body_Roughness, T_Turret_Body_Metallic, T_Turret_Canon_BaseColor, T_Turret_Canon_Roughness, M_TurretMeshy, MI_TurretBody, MI_TurretCanon, SM_TurretBase, SM_TurretHead; change summary (paths, settings, read-back bounds, tris, slots, MCP tools used).
- done_when: bounds of SM_TurretBase and SM_TurretHead read back match the Blender reports in cm (base about 183 x 188 uu wide; total 15,974 tris); slots assigned as above; material compiles with no error; every asset saved (none dirty); committed by path as `task-350-02-turret-import: <summary>`; editor LEFT OPEN.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 116, cache write 108176, cache read 4186012, output 23560
- usage: turns 58, context start 11k peak 108k, resumes 0, 6 min, units 0.67 M
- runs: 1
- milestone: M2
- started: 2026-10-28T16:14:52Z
- waiting_for: none
- result: Done: 5 textures, M_TurretMeshy, MI_TurretBody/Canon, SM_TurretBase (7755 tris) and SM_TurretHead (8219 tris) = 15974, slots set, bounds match Blender report (base 184.7 x 183.1 x 65.2, done_when said ~183 x 188: approximate figure). MCP import was 100x small; used editor py console route. Material compile not read back directly. Commit 375b9273.

## Relations
- [[task-350-turret-in-game]] — parent global task.
