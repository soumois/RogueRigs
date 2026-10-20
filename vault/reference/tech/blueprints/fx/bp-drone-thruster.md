---
id: bp-drone-thruster
description: "NS_DroneThruster, the Niagara engine exhaust of one drone side pod driven by User.Speed (and M_DroneThruster); open for its exact parameter, emitters and values."
parent: blueprints-fx-index
kind: doc
---
# NS_DroneThruster

- asset_path: /Game/RogueRigs/FX/Drone/NS_DroneThruster (Niagara system, no Blueprint); material /Game/RogueRigs/FX/Drone/M_DroneThruster
- parent_class: UNiagaraSystem
- purpose: soft engine exhaust of ONE drone side pod ([[drone]], [[sys-drone]]). The drone gets two instances, one per pod, placed by later tasks (nozzle about 10 cm wide at the pod bottom, exhaust toward -Z of the drone local space). The C++ writes `User.Speed` every tick.
- user_params:
  - `User.Speed`: float (`SetVariableFloat`), default 0, drone speed in uu/s. Reference 0 = hover, 700 = top speed. The system computes `t = saturate(User.Speed / 700)` itself, so any value above 700 is clamped. No `User.Active`; auto-activate is on.
- emitters (both CPU sim, loop Infinite, from the engine Fountain template, LOCAL space so they follow the drone tilt, bounds Fixed (-40,-40,-80) to (40,40,20), gravity 0, drag 0, renderer shadows off, material M_DroneThruster):
  - `Glow`: soft sprite at the nozzle. Spawn 18/s, lifetime 0.22 to 0.30 s, no velocity, size lerp(7, 15, t) cm round, colour (1.0, 1.2, 1.2) with alpha lerp(0.2, 0.8, t) (idle about 25 percent), ScaleSpriteSize curve 1 to 0.4 over life.
  - `Trail`: trailing streaks. Spawn lerp(3, 40, t)/s, lifetime 0.20 to 0.35 s, spawn sphere radius 4 around the nozzle, velocity in a 6 deg cone around (0,0,-1) with speed min lerp(20, 100, t), max lerp(30, 125, t) (so a streak travels 4 to 44 cm), sprite size (width lerp(2, 4.5, t), length lerp(3, 20, t)) velocity aligned, colour (0.85, 1.0, 1.0) alpha lerp(0.25, 0.7, t), ScaleSpriteSize curve 1 to 0.1 over life (stretches then shrinks away).
- material: M_DroneThruster, Additive, Unlit, two-sided, flag bUsedWithNiagaraSprites. Emissive = ParticleColor.RGB x ParticleColor.A x radial falloff `saturate(1 - 4 * dot(uv-0.5, uv-0.5))^2` (soft round glow, no texture).
- colour: soft white with a faint teal tint, no red, yellow, violet or amber; not cyan-saturated ([[color-code]], [[readability-rules]]).
- particle_cost: at most about 20 live particles at t = 1 (18 x 0.30 + 40 x 0.35), about 2 at idle. No lights, no collision, no ribbons.
- components: none (Niagara asset). logic_in_bp: none.
- compile: system UpToDate, 0 errors, 0 warnings, 6 info notes (module version upgrade notes); material recompiled without error; both saved.
- verification (MCP read-back): user variable list, emitter list, spawn rate, sprite size, colour, velocity min and max, lifetime, local space, bounds and renderer settings read back as above.
- unverified: look (nobody saw the viewport: softness, brightness, whether the glow reads at the top-down camera distance); the top-down camera foreshortens the downward streaks; the values are guesses to tune after a look in game; the `User.Speed` binding was not driven from C++ or PIE; the Fountain template ScaleColor alpha fade is kept.
- tools_that_worked: NiagaraToolset_System CreateNiagaraSystem (template NS_PickupSparkle, its emitter then removed), AddEmitter (/Niagara/DefaultAssets/Templates/Emitters/Fountain), RemoveEmitter, AddUserVariables, SetEmitterData, SetStackInputData (Hlsl, Enum, floats, vectors, nested `['Velocity Speed','Minimum']`, DataInterface curve), AddModule (/Niagara/Modules/Update/Size/ScaleSpriteSize), SetRendererData, GetSystemCompileState, GetStackIssues; MaterialTools create_material, add_expression, connect_expressions (ParticleColor outputs RGB and A by name), connect_to_output, recompile; ObjectTools set_properties (`instance` argument); AssetTools save_assets (`asset_paths`).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[drone]] — the drone design.
- [[sys-drone]] — the C++ that writes User.Speed.
- [[bp-drone]] — the drone that carries two instances.
- [[bp-dig-impact]] — the pattern this system follows.
- [[color-code]] — colour rules.
- [[readability-rules]] — no red or amber in the exhaust.
