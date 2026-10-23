---
id: task-296-02-bp-shield-arm-forward
description: "Import the 296-01 mesh and texture as new assets on BP_ShieldChaserV2 (bp)."
parent: task-296-shield-v2-arm-forward
kind: task
status: todo
---
# Shield held forward in Unreal

- goal: Import the 296-01 FBX and EXR as new assets (suffix Fwd); set the mesh on BP_ShieldChaserV2 BodyMesh with the four slot overrides, repoint M_ShieldChaserV2_Anim and the two eye instances to the new texture; same import recipe and build settings as task-286-02.
- agent: bp
- depends_on: task-296-01-bake-shield-arm-forward
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md); files: report of 296-01
- expected_output: new mesh and texture assets; BP and material updated; bp-shield-chaser-v2 updated (before, after, revert)
- done_when: UV channel and build settings read back; slots read back; master recompiled; BP compiled and saved; v1 untouched; docs updated; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-296-shield-v2-arm-forward]] — parent global task.
