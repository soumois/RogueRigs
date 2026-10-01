---
id: sys-grid
description: "Grid terrain as built: cell data as source of truth, 6-shape autotiling, instanced meshes per asset, block destruction and neighbor swap."
parent: systems-index
kind: doc
---
# Grid

- responsibility: terrain data, rendering and destruction.
- main_classes: `ARRArenaGrid` (provisional): reads the layout JSON and builds the blocks itself, one HISM component per block type, in `OnConstruction` so the arena shows in the editor. `BP_ArenaGrid` derives from it and only sets the layout name and one mesh and material per block type; one instance of it is placed in the map.
- M1 grey box (approved): every block type uses the engine cube (`/Engine/BasicShapes/Cube`) or a `LevelPrototyping` cube, with one material instance per type colored after [[color-code]]. Final meshes and autotiling replace them later by changing references in `BP_ArenaGrid`; code and layouts stay unchanged.

## Rules (approved)
- The grid is the source of truth for terrain. Using it for pathfinding and line of sight is provisional ([[sys-enemy-ai]]).
- Autotiling: a block's mesh depends on its open sides. With rotation, 6 shapes cover every case: buried, 1 open side, corner, corridor, wall end, isolated; each with a few variants.
- Top face: a few variants with a random 90° rotation (the most visible face under the near-vertical camera).
- Rendering: one Hierarchical Instanced Static Mesh per asset type; destroying a block removes instances.
- A destroyed block plays a particle burst; its neighbors swap to the shape matching their new open sides.
- Terrain damage arrives through a dedicated interface, outside GAS.
- Chaos Destruction is not used.

- data: level layout JSON in cell coordinates from the Level-Design agent (M1); deterministic generator (M2). `CellSize` is an editable data value (starting 200 uu); arena 60 × 60 cells in M1.
- depends_on: none.
- implements: [[arena-grid]], [[destruction-rules]], [[ore-blocks]].
- gotchas: destroyed blocks are logged by source and type ([[run-log-schema]]).

## Relations
- [[systems-index]] — parent index.
