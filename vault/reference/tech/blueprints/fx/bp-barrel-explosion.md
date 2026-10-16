---
id: bp-barrel-explosion
description: "NS_BarrelExplosion, the one-shot Niagara explosion burst sized by User.Radius, and its soft sprite material; open for emitters, counts and the ARRExplosionFX contract."
parent: blueprints-fx-index
kind: doc
---
# NS_BarrelExplosion

- asset_path: /Game/RogueRigs/FX/NS_BarrelExplosion (Niagara system, no Blueprint; built from NS_RobotDeathPuff as system template and the engine OmnidirectionalBurst emitter template, same folder as the other FX); material /Game/RogueRigs/FX/M_BarrelExplosionSprite.
- parent_class: UNiagaraSystem
- purpose: the burst played on top of the orange sphere by `ARRExplosionFX::BurstSystem` ([[sys-explosions]], [[bp-explosion-fx]]); reusable for the kamikaze (visual radius 327.6 uu) because every size comes from the radius. Owner request 2026-10-16: "an explosion part (we can reuse it for the kamikaze)".
- user_parameter: `User.Radius` (float user variable named `User.Radius`, the `Radius` of the User namespace), default 735 (barrel visual radius); the C++ sets it before activation. Read back: type NiagaraFloat, default 735. Particle sizes, spawn sphere radii and speeds are HLSL expressions `User.Radius * k`.
- one_shot: system loop Once, loop duration 1.0 s (was 0.1 s from the death puff template, raised so the smoke burst at 0.1 s is not cut), inactive response Complete; each emitter loops Once (Life Cycle Mode Self), so the longest particle (smoke, 1.3 s + 0.1 s delay) ends the system at about 1.4 s, inside the 1.5 s `BurstSeconds`. No collision module, no events, no pooling (ARRExplosionFX owns the component).
- emitters (all CPU sprites, shadows off, material M_BarrelExplosionSprite; 45 particles per explosion; provisional, owner 2026-10-16; k = factor of `User.Radius`, R = 735 gives the barrel value in uu):
  - Flash: 1 particle at t = 0, lifetime 0.12 to 0.18 s, sprite size 0.9 to 1.2 R (about 660 to 880 uu), colour HDR (8, 5, 1.8), speed 0, gravity module off.
  - Fireball: 10 particles at t = 0, lifetime 0.45 to 0.8 s, spawn sphere 0.25 R, size 0.35 to 0.6 R, speed 0.2 to 0.7 R per second, drag 2.5, colour HDR orange (2.6, 0.9, 0.1), gravity module off.
  - Sparks: 28 particles at t = 0, lifetime 0.5 to 0.9 s, spawn sphere 0.05 R, size 0.02 to 0.04 R (15 to 29 uu), speed 1.0 to 2.2 R per second, drag 1.5, colour HDR yellow-orange (4, 2, 0.4), engine gravity (-980) kept.
  - Smoke: 6 particles at t = 0.1 s, lifetime 1.0 to 1.3 s, spawn sphere 0.25 R, size 0.35 to 0.55 R, speed 0.1 to 0.3 R per second, drag 1.5, colour dark grey (0.08, 0.07, 0.06) alpha 0.6, gravity module off.
  - all: alpha scaled by the template curve (1 to 0 over the life), sprite size scale from the template curve (about constant), random sprite rotation, no sprite alignment.
- material: M_BarrelExplosionSprite, Translucent, Unlit, two sided, used with Niagara sprites. Emissive = particle colour RGB; opacity = particle colour alpha x (saturate(1 - 2 x distance(UV, 0.5))) squared, a soft round blob instead of the engine DefaultSpriteMaterial square. The engine default material is additive and square, so dark smoke would have been invisible.
- colour: orange and yellow-orange mean "explodes", the grey smoke is neutral; no red, red stays reserved for enemy fire ([[color-code]], [[readability-rules]]).
- events: none
- logic_in_bp: none
- assets: not yet referenced: a barrel child of BP_ExplosionFX that sets `BurstSystem` is task-197-06 or later (bp-explosion-fx still leaves it unset).
- verified: system compile state UpToDate, no stack error or warning; material recompiled with no error; every value above read back through MCP; PIE not run. Not verified: the visual (no capture allowed; squares replaced by the soft material but the look, the density at 735 uu and the 45-particle cost are not judged); that the 1.4 s end really releases the component.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-explosions]] — the `BurstSystem` and `User.Radius` contract.
- [[bp-explosion-fx]] — the explosion Blueprint that will play it.
- [[bp-robot-death-puff]] — the burst it was derived from.
- [[color-code]] — colours.
- [[readability-rules]] — no red, readable burst.
