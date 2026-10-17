---
id: rock-render-counts
description: "Instance and triangle counts of the dual-grid rock look on arena_m1_fine (240 x 240), versus today's cubes, and the proposed per-piece triangle budget."
parent: rock-state-render-options
kind: doc
---
# Rock render: counts and budget

Counted by a scratch node script on `Content/RogueRigs/Data/Levels/arena_m1_fine.json` (2026-10-17, not committed). Solid = rock, ore, heal_ore, special_ore; barrels count as open (they stand on a slab).

## Layout
Cells: void 39 571, rock 17 964, heal_ore 48, barrel 16, spawn 1 (non-void 18 029). Duskite deposits are placed at build from rock: 359 x 4 = 1 436 cells ([[sys-grid-deposits]]). Per-cell shapes (approved rule): interior 13 507, edge 3 267, corner 1 058, end 151, corridor 17, isolated 12.

## Dual-grid pieces (corners with at least one solid cell)
| Shape | Instances | Triangles each (provisional, owner 2026-10-17, to measure at integration) | Triangles |
|---|---|---|---|
| full | 15 085 | 2 | 30 170 |
| edge | 3 230 | 48 | 155 040 |
| outer corner | 1 376 | 48 | 66 048 |
| inner corner | 1 280 | 64 | 81 920 |
| diagonal | 16 | 96 | 1 536 |
| total | 20 987 | | 334 714 |

158 of 225 chunks hold pieces, at most 255 per chunk. Today: 18 028 block instances of a 12-triangle cube, 216 336 triangles.

## Other instances
- Floor everywhere: 18 029 cubes, 216 348 triangles, mostly hidden under rock (today 17 at start, growing with digging).
- Ore crystal clusters, one per ore cell: 1 484 (1 436 Duskite + 48 heal) x 200 = 296 800 triangles (budget 200, max 300, proposed, owner to confirm; same scale as the ore pickup 300 target, [[asset-pipeline]]).
- Shard cluster overlay (replaces the tilted plate): 150-250 triangles per cell (owner 2026-10-17); live only on damaged cells, a few dozen at once (estimate).
- Total about 850 000 triangles in the arena; the camera sees a fraction (about 10 to 15 percent, estimate, not measured).

## Components and draws (estimates)
Rock HISMs per chunk rise from 1 to about 12 (5 shapes x 2 to 3 variants), about 1 900 components arena-wide; draws grow about 12 times on visible rock chunks unless Nanite instancing is used (open). Fewer variants is the first lever. Collision ISMs unchanged (per cell cube).

## Relations
- [[rock-state-render-options]] — parent note.
- [[rock-render-piece-set]] — the shapes counted.
- [[perf-stress-grid-cells]] — measured cost of today's cubes.
