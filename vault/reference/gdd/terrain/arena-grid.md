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
- M1 arena: 60 × 60 cells.
- One cell = one block = one dig unit. Every unit, the heavy robot included, fits through a one-cell corridor: the collision diameter of every robot is smaller than a cell.
- Cell size: a data constant in the grid (Unreal units), tunable in the editor without changing layouts. Starting value 200 uu (provisional).
- Layouts are written in cell coordinates only.
- Visual build (autotiling, instancing): [[sys-grid]].

## Relations
- [[terrain-index]] — parent index.
- [[visual-style]] — camera and look.
