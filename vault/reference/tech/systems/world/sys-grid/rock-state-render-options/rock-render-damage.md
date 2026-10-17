---
id: rock-render-damage
description: "Rock damage display (crack mask per quadrant, tilted plate overlay, health thresholds), ore crystal clusters and how a future weak_rock cell plugs in."
parent: rock-state-render-options
kind: doc
---
# Rock render: damage, ore, weak rock

## Cracks (recommended)
Damage d = 1 - remaining health / max health per cell. A dual-grid piece spans quadrants of 4 cells, so it carries 4 custom data floats (one d per quadrant); the material picks the quadrant from the piece-local position and shows the world-space crack mask where mask > 1 - d. Same world UV in every piece, so cracks run across piece seams ([[rock-render-top-texture]]). Cost: 4 floats per piece written on each damage tick of a dug cell (4 pieces, `bMarkRenderStateDirty` per touched HISM per frame); no geometry. Thresholds: cracks appear as soon as d > 0 (any block below 100 %, owner 2026-10-17), growing to full at 0.6 (proposed).

## Tilted plates
Options:
- A. Overlay HISM of a broken-top mesh (cell-sized, plates tilted, 60 triangles proposed), one instance per cell with d >= 0.6; the 4 quadrants under it are sunk below the floor top by WPO (quadrant flag). Few live instances. Recommended.
- B. Masked material discarding the quadrant: masked pixels cost early depth; rejected.
- C. Per-shape broken variants: 16 masks x 4 quadrants of combinations; rejected.
Rock dies at 0.5 dig-seconds, so plates last a few frames on rock; worth it on ore (1.5 s) and weak rock.

## Ore crystals
One cluster mesh per ore cell (2 x 2 deposit = 4 clusters, image C) in a per-chunk crystal HISM per ore type, emissive, colour from the existing `OreColorCustomDataIndex` floats; random yaw. Slab under ore is ordinary rock pieces ([[color-code]]: same base). Removed with the cell.

## Future weak_rock
A Perlin-noise layer of the generator ([[rock-look-reference]], values open) marks cells `weak_rock` (new `ERRCellType`, or a per-cell flag on rock; open). Render: solid in the dual-grid mask like rock; its quadrant d starts at a floor value (for example 0.3, open) so it is cracked while intact, matching "cracks mean damaged" ([[readability-rules]]); lower `CellHealth`. The damaged-wall protection meaning is open. No new mesh.

## Relations
- [[rock-state-render-options]] — parent note.
- [[sys-grid-deposits]] — deposits and ore colour custom data.
- [[ore-blocks]] — ore rules.
