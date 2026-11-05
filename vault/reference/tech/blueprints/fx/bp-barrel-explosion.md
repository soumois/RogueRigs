---
id: bp-barrel-explosion
description: "NS_BarrelExplosion, the one-shot cartoon burst of the barrel and kamikaze explosion sized by User.Radius; open for emitters, counts, timing and the ARRExplosionFX contract."
parent: blueprints-fx-index
kind: doc
---
# NS_BarrelExplosion

- asset_path: /Game/RogueRigs/FX/NS_BarrelExplosion (Niagara system, no Blueprint). Since task-446-01 a copy of [[bp-robot-death-puff]] (old system deleted, same path); materials M_CartoonBlastCore, M_CartoonBlastBlob, M_CartoonBlastLine of that node, no texture. The former soft-sprite M_BarrelExplosionSprite was deleted (no referencer left).
- parent_class: UNiagaraSystem
- purpose: the burst played over the sphere by `ARRExplosionFX::BurstSystem` ([[sys-explosions]]) for the barrel and kamikaze rows through [[bp-explosion-fx-barrel]]. Look after the owner reference `Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png`, same family as the robot death flash; no pixel of it is used. Values provisional, owner 2026-11-05 ([[task-446-explosions-red]]).
- user_parameter: `User.Radius` (float, default 735 = barrel visual radius; the C++ sets the visual radius before activation, kamikaze 327.6). Sizes, ring radii and speeds are HLSL expressions `User.Radius * k`; R below = `User.Radius`.
- emitters (4 CPU sprites emitters, gravity 0, no position offset, AddVelocity From Point flat on the ring plane; ScaleColor `Scale Alpha` = `Particles.NormalizedAge`, read by the materials as progress; 51 sprites per explosion):
  - Lines: 20 at t = 0.04 s, life 0.2 to 0.3 s, M_CartoonBlastLine, velocity aligned, size 0.015 R x 0.12 to 0.2 R, ring 0.8 R, speed 2 to 3 R/s, drag 4.
  - Core: 1 at t = 0, life 0.16 s, M_CartoonBlastCore, size 0.8 R: the small first flash.
  - Blobs: 14 at t = 0.1 s, life 0.3 to 0.4 s, M_CartoonBlastBlob, size 0.1 to 0.16 R, ring 0.85 R, speed 0.9 to 1.4 R/s, drag 4, colour (1.2, 0.3, 0.02).
  - Puffs: 16 at t = 0.36 s (start of the sphere fade), life 0.45 to 0.6 s, M_CartoonBlastBlob, size 0.14 to 0.24 R, ring 0.8 R, speed 0.05 to 0.15 R/s, drag 2, dark red (0.32, 0.045, 0.025).
- one_shot: system loop Once, loop duration 0.5 s, inactive response Complete, each emitter Life Cycle Mode System; the last puff ends by 0.96 s, inside the 1.5 s `BurstSeconds`. No collision, no events, no pooling (ARRExplosionFX owns the component).
- draw_order: system translucency sort priority overridden to 1, so the burst draws above the sphere (priority 0) instead of being hidden inside it.
- colour: white, yellow, orange, red-orange and dark red as in the reference, red allowed for explosions ([[readability-rules]]); [[color-code]].
- events: none
- logic_in_bp: none
- assets: `BurstSystem` of [[bp-explosion-fx-barrel]]; [[bp-explosion-fx]] leaves it unset.
- verified: compile state UpToDate, 0 errors, 0 warnings; every value above read back; `BurstSystem` read back on the Blueprint. Not verified: the look in game (owner validates, task-446 A8, no capture); that the HLSL inputs are in the compiled script (no PIE dump); that the end really releases the component.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-explosions]] — the `BurstSystem` and `User.Radius` contract.
- [[bp-explosion-fx]] — the parent explosion Blueprint, burst unset.
- [[bp-explosion-fx-barrel]] — the Blueprint that plays it, for the barrel and kamikaze rows.
- [[bp-robot-death-puff]] — the system it was copied from and its materials.
- [[color-code]] — colours.
- [[readability-rules]] — the owner line allowing red in explosions.
- [[task-446-explosions-red]] — the task that gave it the cartoon look.
