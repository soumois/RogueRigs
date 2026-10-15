---
id: bp-pickup-ore
description: "BP_PickupOre, Blueprint of ARRPickup for the ore pickup: 30 uu grey-box cube, cyan; open for its mesh, material and type."
parent: blueprints-fx-index
kind: doc
---
# BP_PickupOre

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupOre
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the visible ore pickup the grid drops from digging; visuals only, behaviour in C++ ([[sys-pickups]]).
- components: inherited root `PickupMesh` only; none added.
- exposed_defaults: PickupType = Ore; PickupMesh StaticMesh = /Engine/BasicShapes/Cube, RelativeScale3D (0.3, 0.3, 0.3) = 30 uu, OverrideMaterials[0] = MI_GB_PickupOre; PopMinDistance = 25 uu and PopMaxDistance = 67 uu (task-027-02: the chunk lands within about one 67 uu cell of the broken block); other C++ defaults kept.
- events: none.
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupOre (instance of M_GreyBox, vector `Color` linear (0, 0.9, 1, 1) (glowing cyan), after [[color-code]]).
- assets_ore_chunk (task-189-03, provisional values the owner tunes by eye, 2026-10-15; the Blueprint itself is NOT changed yet, it still uses the cube): /Game/RogueRigs/Art/OreChunk/ holds SM_OreChunk_01, _02, _03 (Meshy amethyst clusters from Art/Meshes/OreChunk/*.fbx, 396, 398 and 496 tris, two slots crystal_bright and crystal_dark, UV1 LightMap; bounds read back in cm: 01 x +-15, y +-6.11, z +-14.96; 02 x +-15, y +-12.34, z +-13.34; 03 x +-14.13, y +-7.26, z +-15; centred on the origin; SM_OreChunk_02 is not watertight, owner accepted) and two material instances shared by the three meshes, both children of M_XPNutGlow (1.2 s pulse, PulseFrequency 0.8333, PulseAmount 0.4, no tick, [[bp-pickup-xp]]): MI_OreChunk_Bright (Color and GlowColor linear (0.55, 0.1, 1.0), GlowAmount 12) on slot crystal_bright, MI_OreChunk_Dark ((0.25, 0.04, 0.55), GlowAmount 6) on slot crystal_dark; set as the mesh slot defaults on all three. Duskite violet, bluer than the lilac (0.8, 0.25, 1.0) of the big XP chip; GlowAmount copies the chip core_bright and core_dark (12 and 6) as a starting point. Fix found on import: the first FBX export (task-189-02) held metres (30 cm = 0.3), so Unreal imported 100 times too small; Tools/art/blender/ore_chunk.py now works in 1 unit = 1 cm like the XP meshes and the three FBX were re-exported (same geometry). Not checked visually.
- assets_ore_trail (task-189-04, provisional, owner tunes by eye, 2026-10-15; BP_PickupOre is NOT changed yet, the TrailFX component comes in task-189-05): /Game/RogueRigs/FX/NS_OreTrail, a duplicate of NS_XPTrail ([[bp-pickup-xp]]: Ribbon emitter, world space, width 10, lifetime 0.35 s, SpawnRate 60 per second kept, read back 60). Only the particle Color (InitializeParticle) changed: linear (0.55, 0.1, 1.0, 1), the colour of MI_OreChunk_Bright (read back 0.55, 0.1, 1, 1; NS_XPTrail still (0, 0.9, 1)). System compile state UpToDate, no errors or warnings. Not checked visually.
- owner_agent: agent-bp
- verified: defaults read back through ObjectTools on the class default object; referenced by BP_ArenaGrid ([[bp-arena-grid]]) as `OrePickupClass`. PIE collection not driven through MCP (see [[task-006-04-bp-dig]]).

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class it derives from.
- [[bp-arena-grid]] — the grid Blueprint that spawns it.
- [[color-code]] — colour convention.
