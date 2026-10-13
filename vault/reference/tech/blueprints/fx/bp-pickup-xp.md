---
id: bp-pickup-xp
description: "BP_PickupXP (glowing XP hex nut, Niagara trail and sparkle) and its sibling BP_PickupXPBig, the big XP block set in XPBigPickupClass."
parent: blueprints-fx-index
kind: doc
---
# BP_PickupXP

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupXP
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the XP pickup dropped by dead robots; visuals only ([[sys-pickups]]).
- components: inherited root PickupMesh plus TrailFX (NiagaraComponent, Asset NS_XPTrail, bAutoActivate off, attached to the root; replaces the former stretched-cube Trail StaticMeshComponent, removed in task-054-01).
- exposed_defaults: PickupType = XP; PickupMesh = /Game/RogueRigs/Art/XPNut/SM_XPNut (hex nut, 30 cm across corners, bottom-centre pivot), scale (1, 1, 1) (the old cube at 0.3 had the same 30 cm footprint); OverrideMaterials[0] = M_XPNut_body, [1] = M_XPNut_hole (task-132-01). CastShadow and collision unchanged. TrailFX relative scale (0.3, 0.3, 0.3) so its effective scale stays that of the old cube-scale root (it inherits the root scale, now 1). Vertical offset not applied: PickupMesh is the root and the C++ sets the actor location directly, so a root offset cannot persist; the bottom-pivot nut sits 5.85 cm lower than a centred one would (its centre is 5.85 cm above the actor origin). The sparkle uses SpawnSystemAtLocation in world space and needs no change.
- events: OnMagnetStart activates TrailFX and rotates the actor to face the miner (FindLookAtRotation, set once); OnCollected(Location) calls Niagara SpawnSystemAtLocation with NS_PickupSparkle at Location, auto-destroy ([[bp-pickup-sparkle]]); on BP_PickupXPBig the system is NS_PickupSparkleBig (magenta copy, particle Color (1, 0.15, 0.75), task-122-01).
- logic_in_bp: display only (two events).
- assets: /Game/RogueRigs/Art/XPNut/: SM_XPNut; M_XPNutGlow (copy of M_GreyBoxGlow, emissive = GlowColor x GlowAmount x (1 + PulseAmount x sin(2 pi (Time x PulseFrequency + phase))), phase = dot(ObjectPositionWS, (0.0137, 0.0211, 0)) in cycles so pickups do not blink together; PulseAmount 0.15, PulseFrequency 0.8333 = period 1.2 s; no tick); M_XPNut_body (parent M_GreyBox, Color linear (0.162, 0.178, 0.198) = #70757B); M_XPNut_hole (parent M_XPNutGlow, Color and GlowColor (0, 0.9, 1), GlowAmount 4, [[color-code]]). MI_GB_PickupXP (Color 0, 0.9, 1, 1) is no longer used by this Blueprint (the cube pickup is gone). /Game/RogueRigs/Materials/GreyBox/MI_GB_PickupXP (Color 0, 0.9, 1, 1, glowing cyan, [[color-code]]). NS_XPTrail (/Game/RogueRigs/FX/NS_XPTrail, ribbon in world space, cyan 0/0.9/1, width 10, lifetime 0.35 s, SpawnRate 60 per second in the emitter update: without a spawn module the emitter produced no particle and the trail was invisible, task-058-01) and NS_PickupSparkle (/Game/RogueRigs/FX/NS_PickupSparkle, 18-sprite cyan burst, lifetime 0.18-0.28 s, no gravity). MI_XPTrail is no longer used.
- big_block (task-110-02): the cube class default `XPBigPickupClass` = BP_PickupXPBig (/Game/RogueRigs/Blueprints/Pickups/BP_PickupXPBig), which turns the big XP block feature on ([[sys-pickups]]). To turn it off, clear `XPBigPickupClass` in the class defaults of BP_PickupXP (set it to None), compile and save.
- sibling BP_PickupXPBig: a duplicate of BP_PickupXP (same parent ARRPickup, NOT a child of the cube, so its own `XPBigPickupClass` stays empty); PickupType XP, same components and events (TrailFX, magnet look-at, sparkle); root PickupMesh template scale 0.54 (1.8 x the cube 0.3, restored by the pool); OverrideMaterials[0] = /Game/RogueRigs/Materials/GreyBox/MI_PickupXPBig. TrailFX (Asset NS_XPTrailBig, magenta copy of NS_XPTrail, task-122-01) keeps relative scale 1 and is attached to the root, so it inherits the 1.8 x scale (no separate parameter). MI_PickupXPBig: parent M_GreyBoxGlow (the cube material M_GreyBox has no emissive input), Color (1, 0.15, 0.75) bright magenta-pink, GlowColor (1, 0.2, 0.8) same hue, GlowAmount 4 (unchanged; was white (1, 1, 1) and (0.75, 1, 1) until task-122-01), Roughness 0.6 (parent default), [[color-code]].
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class.
- [[bp-chaser]] — drops it.
- [[bp-pickup-sparkle]] — the sparkle it spawns on collection.
