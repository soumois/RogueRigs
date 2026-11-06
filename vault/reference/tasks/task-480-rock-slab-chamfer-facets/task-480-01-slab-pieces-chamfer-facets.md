---
id: task-480-01-slab-pieces-chamfer-facets
description: "3d: Extend the intact slab piece generator per Q1 and Q2: top chamfer marked for the emissive, faceted sides, variants; FBX and previews."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
