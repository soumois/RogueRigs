---
id: task-141-01-xp-pickups-lumen-test
description: "Try to make the XP nut and chip emit light on the floor through Lumen: emissive light source flag and mesh cards."
parent: task-141-xp-pickups-lumen-test
kind: task
status: in-progress
---
# XP pickups: Lumen emissive light test

- goal: Try to make the XP nut and chip emit light on the floor through Lumen: emissive light source flag and mesh cards.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: On the PickupMesh of BP_PickupXP and BP_PickupXPBig (CDO path /Game/RogueRigs/Blueprints/Pickups/<BP>.Default__<BP>_C:PickupMesh): set the Emissive Light Source flag (property bEmissiveLightSource if it exists; use ObjectTools.list_properties to find the exact name) and, if there is a Lumen related per-component property, report it; on SM_XPNut and SM_XPChipBig raise the Max Lumen Mesh Cards of the static mesh build settings (try 12 to 24) and apply the build if the tool allows; report exactly which properties exist and which you set; if a property cannot be set through MCP, report it and do not work around it. Nothing else changes (no scale, material, shadow, collision). Then try to read whether the meshes have Lumen coverage: report anything readable (no screenshots). The owner will look in the viewport and judge. Editor OPEN (the owner is using it; keep it open, save only the assets you name), MCP up, no CaptureEditorImage, Screenshot or CaptureViewport; read every value back; update the doc node bp-pickup-xp; commit by name; the three XP tasks run one after the other, never together.
- done_when: Properties found and set read back on both Blueprints and both meshes, assets saved, doc node updated, a report of what could not be set
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-141-xp-pickups-lumen-test]] — parent global task.
