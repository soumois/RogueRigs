---
id: task-107-02-barrel-integration-bp
description: "SM_Barrel and its textures are imported, its materials built, and BP_ArenaGrid uses them for barrel cells in L_ArenaM1."
parent: task-107-barrel-integration
kind: task
status: done
---
# Barrel import and BP_ArenaGrid

- goal: SM_Barrel and its textures are imported, its materials built, and BP_ArenaGrid uses them for barrel cells in L_ArenaM1.
- agent: bp
- depends_on: task-107-01-barrel-integration
- inputs: agent-bp (vault/reference/agents/agent-bp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), blueprints-index (vault/reference/tech/blueprints/blueprints-index.md), bp-arena-grid (vault/reference/tech/blueprints/bp-arena-grid.md), sys-grid (vault/reference/tech/systems/sys-grid.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), barrel (vault/reference/gdd/terrain/barrel.md); files Art/Meshes/barrel/SM_Barrel.fbx, Art/Meshes/barrel/SM_Barrel.report.json (key mask_contract: read it first), Art/Meshes/barrel/T_Barrel_Sticker.png, Art/Meshes/barrel/T_Barrel_Warning.png, Art/Textures/T_WearMask.png
- expected_output: Editor open with the MCP server. (1) Import SM_Barrel.fbx (keep the two UV channels UVMap and LightMap; no auto-generated lightmap UVs; no collision generated; flat normals kept) and the three textures into /Game/RogueRigs/Art/Barrel/: T_WearMask (linear, sRGB off, tiling), T_Barrel_Sticker (sRGB), T_Barrel_Warning (sRGB, alpha). (2) Materials: a master material M_BarrelPaint with parameters Color and, for lid and cap, scratch mask blending per mask_contract (colour = lerp(lid colour, lighter lid tone, (1 - mask) * 0.6), mask on UV0 at 100 cm per tile, wrap); instances for body, ring, lid, cap, and the three edge band materials as plain flat colours from the report; sticker material opaque; warning sign material Masked (opacity mask from alpha). Assign the 9 material slots of the mesh in the order of the report; slot 0 must be the body material since the grid overrides slot 0 with BarrelMaterial. (3) In BP_ArenaGrid set BarrelMesh = SM_Barrel and BarrelMaterial = the body material instance (data only, no graph logic); the level instance in L_ArenaM1 must inherit (no override). (4) Check without screenshots: the mesh has the expected slots and bounds (90 x 90.27 x 134), BP_ArenaGrid defaults read back, the map opens and the barrel HISM instance count is unchanged (16 on the layout that has them), no console error; save all assets and the map. (5) Update the doc nodes bp-arena-grid (new defaults) and write the doc nodes of any new Blueprint-side asset you consider needed under tech/blueprints (materials may be listed inside bp-arena-grid). Never call CaptureEditorImage, Screenshot or CaptureViewport.
- done_when: assets saved under /Game/RogueRigs/Art/Barrel/; BP_ArenaGrid.BarrelMesh is SM_Barrel and BarrelMaterial the body instance, the level has no override; barrel HISM instance count unchanged and no error in the log; docs updated; lint clean; committed.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 84, cache write 117203, cache read 3234836, output 708
- runs: 3
- milestone: M2
- result: SM_Barrel and its textures imported to /Game/RogueRigs/Art/Barrel/ (9 slots in order, slot 0 body, no collision, lightmap index 1), M_BarrelPaint with scratch blend on lid and cap, 7 instances, sticker opaque, warning Masked; BP_ArenaGrid BarrelMesh = SM_Barrel and BarrelMaterial = MI_Barrel_body, level inherits; 16 barrel instances unchanged, uniform scale 0.742; no log error. Open: an unused UV channel 2 from the auto lightmap generation, two MapCheck usage-flag warnings not re-run, no in-game render seen. Commits ab45fb4, d1043f8, 179bb93.

## Relations
- [[task-107-barrel-integration]] — parent global task.
