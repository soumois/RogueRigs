---
id: bp-explosion-shockwave
description: "M_ExplosionShockwave and MI_ExplosionShockwave, the orange ground ring decal of the explosion shockwave (parameters, ring width, projection); open to wire ShockwaveMaterial or tune the ring."
parent: blueprints-fx-index
kind: doc
---
# M_ExplosionShockwave / MI_ExplosionShockwave

Values below are provisional, owner 2026-10-16 (request: the explosion circle, reusable for the kamikaze). Built by task-197-05; ring width parameter added by task-198-02.

- asset_path: /Game/RogueRigs/FX/M_ExplosionShockwave (material), /Game/RogueRigs/FX/MI_ExplosionShockwave (orange instance, the one to assign to `ARRExplosionFX::ShockwaveMaterial`)
- parent_class: none (material; no Blueprint graph)
- purpose: a sharp orange ring on the ground, growing from the centre to the explosion radius, driven by C++ ([[sys-explosions]], burst and shockwave contract).
- timing (C++, task-198-01, provisional, owner 2026-10-16): the ring has its own timeline, independent of the sphere: `Progress` 0 to 1 over `ARRExplosionFX::ShockwaveGrowSeconds` 0.6 s at `Opacity` 1, then `Opacity` 1 to 0 over `ShockwaveFadeSeconds` 0.2 s; the decal stays drawn after the sphere (hidden at 0.6 s) and is hidden at 0.8 s.
- settings: domain Deferred Decal, blend Translucent, shading Unlit (emissive), same projection rules as [[sys-ground-telegraph]] (decal pitch -90, box half size (256, Radius, Radius), sort order 1; set by the C++ component).
- parameters (names exact, as `M_GroundTelegraph`): `Radius` (scalar, uu, default 700; C++ sets it once to the visual radius); `Progress` (scalar 0..1, default 0: ring radius = Radius x Progress; fully transparent at Progress <= 0.001); `Opacity` (scalar 0..1, default 1); `ProgressColor` (vector, ring colour; default and instance orange 1.0, 0.45, 0.05, the danger orange of [[color-code]]); `ZoneColor` (vector, same orange, unused by the ring); `RingWidth` (scalar, uu, default 60, provisional owner 2026-10-16; the instance keeps the default; task-198-02); `FillOpacity` (scalar, 0.25, kept from the copied graph but unused: no disc fill).
- look: one solid ring, `RingWidth` wide (default 60 uu, provisional; was 24, hardly visible in -game frames), constant world width, inner-aligned (covers Radius x Progress - RingWidth to Radius x Progress, so at Progress 1 it sits inside the box edge and is not cut). One-pixel anti-aliasing via fwidth, no glow, no fill, no dashes. Orange = explodes; red stays reserved for enemy fire ([[readability-rules]]).
- derivation: duplicated from `M_GroundTelegraph` (same graph); the progress-ring Custom node takes the width from the `RingWidth` parameter (input W) instead of the 12 hard-coded; the dashed zone-ring node and the fill node were neutralised (return 0, kept wired so the graph keeps its shape); opacity = max of the three masks x `Opacity`.
- assets: none besides the two materials.
- owner_agent: agent-bp
- verified: material recompiled without error, settings and parameters read back, instance parameters read back, saved. Not checked on screen (no capture allowed; owner judges). Assigned: [[bp-explosion-fx-barrel]] sets `ShockwaveMaterial` to MI_ExplosionShockwave.

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-explosions]] — the C++ class that drives the parameters.
- [[sys-ground-telegraph]] — the material it was derived from.
- [[bp-explosion-fx]] — the explosion Blueprint family that uses it.
- [[bp-explosion-fx-barrel]] — the child that assigns it, played by the barrel and the kamikaze.
- [[color-code]] — orange colour meaning.
- [[readability-rules]] — colour reservation.
