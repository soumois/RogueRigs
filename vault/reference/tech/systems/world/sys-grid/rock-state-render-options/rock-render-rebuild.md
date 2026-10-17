---
id: rock-render-rebuild
description: "Dual-grid rock rebuild on destruction: corner pieces changed per broken cell, chunk HISM add and remove, render-state dirtying, decoupled collision maps."
parent: rock-state-render-options
kind: doc
---
# Rock render: rebuild on destruction

## Per broken cell
- Collision: unchanged, the cell's collision ISM instance is removed (remove at swap).
- Render: the 4 corners of the cell change their 4-cell mask. Each corner: remove its old piece instance, add the new shape (none when the mask becomes empty). At most 4 removes and 4 adds, in at most 4 chunks when the cell touches a chunk corner (1 chunk for 15 x 15 of the 16 x 16 cells of a chunk, about 88 percent, when a corner on a chunk line belongs to the higher chunk).
- Same-shape case (rare): an `UpdateInstanceTransform` rotation is enough.
- Floor: nothing (floor already under every non-void cell, [[rock-render-piece-set]]).
- Crystals: the ore cell's cluster instance removed.

## Data
- New per-corner map: corner index to (chunk visual, instance index), plus index to corner per HISM, patched on remove at swap as `RemoveCellInstance` does today for cells.
- Render and collision indices are no longer in lockstep: `CellInstanceIndex` serves collision only; shake, marked and colour custom data move to per-corner pieces (4 quadrant floats each).

## Dirtying and cost
- Each touched piece HISM calls `MarkRenderStateDirty` once per frame (the task-202-01 rule already in [[sys-grid]]), so a frame breaking many cells recreates each touched proxy once.
- Cost per broken cell: about 4 times today's one render remove and one floor add (one dig destruction about 0.04 to 0.07 ms after the navigation fix, [[perf-stress-grid-cells]]), so about 0.2 to 0.3 ms (estimate, not measured). Explosion of 64 cells: touched corners shared, about 100 to 150 piece changes (estimate).
- Proxy recreation cost grows with the HISMs touched: up to about 12 piece HISMs per chunk may be recreated in one explosion frame (estimate). Batching all removes then all adds per frame (a pending list flushed once) bounds it.

## Relations
- [[rock-state-render-options]] — parent note.
- [[sys-grid-api]] — destruction entry points that would call the rebuild.
- [[sys-automation-tests]] — instance-map tests to rewrite.
