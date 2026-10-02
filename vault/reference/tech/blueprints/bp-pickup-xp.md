---
id: bp-pickup-xp
description: "BP_PickupXP, Blueprint of ARRPickup for the XP pickup: 30 uu cyan cube; open for its mesh, material and type."
parent: blueprints-index
kind: doc
---
# BP_PickupXP

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupXP
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the XP pickup dropped by dead robots; visuals only ([[sys-pickups]]).
- components: inherited root PickupMesh only.
- exposed_defaults: PickupType = XP; PickupMesh = /Engine/BasicShapes/Cube, scale (0.3, 0.3, 0.3), OverrideMaterials[0] = MI_GB_PickupXP.
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupXP (Color 0, 0.9, 1, 1, glowing cyan, [[color-code]]).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class.
- [[bp-chaser]] — drops it.
