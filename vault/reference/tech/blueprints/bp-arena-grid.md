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
- exposed_defaults: LayoutName = arena_m1; FloorMesh, RockMesh, OreMesh, HealOreMesh, SpecialOreMesh, BarrelMesh = /Engine/BasicShapes/Cylinder (other meshes Cube); OrePickupClass = BP_PickupOre_C; HealPickupClass = BP_PickupHeal_C; FloorMaterial = MI_GB_Floor; RockMaterial = MI_GB_Rock; OreMaterial = MI_GB_Ore; HealOreMaterial = MI_GB_HealOre; SpecialOreMaterial = MI_GB_SpecialOre; BarrelMaterial = MI_GB_Barrel (orange); CellSize 200, BlockHeight 100, FloorThickness 200, MeshBaseSize 100 (C++ defaults).
- events: none.
- logic_in_bp: none
- assets: /Engine/BasicShapes/Cube; materials in /Game/RogueRigs/Materials/GreyBox/. Since task-016-03 the rock, ore, heal ore and special ore instances MI_GB_* derive from M_GreyBoxMarked (same Color and Roughness parameters as M_GreyBox) which adds a pulsing white emissive (sine of Time, 0.2 to 1, times 3) multiplied by PerInstanceCustomData 0: marked blocks (custom data 1) pulse white, others stay unchanged ([[sys-objectives]]). Verified in PIE: 1 marked instance of the rock HISM held custom data 1; pulse not viewed on screen.
- fine grid look (task-029-06): CellSize is 67 and the grid is 240 × 240 (`arena_m1_fine`); no Blueprint default changed. `M_GreyBoxMarked` (parent of rock, ore, heal ore and special ore instances) now multiplies its `Color` by a block shade: a Custom node (BlockShade) from the mesh UV and `PerInstanceRandom` gives each instance a brightness of 0.85 to 1.15 and darkens the face border (smoothstep 0.36 to 0.5 of the distance from the face center, down to 50 %), so single 67 uu blocks read as separate beveled stones with a seam. The marked pulse (emissive times custom data 0) is unchanged and still covers the whole face. Floor and barrel stay on `M_GreyBox` (no shade). Per-instance rotation is not available (C++ would have to rotate instances, not done).
- owner_agent: agent-bp
- verified: PIE and editor barrel instance count 16 (task-011-06); the level instance had stale overrides of BarrelMesh and BarrelMaterial, reset to inherit; instance counts floor 1525 (open cells only), rock 2192, heal ore 12, ore, special ore and barrel 0 (re-read at editor level, task-006-04).
- placed_in: /Game/RogueRigs/Maps/L_ArenaM1 (one instance at the origin, with a directional light, a sky light at intensity 0.3 (real-time capture off), no sky atmosphere so the void is dark, a PlayerStart at (8100, 8100, 120) on the spawn cell (40, 40), game mode override BP_TopDownGameMode). Verified: map instance counts floor 1525, rock 2192, heal ore 12; PIE spawns BP_TwinStickCharacter at (8100, 8100) at z 90, on the spawn cell.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-grid]] — the C++ system this Blueprint configures.
- [[bp-pickup-heal]] — pickup Blueprint set as HealPickupClass.
- [[bp-pickup-ore]] — pickup Blueprint set as OrePickupClass.
