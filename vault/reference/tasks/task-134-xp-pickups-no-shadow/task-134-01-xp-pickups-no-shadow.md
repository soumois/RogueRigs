---
id: task-134-01-xp-pickups-no-shadow
description: "Turn off the mesh shadow of the XP nut and chip Blueprints."
parent: task-134-xp-pickups-no-shadow
kind: task
status: done
---
# XP pickups: no mesh shadow

- goal: Turn off the mesh shadow of the XP nut and chip Blueprints.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: BP_PickupXP and BP_PickupXPBig: PickupMesh CastShadow false (read back after compile); nothing else changed; doc node updated. Editor open, MCP up; do not call CaptureEditorImage, Screenshot or CaptureViewport; after a component-template edit dirty the asset (toggle a value) and save again, then check git shows the .uasset modified; update the bp-pickup-xp doc node; commit by name.
- done_when: CastShadow reads false on both Blueprints after compile and save; both .uasset modified in git; doc node updated
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 24, cache write 22973, cache read 212062, output 520
- runs: 1
- milestone: M2
- result: CastShadow false on the PickupMesh of BP_PickupXP and BP_PickupXPBig (set on the CDO path), read back after compile and save.

## Relations
- [[task-134-xp-pickups-no-shadow]] — parent global task.
