---
id: task-480-02-broken-pieces-facets
description: "3d: Per Q3, give the broken variants the faceted outer sides of their intact shape, plates and tilt unchanged."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: done
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides: 3d

- goal: Per Q3, give the broken variants the faceted outer sides of their intact shape, plates and tilt unchanged.
- agent: 3d
- depends_on: task-480-01-slab-pieces-chamfer-facets
- inputs: rock-render-damage (vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md), Tools/art/blender/rock_slab_broken.py
- expected_output: see done_when
- done_when: Per Q3 the broken FBX have outer sides matching the intact facets at joints, no chamfer glow mark on broken plates, tilt data unchanged; previews in the result; committed. If Q3 says unchanged, done with a one-line result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-480
- usage: included in the executor run of task-480
- runs: 1
- milestone: M2
- started: 2026-11-06T21:01:41Z
- waiting_for: none
- result: Per Q3: rock_slab_broken.py keeps the block cuts (block_list of the 10 shapes identical to the old report, rigid tilt section identical, tris unchanged 120-420) on the flat 4 uu rim footprint; wall feet now follow the facet foot chain of the intact piece of the same variant number, border feet unchanged (intact_border_vertices_kept true on z = 0, seam check pass, sag check pass 17768 rays 0 holes, all prisms closed). Rim no longer coded R = 0.5: all tops R = 1 (no chamfer glow mark); side faces carry the block seed in R for the facet tint. Winding of walls and bottoms now comes from the top triangle order, since a skewed foot can fold a hidden bottom triangle. Previews Art/Meshes/rock-slab/preview/rockbroken_*.png.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
