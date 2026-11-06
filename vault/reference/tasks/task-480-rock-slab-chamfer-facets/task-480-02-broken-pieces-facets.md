---
id: task-480-02-broken-pieces-facets
description: "3d: Per Q3, give the broken variants the faceted outer sides of their intact shape, plates and tilt unchanged."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T21:01:41Z
- waiting_for: none
- result: open

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
