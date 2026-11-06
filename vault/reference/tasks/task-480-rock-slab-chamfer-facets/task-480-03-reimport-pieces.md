---
id: task-480-03-reimport-pieces
description: "Bp: Reimport the slab meshes and wire the new variants into the existing piece choice."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-480
- usage: included in the executor run of task-480
- runs: 1
- milestone: M2
- started: 2026-11-06T21:03:56Z
- waiting_for: none
- result: 14 changed meshes reimported, vertex colours Replace then plain Interchange reimport (the import task alone came back 100 times too large in 5.8, the plain reimport with the pinned scene unit restores 67 x 67 x 100; recorded for the doc). Tools/unreal/rock_slab_variants_assign.py imports SM_RockSlab_{Edge,Outer,Inner,Diagonal}_02, slot MI_RockSlab, Nanite on, and lists _01 and _02 on BP_ArenaGrid RockPieceMeshes; no C++ change, existing corner hash picks the variant. static_mesh_build_settings on all 18; check_mesh_channels: colour classes present, UV0-UV3 render as expected. -game run Saved/Captures/Wall480Geo: 20987 pieces in 1078 ISMs (2 variant slots per shape, was 633 ISMs), new geometry drawn, no new error. Full_01 left untouched.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
