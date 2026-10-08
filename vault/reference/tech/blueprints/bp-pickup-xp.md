---
id: bp-pickup-xp
description: "BP_PickupXP, Blueprint of ARRPickup for the XP pickup: 30 uu cyan cube with a Niagara ribbon trail and Niagara collect sparkle."
parent: blueprints-index
kind: doc
---
# BP_PickupXP

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupXP
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the XP pickup dropped by dead robots; visuals only ([[sys-pickups]]).
- components: inherited root PickupMesh plus TrailFX (NiagaraComponent, Asset NS_XPTrail, bAutoActivate off, attached to the root; replaces the former stretched-cube Trail StaticMeshComponent, removed in task-054-01).
- exposed_defaults: PickupType = XP; PickupMesh = /Engine/BasicShapes/Cube, scale (0.3, 0.3, 0.3), OverrideMaterials[0] = MI_GB_PickupXP.
- events: OnMagnetStart activates TrailFX and rotates the actor to face the miner (FindLookAtRotation, set once); OnCollected(Location) calls Niagara SpawnSystemAtLocation with NS_PickupSparkle at Location, auto-destroy ([[bp-pickup-sparkle]]).
- logic_in_bp: display only (two events).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupXP (Color 0, 0.9, 1, 1, glowing cyan, [[color-code]]). NS_XPTrail (/Game/RogueRigs/FX/NS_XPTrail, ribbon, cyan 0/0.9/1, width 10, lifetime 0.35 s) and NS_PickupSparkle (/Game/RogueRigs/FX/NS_PickupSparkle, 18-sprite cyan burst, lifetime 0.18-0.28 s, no gravity). MI_XPTrail is no longer used.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class.
- [[bp-chaser]] — drops it.
- [[bp-pickup-sparkle]] — the sparkle it spawns on collection.
