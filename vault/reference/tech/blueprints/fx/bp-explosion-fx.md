---
id: bp-explosion-fx
description: "BP_ExplosionFX, Blueprint of ARRExplosionFX with an engine sphere and the translucent orange MI_GB_Explosion; open for its mesh and material."
parent: blueprints-fx-index
kind: doc
---
# BP_ExplosionFX

- asset_path: /Game/RogueRigs/Blueprints/FX/BP_ExplosionFX (exact path, referenced by ExplosionStats.csv)
- parent_class: ARRExplosionFX (/Script/RogueRigs.RRExplosionFX)
- purpose: the visual of every explosion: an expanding, fading sphere ([[sys-explosions]]).
- components: inherited root Mesh only: StaticMesh = engine Sphere, material slot 0 = MI_GB_Explosion, no collision, no shadow.
- exposed_defaults: C++ defaults kept (Duration 0.25 s, FadeParameterName Opacity; since task-097-01 also FadeSeconds 0.25 s and HoldSeconds 0, the hold being set at spawn by the subsystem). Radius (row radius × 1.05) and hold (0.1 s) are passed from C++ at spawn: no Blueprint change was needed (task-097-01).
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/M_GreyBoxTranslucent (blend mode Translucent, unlit; vector Color to emissive, scalar Opacity to opacity); MI_GB_Explosion (Color 1, 0.45, 0.05, Opacity 1).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-explosions]] — the C++ class and queue that spawn it.
