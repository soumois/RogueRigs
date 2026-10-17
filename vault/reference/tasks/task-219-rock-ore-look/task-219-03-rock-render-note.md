---
id: task-219-03-rock-render-note
description: "Tech note comparing ways to render rock damage states and ore blocks on the chunked grid HISMs, with costs and a triangle budget; no code."
parent: task-219-rock-ore-look
kind: task
status: todo
---
# Rock and ore look: rendering note

- goal: a `doc` node `rock-state-render-options`, child of [[sys-grid]], on rendering the owner-chosen look (image C, continuous slab with rounded organic outline, task-219-04) on the grid: dual-grid (marching squares) tile pieces placed at cell corners and chosen by the 4 cells around (6 base shapes, replacing the approved per-cell 6 shapes by open sides), outline vertices warped by world-position noise so neighbours match without seams, piece variants and random rotation, lighter rim as geometry with its own flat material, faceted low-poly sides; damage states from remaining cell health: world-space crack mask driven by per-instance custom data, and a broken-top mesh variant for tilted plates; ore crystals as instanced clusters on deposit cells (owner 2026-10-17).
- agent: cpp
- depends_on: task-219-05-rock-look-c-review
- inputs: vault/reference/tech/systems/world/sys-grid.md, vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md, vault/reference/gdd/terrain/arena-grid.md, vault/reference/gdd/art-audio/visual-style.md, vault/reference/gdd/art-audio/rock-look-reference.md, vault/reference/tech/pipelines/asset-pipeline.md, vault/reference/tech/performance/perf-stress-grid-cells.md; code read only: `Source/RogueRigs/Grid/RRArenaGrid.h/.cpp`.
- expected_output: the node (under ~400 words, split into children if needed): piece set and how it maps to cell states, rebuild on destruction (which pieces change, cost per broken cell, chunk HISM updates), crack and plate options with costs, instance and triangle counts at 240 x 240 from the M1 layout, a proposed triangle budget per piece written `proposed, owner to confirm`, risks and a recommendation; how the future weak-rock cell type would plug in; Relations on both ends.
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
