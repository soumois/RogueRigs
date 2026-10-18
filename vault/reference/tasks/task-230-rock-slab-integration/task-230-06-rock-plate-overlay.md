---
id: task-230-06-rock-plate-overlay
description: "Broken variants of the 5 slab pieces: thick angular plates, dark base to 60 uu, same borders as the intact pieces."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Broken variants of the slab pieces

- goal: Broken variants of the 5 dual-grid slab pieces (owner 2026-10-18, replaces the shard cluster of bc679aa9 and 41e0f547, rejected: added matter looks wrong): same outline and border crossing points as SM_RockSlab_<Shape>_01, top pre-cut into thick angular plates, a dark solid base up to 60 uu, the material sags the plates toward the empty side.
- agent: 3d
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-counts.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/gdd/art-audio/art-style-guide.md; look target Art/Source/rock-look/20261017-131704-rock-look-2.png
- owner_correction (2026-10-18, after 410e09a6): each plate tilts rigidly around its pivot (edge nearest an intact cell), pivot stored in the mesh for the material, random per-plate angle and height offset so plate edges show; more irregular plate cut with a second cut variant; rim band coded apart from plate tops so it stays visible.
- expected_output: `Art/Meshes/rock-slab/SM_RockSlabBroken_<Shape>_01.fbx` for Full, Edge, Outer, Inner, Diagonal, script `Tools/art/blender/rock_slab_broken.py`, report, previews with a simulated sag; 80-150 tris each (estimate).
- done_when: 5 FBX exist; borders match the intact pieces and each other (seam check); no hole down to 60 uu under the plates at full sag; previews sent; committed.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
