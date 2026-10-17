---
id: task-230-06-rock-plate-overlay
description: "Broken-top overlay mesh: cell-sized plates tilted up, shown on heavily damaged cells."
parent: task-230-rock-slab-integration
kind: task
status: todo
---
# Broken-top overlay mesh: cell-sized plates tilted up, shown on heavily damaged cells

- goal: Broken-top overlay mesh: cell-sized plates tilted up, shown on heavily damaged cells.
- agent: 3d
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-counts.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/gdd/art-audio/art-style-guide.md; look target Art/Source/rock-look/20261017-131704-rock-look-2.png
- expected_output: `Art/Meshes/rock-slab/SM_RockPlates_01.fbx` (67 x 67 uu footprint, about 60 tris provisional), script, report, previews.
- done_when: FBX and previews exist, budget met, previews sent; committed.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
