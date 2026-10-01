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
- Cell types in the JSON: rock, ore, heal_ore, special_ore, barrel, spawn, exit. Command consoles and bonus chests are dropped during play, not placed in the layout ([[level-flow]]).
- Cell size and arena size: open.
- Visual build (autotiling, instancing): [[sys-grid]].

## Relations
- [[terrain-index]] — parent index.
- [[visual-style]] — camera and look.
