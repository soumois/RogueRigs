---
id: bp-pickup-heal
description: "BP_PickupHeal, Blueprint of ARRPickup for the heal pickup: 30 uu grey-box cube, red; open for its mesh, material and type."
parent: blueprints-fx-index
kind: doc
---
# BP_PickupHeal

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the visible heal pickup the grid drops from digging; visuals only, behaviour in C++ ([[sys-pickups]]).
- components: inherited root `PickupMesh` only; none added.
- exposed_defaults: PickupType = Heal; PickupMesh StaticMesh = /Engine/BasicShapes/Cube, RelativeScale3D (0.3, 0.3, 0.3) = 30 uu, OverrideMaterials[0] = MI_GB_PickupHeal; PopMinDistance = 25 uu and PopMaxDistance = 67 uu (task-027-02: the chunk lands within about one 67 uu cell of the broken block); other C++ defaults kept.
- events: OnCollected(Location) spawns BP_HealRing at Location, the miner position ([[bp-heal-ring]]).
- logic_in_bp: display only (one event).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupHeal (instance of M_GreyBox, vector `Color` linear (1, 0.05, 0.05, 1) (red), after [[color-code]]).
- rest_orientation: `RestOrientation` = Full (task-192-02, owner 2026-10-15; [[sys-pickups-lifecycle]]); `MeshVariants` left empty. Read back on the class default object after compile and save.
- floaty_pop (task-195-02, provisional, owner 2026-10-15): class defaults PopDuration 1.6 s, PopArcHeight 250, PopMinDistance 80, PopMaxDistance 300 (were 0.35 s, 80, 25, 67), PopFallTimeRatio 2, bAttractDuringPop true, PopAttractDelay 1.2 s; PopBounceFraction 0.25 unchanged; read back on the class default object after compile and save. Landing on unbroken rock accepted for now. task-256-02 (provisional, owner 2026-10-20): PopMaxDistance = 150 (was 300), PopMinDistance 80 kept, other defaults unchanged (same change as [[bp-pickup-ore]]); read back on the class default object after compile and save.
- simple_collision (task-229-01, read only): /Engine/BasicShapes/Cube already carries one box, centre (0, 0, 0), size 100 x 100 x 100 (30 cm at the 0.3 scale), CTF_UseDefault; it fits the cube shape, so nothing was changed (engine content, shared, not saved).
- owner_agent: agent-bp
- verified: defaults read back through ObjectTools on the class default object; referenced by BP_ArenaGrid ([[bp-arena-grid]]) as `HealPickupClass`. PIE collection not driven through MCP (see [[task-006-04-bp-dig]]).

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class it derives from.
- [[sys-pickups-lifecycle]] — the rest orientation rule.
- [[bp-arena-grid]] — the grid Blueprint that spawns it.
- [[bp-pickup-ore]] — sibling Blueprint with the same pop distance.
- [[color-code]] — colour convention.
- [[bp-heal-ring]] — the ring it spawns on collection.
