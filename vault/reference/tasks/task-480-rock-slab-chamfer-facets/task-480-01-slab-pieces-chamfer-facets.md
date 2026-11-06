---
id: task-480-01-slab-pieces-chamfer-facets
description: "3d: Extend the intact slab piece generator per Q1 and Q2: top chamfer marked for the emissive, faceted sides, variants; FBX and previews."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: done
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides: 3d

- goal: Extend the intact slab piece generator per Q1 and Q2: top chamfer marked for the emissive, faceted sides, variants; FBX and previews.
- agent: 3d
- depends_on: none
- inputs: bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), handover-2026-10-18-rock-slab (vault/reference/production/handover-2026-10-18-rock-slab.md), rock-state-render-options (vault/reference/tech/systems/world/sys-grid/rock-state-render-options.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), Tools/art/blender/rock_slab_pieces.py, Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png
- expected_output: Regenerated intact piece FBX in place with the new variants, Blender previews of each shape (top and low side view), triangle counts before and after.
- done_when: Facts of Q1/Q2 checked on the real files first (stop and ask the owner only if one contradicts the approved proposal); every intact shape has the chamfer and faceted sides per Q1/Q2; top outline seen from above and heights unchanged; facet offsets return to zero at piece borders so neighbours join; previews and triangle counts in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-480
- usage: included in the executor run of task-480
- runs: 1
- milestone: M2
- started: 2026-11-06T20:54:57Z
- waiting_for: none
- result: Facts checked on the real files, no contradiction with Q1/Q2 (BlockHeight 153 = mesh z scale 1.53, so the chamfer drop is 5.23 mesh units for 8 world uu). rock_slab_pieces.py extended: chamfer 8 x 8 uu along the unchanged outline (R = 1 mark), sides as 2 large facets per outline chain (widths 16-46 uu, on outline vertices), interior columns offset 0-5 uu, up to 6 deg tilt, 35 percent ridges, facet random value in vertex R of side faces; end columns fixed (seam check 114 pairs, 0 mismatch, variants mixed). SM_RockSlab_{Edge,Outer,Inner,Diagonal}_02 new; Full has no side so no _02. Tris before -> after: full 2 -> 2, edge 43 -> 35, outer 42 -> 34, inner 58 -> 44, diagonal 86 -> 70. Previews Art/Meshes/rock-slab/preview/rockslab_<shape>_<variant>_{top,low_side}.png and rockslab_layout_low_side.png. Blender previews are mesh checks, no style-guide on them.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
