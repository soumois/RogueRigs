---
id: task-230-02-rock-slab-pieces
description: "Dual-grid slab piece meshes: full, edge, outer corner, inner corner, diagonal; Blender script."
parent: task-230-rock-slab-integration
kind: task
status: todo
---
# Dual-grid slab piece meshes: full, edge, outer corner, inner corner, diagonal; Blender script

- goal: Dual-grid slab piece meshes: full, edge, outer corner, inner corner, diagonal; Blender script.
- agent: 3d
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-piece-set.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-counts.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/tech/pipelines/asset-pipeline.md, vault/reference/gdd/art-audio/art-style-guide.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/agents/art-3d-lessons.md; look target Art/Source/rock-look/20261017-131704-rock-look-2.png
- expected_output: `Tools/art/blender/rock_slab_pieces.py`, FBX per shape in `Art/Meshes/rock-slab/` (SM_RockSlab_<Shape>_01), report.json, previews from the game camera; one cell = 67 uu, top at 100 uu over the floor top, pieces centred on a cell corner; rounded organic outline inside the piece, edges match between neighbours; faceted low-poly sides; top flat (texture from world UV); vertex colour R = rim band, G = side faces; LightMap UV; triangle counts within the provisional budget of rock-render-counts.
- done_when: all 5 FBX and previews exist; seams match when pieces are tiled (preview of a test layout); budget met; previews sent to the owner; committed.
- editor_state: any
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
