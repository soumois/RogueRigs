---
id: bp-pickup-ore
description: "BP_PickupOre, Blueprint of ARRPickup for the ore pickup: violet amethyst chunk (three meshes, random per drop, random rest orientation), violet trail on magnet."
parent: blueprints-fx-index
kind: doc
---
# BP_PickupOre

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupOre
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the visible ore pickup the grid drops from digging; visuals only, behaviour in C++ ([[sys-pickups]]).
- components: inherited root `PickupMesh` plus `TrailFX` (NiagaraComponent, Asset NS_OreTrail, bAutoActivate off, attached to the root, relative scale (0.3, 0.3, 0.3) like the XP nut; it inherits the root scale 1, so its effective scale is that of [[bp-pickup-xp]]). Added in task-189-05.
- exposed_defaults (task-189-05, provisional, the owner judges the look on screen): PickupType = Ore; PickupMesh StaticMesh = /Game/RogueRigs/Art/OreChunk/SM_OreChunk_01, RelativeScale3D (1, 1, 1) (was the engine cube at 0.3), CastShadow false (as the XP nut), OverrideMaterials empty (the former slot 0 override MI_GB_PickupOre is cleared, so every variant uses its own slot defaults: crystal_bright = MI_OreChunk_Bright, crystal_dark = MI_OreChunk_Dark, read back on the three meshes); RestOrientation = Full; MeshVariants = [SM_OreChunk_01, _02, _03], drawn once per life with the orientation in `StartLife` ([[sys-pickups-lifecycle]]); PopMinDistance = 25 uu and PopMaxDistance = 67 uu (task-027-02: the chunk lands within about one 67 uu cell of the broken block); other C++ defaults kept.
- events: `OnMagnetStart` activates TrailFX only. There is NO look-at rotation (unlike BP_PickupXP): it would undo the random rest orientation, so the chunk keeps it while it flies to the miner (deliberate, Orchestrator decision 2026-10-15). `OnCollected` not implemented: no sparkle for the ore.
- logic_in_bp: display only (one event).
- assets: meshes /Game/RogueRigs/Art/OreChunk/SM_OreChunk_01, _02, _03 (Meshy amethyst clusters, 396, 398 and 496 tris, 30 cm, centred pivot, two slots crystal_bright and crystal_dark, UV1 LightMap; SM_OreChunk_02 is not watertight, owner accepted; FBX scale fixed to 1 unit = 1 cm in task-189-03, source Tools/art/blender/ore_chunk.py); materials MI_OreChunk_Bright (Color and GlowColor linear (0.55, 0.1, 1.0), GlowAmount 12) and MI_OreChunk_Dark ((0.25, 0.04, 0.55), GlowAmount 6), children of M_XPNutGlow (1.2 s pulse, PulseAmount 0.4, no tick, [[bp-pickup-xp]]), the same on the three meshes; /Game/RogueRigs/FX/NS_OreTrail (task-189-04, duplicate of NS_XPTrail: ribbon, world space, width 10, lifetime 0.35 s, SpawnRate 60, particle Color linear (0.55, 0.1, 1.0, 1)). Values provisional, the owner tunes by eye. MI_GB_PickupOre (cyan) is no longer used by this Blueprint (asset kept).
- floaty_pop (task-195-02, provisional, owner 2026-10-15): class defaults PopDuration 1.6 s, PopArcHeight 250, PopMinDistance 80, PopMaxDistance 300 (were 0.35 s, 80, 25, 67), PopFallTimeRatio 2, bAttractDuringPop true, PopAttractDelay 1.2 s; PopBounceFraction 0.25 unchanged; read back on the class default object after compile and save. Landing on unbroken rock accepted for now.
- owner_agent: agent-bp
- verified: after compile (warnings as errors, no error) and save, read back through ObjectTools on the class default object and the TrailFX template: mesh, scale, CastShadow false, empty overrides, RestOrientation Full, three MeshVariants, TrailFX Asset NS_OreTrail, bAutoActivate false, scale 0.3; event graph shows OnMagnetStart -> Activate(TrailFX); BP_ArenaGrid (/Game/RogueRigs/Blueprints/Terrain/BP_ArenaGrid) still has `OrePickupClass` = BP_PickupOre_C ([[bp-arena-grid]]). Not checked: on-screen look, trail in PIE, the random variant and orientation at runtime (owner judges by eye).

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class it derives from.
- [[sys-pickups-lifecycle]] — the per-life look draw it relies on.
- [[bp-pickup-xp]] — the trail and material model it copies.
- [[bp-arena-grid]] — the grid Blueprint that spawns it.
- [[color-code]] — colour convention.
