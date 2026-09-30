---
id: sys-grid
description: "Grid terrain as built: cell data as source of truth, 6-shape autotiling, instanced meshes per asset, block destruction and neighbor swap."
parent: systems-index
kind: doc
---
# Grid

- responsibility: terrain data, rendering and destruction.
- main_classes: open (not written yet).

## Rules (approved)
- The grid is the source of truth for terrain. Using it for pathfinding and line of sight is provisional ([[sys-enemy-ai]]).
- Autotiling: a block's mesh depends on its open sides. With rotation, 6 shapes cover every case: buried, 1 open side, corner, corridor, wall end, isolated; each with a few variants.
- Top face: a few variants with a random 90° rotation (the most visible face under the near-vertical camera).
- Rendering: one Hierarchical Instanced Static Mesh per asset type; destroying a block removes instances.
- A destroyed block plays a particle burst; its neighbors swap to the shape matching their new open sides.
- Terrain damage arrives through a dedicated interface, outside GAS.
- Chaos Destruction is not used.

- data: level layout JSON from the Level-Design agent (M1); deterministic generator (M2).
- depends_on: none.
- implements: [[arena-grid]], [[destruction-rules]], [[ore-blocks]].
- gotchas: destroyed blocks are logged by source and type ([[run-log-schema]]).

## Relations
- [[systems-index]] — parent index.
