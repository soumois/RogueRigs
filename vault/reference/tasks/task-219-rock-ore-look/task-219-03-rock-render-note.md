---
id: task-219-03-rock-render-note
description: "Tech note comparing ways to render rock damage states and ore blocks on the chunked grid HISMs, with costs and a triangle budget; no code."
parent: task-219-rock-ore-look
kind: task
status: todo
---
# Rock and ore look: rendering note

- goal: a `doc` node `rock-state-render-options`, child of [[sys-grid]], that compares how to show rock intact and in 2 damage states (from remaining cell health) and ore blocks in the engine, for track A (per-cell blocks, with the approved autotiling and top-face variants) and track B (continuous slab top with rounded outline, rebuilt on destruction).
- agent: cpp
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid.md, vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md, vault/reference/gdd/terrain/arena-grid.md, vault/reference/gdd/art-audio/visual-style.md, vault/reference/gdd/art-audio/rock-look-reference.md, vault/reference/tech/pipelines/asset-pipeline.md, vault/reference/tech/performance/perf-stress-grid-cells.md; code read only: `Source/RogueRigs/Grid/RRArenaGrid.h/.cpp`.
- expected_output: the node (under ~400 words): options (mesh swap between HISMs, per-instance custom data driving the material, decals, slab mesh per chunk), cost per option at 240 x 240 (instance counts from the M1 layout), effect on destruction updates, a proposed triangle budget per block written `proposed, owner to confirm`, a recommendation per track; Relations on both ends.
- done_when: node present, lint clean, committed; no change under `Source/`, `Content/` or `Config/` (git diff empty there); nothing in the engine is touched (owner condition).
- editor_state: any (editor not needed, no build)
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-219-rock-ore-look]] — parent global task.
