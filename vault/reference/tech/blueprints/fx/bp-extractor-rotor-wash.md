---
id: bp-extractor-rotor-wash
description: "NS_ExtractorRotorWash, M_ExtractorRotorSwirl and M_ExtractorRotorWash, the soft rotor-wash swirls and floor dust ring under the extractor rotors; open for parameters, emitter values and wiring."
parent: blueprints-fx-index
kind: doc
---
# NS_ExtractorRotorWash

- asset_path: /Game/RogueRigs/FX/NS_ExtractorRotorWash (Niagara system, no Blueprint); materials /Game/RogueRigs/FX/M_ExtractorRotorSwirl (Swirls), /Game/RogueRigs/FX/M_ExtractorRotorWash (DustRing)
- parent_class: UNiagaraSystem
- purpose: rotor wash under each of the four rotors of the extractor ([[bp-extraction-shuttle]], [[task-374-extractor-rotor-wash]]). Made in aeb5cb49 (task-374-01) by duplicating [[bp-drone-thruster]] (Glow and Trail removed); wired in 178508af (task-374-02).
- user_params (floats):
  - `User.Intensity`: default 0, range 0-1, the rotor speed alpha.
  - `User.FloorHeight`: default 150 uu, distance from the system down to the floor.
- emitters (both CPU, loop Infinite, local space):
  - `Swirls` (was `Streaks`, the thin lines the owner read as rain, replaced in 50377ce0, [[task-442-extractor-wash-no-rain]]): spawn rate Intensity x 8/s, lifetime 1.2-1.8 s, so at most 14.4 live at Intensity 1 and about 7 at 0.6 on the ground, computed from the values read back. Sprite Unaligned, facing Automatic (camera), size (220 + rand 100) x (0.7 + 0.6 x age) uu, SpriteRotation rand 360 at spawn then + 90 deg/s x Intensity. Ring/Disc radius 30, cone axis (0,0,-1) 50 deg, speed 90, drag 1.2, gravity 0. Colour (0.80, 0.76, 0.68) alpha 0.14 x saturate(Intensity) x sin(pi x age), written each update. Bounds (-130,-130,-130) to (130,130,60).
  - `DustRing`: spawn rate Intensity x step(FloorHeight, 300) x 40/s, lifetime 0.6-0.9 s, sprite 34 x 34 cm camera-facing, Ring/Disc radius 40, position offset (0,0,-FloorHeight), velocity From Point 70 uu/s with origin offset (0,0,-FloorHeight), drag 1.2, colour (0.78, 0.73, 0.64) alpha 0.22 x saturate(Intensity) x step(FloorHeight, 300), bounds (-130,-130,-330) to (130,130,30). The ring only exists within 300 uu of the floor.
- materials: both translucent, unlit, two-sided, Emissive = ParticleColor.RGB. M_ExtractorRotorWash (DustRing): Opacity = ParticleColor.A x radial falloff. M_ExtractorRotorSwirl (Swirls, no texture): Custom node, Opacity = ParticleColor.A x radial falloff x centre hole x two-arm spiral (cos(2a - 6r)), then DepthFade 120 uu so a sprite crossing the floor has no hard line.
- scale: the components are attached to the Mesh scaled x4 ([[bp-extraction-shuttle]]) and the emitters are local space, so positions, speeds and bounds are x4 in the world (Swirls travel about 60 uu locally, about 240 in the world, near the floor at 247-298). Sprite sizes stay world uu: the streaks measured about 26 uu on the owner's image at the x4 scale (estimate, not read from the engine).
- colour: pale grey-beige, no code colour, no colour set from code ([[color-code]], [[readability-rules]]).
- wiring (BP_ExtractionShuttle, 178508af): four NiagaraComponents WashFL, WashFR, WashRL, WashRR (auto-activate) attached to the inherited Mesh, not to the spinning rotors, so the wash does not turn; positions in [[bp-extraction-shuttle]]. Event `OnRotorSpeedChanged(Alpha)` sets `User.Intensity` = Alpha on the four (SetNiagaraVariableByString Float); BeginPlay sets `User.FloorHeight` = component world Z minus actor Z once per component, then reads GetRotorSpeedAlpha once and sets Intensity. No Event Tick.
- tools_that_worked: Niagara SetNiagaraVariableByString(Float) with "User.X"; not the parameter-collection node. `SetEmitterData` `{"Name":"Swirls"}` renames an emitter; its first compiles logged a harmless `Emitter.Scale Alpha.FloatCurve` data interface message, gone once ScaleColor was removed.
- verification: PIE on a landed extractor: 4 components, Intensity 0.6 on the ground, FloorHeight 61.866 and 74.387 read, no LogNiagara error. task-442: compile UpToDate with no error, values read back, DustRing values unchanged, a short PIE with the extractor present and no LogNiagara error.
- unverified: look of the Swirls on screen (no capture by owner decision, he checks it in game); Swirls live count, computed not measured; compiled Swirls values in the running simulation; size of the Swirls sprites at the x4 scale; look of the DustRing on screen; runtime Intensity 0 and 1 and the 300 uu cutoff only checked by HLSL and compile; Ring/Disc coverage meaning and the local-space offset are assumed; arrival extractor; Intensity 1 during takeoff; FloorHeight FR and RL values.
- components: none (Niagara asset). logic_in_bp: none.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-extraction-shuttle]] — the Blueprint carrying the four instances.
- [[bp-drone-thruster]] — the system it was duplicated from.
- [[task-374-extractor-rotor-wash]] — the task that made it.
- [[task-442-extractor-wash-no-rain]] — the task that replaced the streaks by swirls.
- [[color-code]] — colour rules.
- [[readability-rules]] — readability constraint.
