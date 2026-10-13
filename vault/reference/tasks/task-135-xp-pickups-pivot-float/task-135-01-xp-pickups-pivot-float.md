---
id: task-135-01-xp-pickups-pivot-float
description: "Stop the XP nut and chip from floating: put their pivot at their centre like the old cube."
parent: task-135-xp-pickups-pivot-float
kind: task
status: todo
---
# XP pickups: fix the floating pivot

- goal: Stop the XP nut and chip from floating: put their pivot at their centre like the old cube.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: SM_XPNut and SM_XPChipBig re-imported with the pivot at the geometric centre: first try the FBX import translation offset of the static mesh import (Z +5.85 cm for the nut, +5.3 cm for the chip; bounds read back must be z -5.85..5.85 and z -5.3..5.3); if the import tool cannot, report it and stop (the Blender export with a centred pivot is then a separate 3d task); the doc node explains that the pickup pivot is the centre, an exception to the bottom-centre pivot of asset-pipeline. Blueprint structure untouched. Editor open, MCP up; do not call CaptureEditorImage, Screenshot or CaptureViewport; after a component-template edit dirty the asset (toggle a value) and save again, then check git shows the .uasset modified; update the bp-pickup-xp doc node; commit by name.
- done_when: Both meshes report centred bounds after reimport; Blueprints compile and save; doc node updated; or a report that the import cannot offset
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-135-xp-pickups-pivot-float]] — parent global task.
