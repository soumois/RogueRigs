---
id: rock-render-piece-set
description: "Dual-grid (marching squares) slab pieces versus per-cell 6 shapes, seamless outline warp, rim, faceted sides, floor under rock. Design only."
parent: rock-state-render-options
kind: doc
---
# Rock render: piece set

## Per-cell 6 shapes by open sides (approved rule, not built)
One mesh per block cell chosen by its 4 open sides (interior, edge, corner, corridor, end, isolated). Outline corners sit at cell corners, so a mass reads as a staircase of squares; rounding happens inside one cell only, and two neighbours never share an outline vertex they both own. Fits the cube grey box, not image C.

## Dual grid (recommended)
One piece per cell corner (241 x 241 corners), chosen by the 4 cells around it (16 cases, 6 shapes): empty (no instance), full, edge (two solids on one side), outer corner (one solid), inner corner (three solids), diagonal (two opposite solids). Rotation by 90 degree steps covers the 16 cases. The outline runs along cell edges with rounded convex and concave corners, the continuous slab of image C. Variants: 2 to 3 per outlined shape; full gets 1 (flat top, no outline). Random rotation only on full (the others have a fixed orientation).

## Outline warp without seams
Outline vertices (vertex colour tag) are pushed horizontally in the material (world position offset) by a 2D noise of their own world XY. Two pieces sharing an outline vertex compute the same offset, so no seam. Amplitude about 0.2 cell (about 13 uu, estimate), below folding. Not baked into meshes: baked variants only meet at fixed edge vertices. Top interior and floor contact vertices are not moved.

## Rim and sides
- Rim: a thin band of top geometry along the outline, a lighter flat tone (A1 "thin lighter band, own flat material"). A second material slot doubles draws per piece HISM; a vertex-colour tone in the one material avoids it (preferred, owner to confirm).
- Sides: 2 rows of faceted triangles from the warped outline down to the floor top, slightly flared; only outlined pieces have them.

## Floor under rock
Today a rock block spans the floor slab and a floor instance is added on destruction ([[sys-grid]]). Rounded and warped outlines would show the void inside a block cell, so: floor slab on every non-void cell, rock pieces from floor top to `BlockHeight`, `bBlocksFillFloorSlab` false. The destroy path no longer adds floor.

## Relations
- [[rock-state-render-options]] — parent note.
- [[rock-render-counts]] — piece counts and triangle budget.
- [[art-style-guide]] — A1 rim rule.
