---
id: bp-extractor-rotor-wash
description: "NS_ExtractorRotorWash and M_ExtractorRotorWash, the rotor-wash streaks and floor dust ring under the extractor rotors; open for parameters, emitter values and wiring."
parent: blueprints-fx-index
kind: doc
---
# NS_ExtractorRotorWash

- asset_path: /Game/RogueRigs/FX/NS_ExtractorRotorWash (Niagara system, no Blueprint); material /Game/RogueRigs/FX/M_ExtractorRotorWash
- parent_class: UNiagaraSystem
- purpose: rotor wash under each of the four rotors of the extractor ([[bp-extraction-shuttle]], [[task-374-extractor-rotor-wash]]). Made in aeb5cb49 (task-374-01) by duplicating [[bp-drone-thruster]] (Glow and Trail removed); wired in 178508af (task-374-02).
- user_params (floats):
  - `User.Intensity`: default 0, range 0-1, the rotor speed alpha.
  - `User.FloorHeight`: default 150 uu, distance from the system down to the floor.
- emitters (both CPU, loop Infinite, local space):
  - `Streaks`: spawn rate Intensity x 45/s, lifetime 0.35-0.6 s, sprite 3 x 26 cm velocity-aligned, Ring/Disc radius 30, cone axis (0,0,-1) 4 deg, speed 260 uu/s, gravity 0, colour (0.80, 0.76, 0.68) alpha 0.30 x saturate(Intensity), bounds (-45,-45,-200) to (45,45,20).
  - `DustRing`: spawn rate Intensity x step(FloorHeight, 300) x 40/s, lifetime 0.6-0.9 s, sprite 34 x 34 cm camera-facing, Ring/Disc radius 40, position offset (0,0,-FloorHeight), velocity From Point 70 uu/s with origin offset (0,0,-FloorHeight), drag 1.2, colour (0.78, 0.73, 0.64) alpha 0.22 x saturate(Intensity) x step(FloorHeight, 300), bounds (-130,-130,-330) to (130,130,30). The ring only exists within 300 uu of the floor.
- material: M_ExtractorRotorWash, translucent, unlit, two-sided; Emissive = ParticleColor.RGB, Opacity = ParticleColor.A x radial falloff.
- colour: pale grey-beige, no code colour, no colour set from code ([[color-code]], [[readability-rules]]).
- wiring (BP_ExtractionShuttle, 178508af): four NiagaraComponents WashFL, WashFR, WashRL, WashRR (auto-activate) attached to the inherited Mesh, not to the spinning rotors, so the wash does not turn; positions in [[bp-extraction-shuttle]]. Event `OnRotorSpeedChanged(Alpha)` sets `User.Intensity` = Alpha on the four (SetNiagaraVariableByString Float); BeginPlay sets `User.FloorHeight` = component world Z minus actor Z once per component, then reads GetRotorSpeedAlpha once and sets Intensity. No Event Tick.
- tools_that_worked: Niagara SetNiagaraVariableByString(Float) with "User.X"; not the parameter-collection node.
- verification: PIE on a landed extractor: 4 components, Intensity 0.6 on the ground, FloorHeight 61.866 and 74.387 read, no LogNiagara error.
- unverified: look on screen; runtime Intensity 0 and 1 and the 300 uu cutoff only checked by HLSL and compile; Ring/Disc coverage meaning and the local-space offset are assumed; arrival extractor; Intensity 1 during takeoff; FloorHeight FR and RL values.
- components: none (Niagara asset). logic_in_bp: none.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-extraction-shuttle]] — the Blueprint carrying the four instances.
- [[bp-drone-thruster]] — the system it was duplicated from.
- [[task-374-extractor-rotor-wash]] — the task that made it.
- [[color-code]] — colour rules.
- [[readability-rules]] — readability constraint.
