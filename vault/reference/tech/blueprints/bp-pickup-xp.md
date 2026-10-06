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
- components: inherited root PickupMesh plus Trail (StaticMeshComponent, engine Cube, relative (-170, 0, 0), scale (3, 0.25, 0.25) = 90 x 7.5 uu behind the cube, MI_XPTrail translucent cyan opacity 0.45, no collision, no shadow, hidden by default).
- exposed_defaults: PickupType = XP; PickupMesh = /Engine/BasicShapes/Cube, scale (0.3, 0.3, 0.3), OverrideMaterials[0] = MI_GB_PickupXP.
- events: OnMagnetStart sets Trail visible and rotates the actor to face the miner (FindLookAtRotation, set once: the trail points along the flight line at latch time); OnCollected(Location) spawns BP_PickupSparkle at Location ([[bp-pickup-sparkle]]).
- logic_in_bp: display only (two events).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupXP (Color 0, 0.9, 1, 1, glowing cyan, [[color-code]]). MI_XPTrail (/Game/RogueRigs/FX/MI_XPTrail, translucent cyan).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class.
- [[bp-chaser]] — drops it.
- [[bp-pickup-sparkle]] — the sparkle it spawns on collection.
