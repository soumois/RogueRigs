---
id: task-160-01-chaser-no-shadow
description: "Stop the chaser from casting a shadow: BodyMesh and the shield shell cast no shadow."
parent: task-160-chaser-no-shadow
kind: task
status: in-progress
---
# Chaser: no cast shadow

- goal: Stop the chaser from casting a shadow: BodyMesh and the shield shell cast no shadow.
- agent: bp
- depends_on: none
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In BP_Chaser and BP_ShieldChaser the BodyMesh component (and the ShieldShell of the shield variant) has CastShadow false (also bCastDynamicShadow false, bCastStaticShadow false, bCastContactShadow false, bAffectDistanceFieldLighting false and bCastInsetShadow false if those properties exist) so no shadow, no Lumen shadow-casting mesh; the EyeMesh stays hidden; docs bp-chaser and bp-shield-chaser updated
- done_when: Both Blueprints compile and save; the values read back; the docs updated; the editor stays open; the owner is told to look at the ground under a chaser in game (no shadow)
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-160-chaser-no-shadow]] — parent global task.
