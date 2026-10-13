---
id: bp-pickup-sparkle
description: "Legacy BP_PickupSparkle, unused since task-054-01 (Niagara NS_PickupSparkle replaced it); kept for the Juice automation test."
parent: blueprints-fx-index
kind: doc
---
# BP_PickupSparkle

- asset_path: /Game/RogueRigs/Blueprints/FX/BP_PickupSparkle
- parent_class: ARRExplosionFX (/Script/RogueRigs.RRExplosionFX)
- purpose: former collect sparkle of an XP cube (task-027-02). Since task-054-01 BP_PickupXP spawns the Niagara system NS_PickupSparkle (/Game/RogueRigs/FX/NS_PickupSparkle) instead; this Blueprint is kept only because the Juice automation test (RRJuiceIntegrationTests) still loads its class.
- components: inherited root Mesh only: engine Sphere, material slot 0 = MI_PickupSparkle, no collision.
- exposed_defaults: Duration = 0.25 s; MeshRadius 50 kept.
- events: EventBeginPlay calls SetRadius(45) (the C++ class grows the mesh to it, fades Opacity 1 to 0, destroys itself).
- logic_in_bp: one SetRadius call (display setup).
- assets: /Game/RogueRigs/FX/MI_PickupSparkle (M_GreyBoxTranslucent, Color 0.3/1/1).
- owner_agent: agent-bp
- verified: compiled with warnings as errors (task-027-02); PIE result in the task result.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-explosions]] — the C++ base class ARRExplosionFX.
- [[sys-pickups]] — the pickup events that spawn it.
- [[bp-pickup-xp]] — spawner of the sparkle.
- [[bp-pickup-heal]] — spawner of the heal ring.
