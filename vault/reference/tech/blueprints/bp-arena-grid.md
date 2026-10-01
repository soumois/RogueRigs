---
id: bp-arena-grid
description: "BP_ArenaGrid, data-only Blueprint of ARRArenaGrid holding the arena_m1 layout name, cube mesh and grey-box materials; open to see its defaults."
parent: blueprints-index
kind: doc
---
# BP_ArenaGrid

- asset_path: /Game/RogueRigs/Blueprints/Terrain/BP_ArenaGrid
- parent_class: ARRArenaGrid (/Script/RogueRigs.RRArenaGrid)
- purpose: turns the C++ grid into the M1 grey-box asteroid; dropped in a level, it builds the terrain from the layout.
- components: inherited from C++ only (GridRoot and one hierarchical instanced mesh per cell type: floor, rock, ore, heal ore, special ore, barrel); none added.
- exposed_defaults: LayoutName = arena_m1; FloorMesh, RockMesh, OreMesh, HealOreMesh, SpecialOreMesh, BarrelMesh = /Engine/BasicShapes/Cube; FloorMaterial = MI_GB_Floor; RockMaterial = MI_GB_Rock; OreMaterial = MI_GB_Ore; HealOreMaterial = MI_GB_HealOre; SpecialOreMaterial = MI_GB_SpecialOre; BarrelMaterial = none (no barrel material yet); CellSize 200, BlockHeight 100, FloorThickness 200, MeshBaseSize 100 (C++ defaults).
- events: none.
- logic_in_bp: none
- assets: /Engine/BasicShapes/Cube; materials in /Game/RogueRigs/Materials/GreyBox/.
- owner_agent: agent-bp
- verified: spawned in a level, instance counts floor 2228, rock 869, heal ore 8 (the arena_m1 expected counts).
- placed_in: /Game/RogueRigs/Maps/L_ArenaM1 (one instance at the origin, with a directional light, a sky light at intensity 0.3, no sky atmosphere so the void is dark, a PlayerStart at (6100, 6100, 120) on the spawn cell (30, 30), game mode override BP_TopDownGameMode). Verified: reopened map instance counts floor 2228, rock 869, heal ore 8; PIE spawns BP_TopDownCharacter near (6100, 6100) at z 98, on the floor.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-grid]] — the C++ system this Blueprint configures.
