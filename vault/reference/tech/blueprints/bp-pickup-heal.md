---
id: bp-pickup-heal
description: "BP_PickupHeal, Blueprint of ARRPickup for the heal pickup: 30 uu grey-box cube, red; open for its mesh, material and type."
parent: blueprints-index
kind: doc
---
# BP_PickupHeal

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the visible heal pickup the grid drops from digging; visuals only, behaviour in C++ ([[sys-pickups]]).
- components: inherited root `PickupMesh` only; none added.
- exposed_defaults: PickupType = Heal; PickupMesh StaticMesh = /Engine/BasicShapes/Cube, RelativeScale3D (0.3, 0.3, 0.3) = 30 uu, OverrideMaterials[0] = MI_GB_PickupHeal; PopMinDistance = 25 uu and PopMaxDistance = 67 uu (task-027-02: the chunk lands within about one 67 uu cell of the broken block); other C++ defaults kept.
- events: none.
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupHeal (instance of M_GreyBox, vector `Color` linear (1, 0.05, 0.05, 1) (red), after [[color-code]]).
- owner_agent: agent-bp
- verified: defaults read back through ObjectTools on the class default object; referenced by BP_ArenaGrid ([[bp-arena-grid]]) as `HealPickupClass`. PIE collection not driven through MCP (see [[task-006-04-bp-dig]]).

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class it derives from.
- [[bp-arena-grid]] — the grid Blueprint that spawns it.
- [[color-code]] — colour convention.
