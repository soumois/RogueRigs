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
- burst_and_shockwave (C++ side, task-197-02): `BurstSystem` and `ShockwaveMaterial` are left unset here, so BP_ExplosionFX (kamikaze) keeps exactly its look and 0.6 s lifetime. The child [[bp-explosion-fx-barrel]] (task-197-06, used by the Barrel row) sets them. A child that sets them gets a Niagara burst (user parameter `User.Radius`, actor kept 1.5 s) and a ground ring decal (`Radius`, `Progress`, `Opacity`); contract in [[sys-explosions]].
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/M_GreyBoxTranslucent (blend mode Translucent, unlit; vector Color to emissive, scalar Opacity to opacity); MI_GB_Explosion (Color 1, 0.45, 0.05, Opacity 1).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-explosions]] — the C++ class and queue that spawn it.
- [[bp-explosion-fx-barrel]] — its child with burst and shockwave, used by the Barrel row.
