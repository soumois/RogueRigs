---
id: bp-heal-ring
description: "BP_HealRing, expanding fading green ground ring spawned at the miner when a heal pickup is collected; open for its mesh, material and radius."
parent: blueprints-fx-index
kind: doc
---
# BP_HealRing

- asset_path: /Game/RogueRigs/Blueprints/FX/BP_HealRing
- parent_class: ARRExplosionFX (/Script/RogueRigs.RRExplosionFX)
- purpose: green ring around the miner on heal collection (task-027-02).
- components: inherited root Mesh only: engine Plane (100 x 100), material slot 0 = MI_GroundRingGreen (ring mask at 0.88 of the half width).
- exposed_defaults: Duration = 0.6 s; MeshRadius 50 kept.
- events: EventBeginPlay calls SetRadius(140) (the C++ class grows the mesh to it, fades Opacity 1 to 0, destroys itself).
- logic_in_bp: one SetRadius call (display setup).
- assets: /Game/RogueRigs/FX/M_GroundRing (translucent unlit, two-sided, params Color and Opacity, ring = saturate(1 - abs(2 x distance(UV, 0.5) - 0.88) x 10) x Opacity); MI_GroundRingGreen (Color 0.1/1/0.2), MI_GroundRingTeal (Color 0/0.85/1, for the console).
- owner_agent: agent-bp
- verified: compiled with warnings as errors (task-027-02); PIE result in the task result.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-explosions]] — the C++ base class ARRExplosionFX.
- [[sys-pickups]] — the pickup events that spawn it.
- [[bp-pickup-xp]] — spawner of the sparkle.
- [[bp-pickup-heal]] — spawner of the heal ring.
