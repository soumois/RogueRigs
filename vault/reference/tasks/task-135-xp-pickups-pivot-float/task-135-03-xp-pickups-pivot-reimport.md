---
id: task-135-03-xp-pickups-pivot-reimport
description: "Reimport the two centred FBX over the existing Unreal assets so the pickups no longer float, Blueprint references kept."
parent: task-135-xp-pickups-pivot-float
kind: task
status: todo
---
# XP pickups: reimport the centred meshes

- goal: Reimport the two centred FBX over the existing Unreal assets so the pickups no longer float, Blueprint references kept.
- agent: bp
- depends_on: task-135-02-xp-pickups-pivot-export
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md)
- expected_output: SM_XPNut and SM_XPChipBig reimported in place (same asset paths, import_materials off, meshes combined); bounds read back; Blueprints still point to them with their material override instances; doc node bp-pickup-xp says the pickup meshes have their pivot at the centre, an exception to the bottom-centre pivot of asset-pipeline.
- done_when: Bounds read back centred (nut z -5.85 to 5.85, chip z -5.3 to 5.3); both Blueprints compile and save with the same mesh and the override instances in place (BP_PickupXP: M_XPNut_body, M_XPNut_hole; BP_PickupXPBig: seven M_XPChip_*); doc node updated; no screenshot tool
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-135-xp-pickups-pivot-float]] — parent global task.
