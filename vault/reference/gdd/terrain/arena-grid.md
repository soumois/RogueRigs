---
id: arena-grid
description: "The arena is a grid of blocks: placement, block height, open floor, void edge, and where layouts come from."
parent: terrain-index
kind: spec
status: approved
---
# Arena grid

- The arena is a grid. Rock and ore are placed, and destroyed, one block at a time.
- Blocks are always low, so nothing hides a unit from the near-vertical camera.
- Floor is clean: no scattered pebbles.
- Arena edge: a cliff into open space (stars, planet). No indestructible rock in the rock biome.
- Layouts: M1 uses one fixed layout written as grid JSON by the Level-Design agent; M2 uses a deterministic C++ generator (no runtime AI).
- Arena shape: an irregular asteroid inside the grid, never a rectangle. Cells outside it are void: no floor, no block; the floor edge is the cliff.
- Cell types in the layout: void, rock, ore, heal_ore, special_ore, barrel, spawn (the miner's start). Robots, consoles, bonus chests and the shuttle are placed during play, not in the layout ([[level-flow]]). Format: [[level-layout-schema]].
- M1 arena: 240 × 240 cells of 67 uu, the same world size as the former 80 × 80 cells of 200 uu (owner 2026-10-06: finer, more organic destruction like DRGS slabs).
- One cell = one block = one dig unit. Passages in layouts are at least 3 cells (200 uu) wide, and every unit, the heavy robot included, fits through a 3-cell corridor (owner 2026-10-06, confirmed by the owner after handover-2026-10-06: a single 67 uu cell is narrower than some units).
- Fine layout `arena_m1_fine`: the old layout upscaled 3 × 3, rock and cliff edges then roughened by a deterministic noise rule so they are irregular; ore remains clusters, heal ore is 12 veins of 2 × 2 cells (shrunk from 3 × 3 on 2026-10-10), barrels single cells ([[layout-generator-recipe]]).
- Cell size: a data constant in the grid (Unreal units), tunable in the editor without changing layouts. Value 67 uu (owner 2026-10-06; was 200 uu). The flow field updates incrementally and spread over frames: digging only opens cells, so a path that is 2–3 frames late is still valid (owner 2026-10-06).
- Layouts are written in cell coordinates only.
- Visual build (autotiling, instancing): [[sys-grid]].

## Relations
- [[terrain-index]] — parent index.
- [[layout-generator-recipe]] — how the fine layout is derived.
- [[visual-style]] — camera and look.
