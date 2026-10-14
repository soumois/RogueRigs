---
id: task-159-02-chaser-part-index-unreal
description: "Reimport the part mesh with the PartId UV channel and read the part index from it in the animation material."
parent: task-159-chaser-anim-part-index
kind: task
status: done
---
# Chaser: part index UV in Unreal

- goal: Reimport the part mesh with the PartId UV channel and read the part index from it in the animation material.
- agent: bp
- depends_on: task-159-01-chaser-anim-part-index
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: SM_Chaser_Meshy_Anim (/Game/RogueRigs/Art/Chaser/) reimported from the new chaser_meshy_parts.fbx (bounds x -17.3..17.8, y about +-36.9, z 0..123, 3273 triangles as before, the 40 cm extended bounds kept, the slot keeps its material) with the UV channel PartId read back (number of UV channels 3 including LightMap and PartId: check through the headless Python of the reimport run, for example unreal.StaticMeshEditorSubsystem get_num_uv_channels, and report the value); M_Chaser_Meshy_Anim reads the part index from TexCoord[2] instead of the vertex colour: part = clamp(floor(u x 10), 0, 9) (u of PartId = (index + 0.5) / 10), the rest of the material unchanged (check that task-158 changed the emissive mask first if it is already done: do not undo it); docs bp-chaser updated
- done_when: The mesh reads back with the right bounds and the UV channel count; the material recompiles without error; assets saved and committed by name; doc updated; the editor is left open; the owner is told to run the game and look at the legs, arms and torso
- rules: save only the named assets; no SlateInspector, no Ctrl+S, no UI automation; never CaptureEditorImage; the existing asset cannot be re-imported through MCP import_file: close the editor ONLY with CloseMainWindow and only because I launched it this session, run the headless tool Tools/unreal/reimport_static_mesh.py (see tool-unreal-reimport), then relaunch the editor and wait for MCP; if the reimport result is 100x too large, stop and report; commit by name with message `task-159-02-chaser-part-index-unreal: <summary>`
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 56, cache write 78368, cache read 1593334, output 1662
- runs: 1
- milestone: M2
- result: part index read from UV channel 2 PartId (engine reports 3 UV sets, 10 exact bins); material compiled; not seen in game

## Relations
- [[task-159-chaser-anim-part-index]] — parent global task.
