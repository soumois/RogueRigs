---
id: task-230-06-rock-plate-overlay
description: "Broken variants of the 5 slab pieces as a fractured volume: solid full-height prisms, faceted sides, same borders as the intact pieces."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Broken variants of the slab pieces

- goal: Broken variants of the 5 dual-grid slab pieces (owner 2026-10-18, no added matter): same outline and border crossing points as SM_RockSlab_<Shape>_01, the whole volume fractured into solid prisms down to the ground (see owner_correction_2); the material sinks and tilts each block toward the empty side, never below 60 uu.
- agent: 3d
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-counts.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/gdd/art-audio/art-style-guide.md; look target Art/Source/rock-look/20261017-131704-rock-look-2.png
- owner_correction (2026-10-18, after 410e09a6): each plate tilts rigidly around its pivot (edge nearest an intact cell), pivot stored in the mesh for the material, random per-plate angle and height offset so plate edges show; more irregular plate cut with a second cut variant; rim band coded apart from plate tops so it stays visible.
- owner_correction_2 (2026-10-18, after 5ed04bf2: breaking only the surface leaves holes; reference DRG Survivor): fracture the whole volume into solid prisms extruded to the ground (Voronoi-like top cut, about 8 to 12 blocks per cell, minimum block size, no slivers); sides toward the void made of the prism outer faces (faceted columns), intact pieces keep smooth sides; each block moves rigidly (sink + small tilt around a stored pivot + random offset), a gap shows a neighbour side, never a hole; 300-450 tris per piece (estimate, provisional). Design: [[rock-render-damage]].
- expected_output: `Art/Meshes/rock-slab/SM_RockSlabBroken_<Shape>_{01,02}.fbx` for Full, Edge, Outer, Inner, Diagonal (replace version 3), script `Tools/art/blender/rock_slab_broken.py`, report `SM_RockSlabBroken.report.json` (per-block channel layout, pivot, tri counts), previews with simulated sink and tilt at the same cameras as version 3.
- done_when: 10 FBX exist; every block is a closed prism reaching the ground; borders match the intact pieces and each other (seam check); no block thinner than the minimum size; no visible hole at full sag (block tops >= 60 uu); previews rendered; committed.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open. Version 4 (fractured volume) in progress 2026-10-18. Superseded: version 3 surface plates 5ed04bf2 (holes), SM_RockPlates_01 (bc679aa9, 41e0f547) and the bent-sheet version 410e09a6.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
- [[rock-render-damage]] — the broken-piece design this subtask builds.
