---
id: task-480-03-reimport-pieces
description: "Bp: Reimport the slab meshes and wire the new variants into the existing piece choice."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: todo
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides: bp

- goal: Reimport the slab meshes and wire the new variants into the existing piece choice.
- agent: bp
- depends_on: task-480-02-broken-pieces-facets
- inputs: tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-rock-piece-data (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-piece-data.md)
- expected_output: see done_when
- done_when: All changed slab meshes reimported with vertex colours and UV channels as before, Nanite kept; new variants placed by the existing generation (cpp change only if the variant choice requires it, same subtask, at most 3 files); a -game run shows the new geometry, no new error; committed.
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
