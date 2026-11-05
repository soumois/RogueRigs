---
id: bp-explosion-shockwave
description: "M_ExplosionShockwave and MI_ExplosionShockwave, the cartoon ground ring decal of the explosion shockwave (parameters, bands, width, projection); open to wire ShockwaveMaterial or tune the ring."
parent: blueprints-fx-index
kind: doc
---
# M_ExplosionShockwave / MI_ExplosionShockwave

Built by task-197-05 (owner 2026-10-16: the explosion circle, reusable for the kamikaze); ring width parameter by task-198-02; cartoon look by task-446-03 (provisional, owner 2026-11-05, "refaire tout l'explosion en style cartoon", reference `Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png`, same family as [[bp-robot-death-puff]]).

- asset_path: /Game/RogueRigs/FX/M_ExplosionShockwave (material), /Game/RogueRigs/FX/MI_ExplosionShockwave (instance assigned to `ARRExplosionFX::ShockwaveMaterial`; no override left, the stale colour overrides were cleared in task-446-03)
- parent_class: none (material; no Blueprint graph)
- purpose: a ring on the ground growing from the centre to the explosion radius, driven by C++ ([[sys-explosions]], burst and shockwave contract).
- timing (C++, task-198-01, provisional, owner 2026-10-16): `Progress` 0 to 1 over `ARRExplosionFX::ShockwaveGrowSeconds` 0.6 s at `Opacity` 1, then `Opacity` 1 to 0 over `ShockwaveFadeSeconds` 0.2 s; the decal stays drawn after the sphere (hidden at 0.6 s) and is hidden at 0.8 s.
- settings: domain Deferred Decal, blend Translucent, shading Unlit (emissive), same projection rules as [[sys-ground-telegraph]] (decal pitch -90, box half size (256, Radius, Radius), sort order 1; set by the C++ component).
- parameters (names exact): `Radius` (scalar, uu, default 700; C++ sets it once to the visual radius); `Progress` (scalar 0..1, default 0; nothing drawn at Progress <= 0.001); `Opacity` (scalar 0..1, default 1); `RingWidth` (scalar, uu, default 60, provisional owner 2026-10-16, base of the width). `ZoneColor`, `ProgressColor` and `FillOpacity` were removed in task-446-03: the colours are now in the shader.
- look (task-446-03): one Custom node (float4: RGB to emissive, alpha to opacity), no texture. Outer edge at Radius x Progress, jagged inward by at most 3 % (triangle waves of the angle, the waves of M_CartoonBlastCore), so at Progress 1 it reaches the radius and sits inside the box edge. Width `RingWidth` x 1.35 to 2.25, jagged, at most 0.6 of the outer radius (a small disc at the very start). Three flat bands with jagged borders, from outside in: red-orange (0.7, 0.09, 0.013), orange (1.1, 0.33, 0.02), yellow (1.5, 1.0, 0.08), the colours of the sphere ([[bp-explosion-fx-barrel]]). Cooling = 0.25 x Progress + (1 - Opacity): each band moves one step towards dark red (0.17, 0.016, 0.012); during the fade the width thins to 40 % ("burning out"). Alpha = ring x step(Progress) x Opacity; edges anti-aliased with fwidth.
- colour: red and dark red allowed for explosions ([[readability-rules]]); the danger circles before an explosion keep their own material, unchanged ([[sys-ground-telegraph]]).
- derivation: duplicated from `M_GroundTelegraph` (task-197-05); since task-446-03 the copied graph is replaced, `M_GroundTelegraph` and its instances untouched.
- assets: none besides the two materials.
- owner_agent: agent-bp
- verified: material recompiled without error, the five inputs and both outputs read back, instance saved; `RogueRigs.Explosions` 10 of 10 passed (-nullrhi). Not checked on screen (no capture, the owner judges in game); a formula preview only.

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-explosions]] — the C++ class that drives the parameters.
- [[sys-ground-telegraph]] — the material it was derived from, unchanged.
- [[bp-explosion-fx]] — the explosion Blueprint family that uses it.
- [[bp-explosion-fx-barrel]] — the child that assigns it, played by the barrel and the kamikaze.
- [[bp-robot-death-puff]] — the cartoon flash it follows.
- [[color-code]] — colour meaning.
- [[readability-rules]] — the owner line allowing red in explosions.
