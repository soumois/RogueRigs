---
id: task-230-11-rock-slab-assign
description: "Import pieces, plates and textures; assign meshes and materials in BP_ArenaGrid."
parent: task-230-rock-slab-integration
kind: task
status: todo
---
# Import pieces, plates and textures; assign meshes and materials in BP_ArenaGrid

- goal: Import pieces, plates and textures; assign meshes and materials in BP_ArenaGrid.
- agent: bp
- depends_on: task-230-10-heal-crystal-material
- inputs: vault/reference/tech/blueprints/world/bp-arena-grid.md, vault/reference/agents/bp-mcp-lessons.md, vault/reference/tech/pipelines/tool-unreal-reimport.md; FBX and textures from the 02, 05, 06 results
- expected_output: assets imported under Content/RogueRigs/Environment/Rock/ with Nanite on the pieces; BP_ArenaGrid piece meshes, plate mesh, crystal meshes and materials set; bp-arena-grid doc updated.
- done_when: BP compiled and saved; doc updated; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
