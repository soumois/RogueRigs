---
id: rock-render-damage
description: "Rock damage display (crack mask, sagging broken pieces, thresholds), ore crystal clusters and how a future weak_rock cell plugs in."
parent: rock-state-render-options
kind: doc
---
# Rock render: damage, ore, weak rock

## Cracks (recommended)
Damage d = 1 - remaining health / max health per cell. A dual-grid piece spans quadrants of 4 cells, so it carries 4 custom data floats (one d per quadrant); the material picks the quadrant from the piece-local position and shows the world-space crack mask where mask > 1 - d. Same world UV in every piece, so cracks run across piece seams ([[rock-render-top-texture]]). Cost: 4 floats per piece written on each damage tick of a dug cell (4 pieces, `bMarkRenderStateDirty` per touched HISM per frame); no geometry. Thresholds: cracks appear as soon as d > 0 (any block below 100 %, owner 2026-10-17), growing to full at 0.6 (proposed).

## Broken pieces (owner 2026-10-18, replaces the overlay and the shard cluster)
No added matter: a damaged wall breaks and sags in place ([[rock-look-reference]], `drgs_damaged_row_junction.png`). Fractured volume (owner 2026-10-18, replaces the surface plates: they left holes): each of the 5 dual-grid shapes has a broken variant made of solid prisms, a Voronoi-like top-view cut extruded to the ground, about 8 to 12 blocks per cell, minimum block size (no thin slivers); same outline and border crossing points as the intact piece. Side faces toward the void are the outer faces of these prisms, so the cliff reads as faceted columns; intact pieces keep smooth sides. A piece uses its broken variant as soon as one of its 4 corner cells has d > 0. The material moves each block rigidly (WPO, broken variants only): sinks by sag = bilinear damage of the 4 corners at the block centre x a factor growing toward the empty side, capped so its top never goes below 60 uu (minimum height, owner), plus a small tilt around its stored pivot and a random per-block offset, so block edges show; a gap shows a neighbour's side, never a hole. Rim band coded apart from block tops. Next to an intact cell the sag is 0 (smooth junction); between two damaged cells the values match; toward the void the wall slumps. Computed from world position and corner data, so shared borders move together. Broken pieces are drawn without Nanite (owner 2026-10-19: the WPO sag did not draw under Nanite, [[sys-grid-rock-pieces]]). Cost: 300-450 tris per broken piece (estimate, provisional, measured in task-230-12), only around damaged cells.
Rock dies at 0.5 dig-seconds, so plates last a few frames on rock; worth it on ore (1.5 s) and weak rock.

## Ore crystals
One cluster mesh per ore cell (2 x 2 deposit = 4 clusters, image C) in a per-chunk crystal HISM per ore type, emissive, colour from the existing `OreColorCustomDataIndex` floats; random yaw. Slab under ore is ordinary rock pieces ([[color-code]]: same base). Removed with the cell.

## Future weak_rock
A Perlin-noise layer of the generator ([[rock-look-reference]], values open) marks cells `weak_rock` (new `ERRCellType`, or a per-cell flag on rock; open). Render: solid in the dual-grid mask like rock; its quadrant d starts at a floor value (for example 0.3, open) so it is cracked while intact, matching "cracks mean damaged" ([[readability-rules]]); lower `CellHealth`. Its total is capped by a maximum share of the rock (owner 2026-10-17, value open). No new mesh.

## Relations
- [[rock-state-render-options]] — parent note.
- [[sys-grid-rock-damage]] — the data layout, swap and crystals as built (task-230-08).
- [[sys-grid-deposits]] — deposits and ore colour custom data.
- [[ore-blocks]] — ore rules.
