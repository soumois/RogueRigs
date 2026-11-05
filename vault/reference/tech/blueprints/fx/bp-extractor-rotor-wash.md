---
id: bp-extractor-rotor-wash
description: "NS_ExtractorRotorWash, M_ExtractorRotorSwirl and M_ExtractorRotorWash, the rotor-wash swirls and height-fading ground blast under the extractor rotors; open for parameters, emitter values and wiring."
parent: blueprints-fx-index
kind: doc
---
# NS_ExtractorRotorWash

- asset_path: /Game/RogueRigs/FX/NS_ExtractorRotorWash (Niagara system, no Blueprint); materials /Game/RogueRigs/FX/M_ExtractorRotorSwirl (Swirls), /Game/RogueRigs/FX/M_ExtractorRotorWash (GroundBlast)
- parent_class: UNiagaraSystem
- purpose: rotor wash under each of the four rotors of the extractor ([[bp-extraction-shuttle]], [[task-374-extractor-rotor-wash]]). Made in aeb5cb49 (task-374-01) by duplicating [[bp-drone-thruster]] (Glow and Trail removed); wired in 178508af (task-374-02).
- user_params (floats):
  - `User.Intensity`: default 0, range 0-1, the rotor speed alpha.
  - `User.FloorHeight`: default 150 uu, distance from the system down to the floor when landed.
  - `User.HeightAboveFloor`: default 0, height of the extractor base above the floor (uu), task-443.
- emitters (both CPU, loop Infinite; Swirls local space, GroundBlast world space):
  - `Swirls` (was `Streaks`, the thin lines the owner read as rain, replaced in 50377ce0, [[task-442-extractor-wash-no-rain]]): spawn rate Intensity x 8/s, lifetime 1.2-1.8 s, so at most 14.4 live at Intensity 1 and about 7 at 0.6 on the ground, computed from the values read back. Sprite Unaligned, facing Automatic (camera), size (220 + rand 100) x (0.7 + 0.6 x age) uu, SpriteRotation rand 360 at spawn then + 90 deg/s x Intensity. Ring/Disc radius 30, cone axis (0,0,-1) 50 deg, speed 90, drag 1.2, gravity 0. Colour (0.80, 0.76, 0.68) alpha 0.14 x saturate(Intensity) x sin(pi x age), written each update. Bounds (-130,-130,-130) to (130,130,60).
  - `GroundBlast` (was `DustRing`, task-443, c7f6db71, beb4fc12; provisional, owner 2026-11-05, [[task-443-extractor-ground-blast]]): strength S = saturate(1 - HeightAboveFloor / 400), continuous, none at 400 uu. Spawn rate Intensity x S x 50/s, lifetime 0.8-1.2 s. World space, so particles stay on the floor while the extractor rises: position and velocity origin at owner + (0, 0, 8 - FloorHeight - HeightAboveFloor) (coordinate space Simulation), Ring/Disc radius 40 x owner scale 4, velocity From Point 500 + rand 300 uu/s, drag 1.5, gravity 0. Sprite flat on the floor (facing Custom, `Particles.SpriteFacing` (0,0,1)) and aligned on the velocity, `Particles.BlastSize` 110 + rand 60, size BlastSize x (1, 1.8) x (0.7 + 0.9 x age). Colour (0.78, 0.73, 0.64) alpha 0.5 x saturate(Intensity) x S x sin(pi x sqrt(age)), written each update. Fixed bounds (-220,-220,-220) to (220,220,30), relative to the component, x4 in the world. Before task-443 the local-space offset was multiplied by the x4 scale and the ring sat about 700 uu under the floor (computed), so it was likely never seen.
- materials: both translucent, unlit, two-sided, Emissive = ParticleColor.RGB. M_ExtractorRotorWash (DustRing): Opacity = ParticleColor.A x radial falloff. M_ExtractorRotorSwirl (Swirls, no texture): Custom node, Opacity = ParticleColor.A x radial falloff x centre hole x two-arm spiral (cos(2a - 6r)), then DepthFade 120 uu so a sprite crossing the floor has no hard line.
- scale: the components are attached to the Mesh scaled x4 ([[bp-extraction-shuttle]]); Swirls is local space, so its positions, speeds and bounds are x4 in the world (Swirls travel about 60 uu locally, about 240 in the world, near the floor at 247-298). Sprite sizes stay world uu: the streaks measured about 26 uu on the owner's image at the x4 scale (estimate, not read from the engine).
- colour: pale grey-beige, no code colour, no colour set from code ([[color-code]], [[readability-rules]]).
- wiring (BP_ExtractionShuttle, 178508af): four NiagaraComponents WashFL, WashFR, WashRL, WashRR (auto-activate) attached to the inherited Mesh, not to the spinning rotors, so the wash does not turn; positions in [[bp-extraction-shuttle]]. Event `OnRotorSpeedChanged(Alpha)` sets `User.Intensity` = Alpha on the four (SetNiagaraVariableByString Float); BeginPlay sets `User.FloorHeight` = component world Z minus actor Z once per component, then reads GetRotorSpeedAlpha once and sets Intensity. Event `OnHeightAboveFloorChanged(Height)` sets `User.HeightAboveFloor` on the four (beb4fc12). No Event Tick. `ARRShuttle::StartTakeoff` sets the components solo and tickable when paused ([[sys-objectives-shuttle]]).
- tools_that_worked: Niagara SetNiagaraVariableByString(Float) with "User.X"; not the parameter-collection node. `SetEmitterData` `{"Name":"Swirls"}` renames an emitter; its first compiles logged a harmless `Emitter.Scale Alpha.FloatCurve` data interface message, gone once ScaleColor was removed. Last-edit compile trap: [[unreal-mcp-quirks]].
- verification: task-443 PIE, values from `fx.Niagara.DumpComponents full filter=NS_ExtractorRotorWash` typed in the Cmd box, no capture: landed Intensity 0.6, FloorHeight 247.46, 247.62, 298.25, 297.55, Height 0, 32-36 GroundBlast particles per component, alpha up to about 0.29; takeoff behind the stats screen (world paused) Intensity 1, Height 241.98 then 400, particles dropping to 12-14 with alpha falling to 0 at Height 400, every particle at world Z 8 with horizontal outward velocity. No LogNiagara error.
- unverified: look of the blast on screen, whether it reads around the hull, the arrival blast (same event, particles not dumped), performance (no capture and no stress run by owner decision, he checks it in game). Defect left as told by the brief of task-443: Swirls compiled alpha is 0 (60 of 60 particles read A 0 in the same dump) although the read-back shows the expression, so the Swirls of task-442 are likely invisible in game; their size at the x4 scale stays unverified.
- components: none (Niagara asset). logic_in_bp: none.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-extraction-shuttle]] — the Blueprint carrying the four instances.
- [[bp-drone-thruster]] — the system it was duplicated from.
- [[task-374-extractor-rotor-wash]] — the task that made it.
- [[task-442-extractor-wash-no-rain]] — the task that replaced the streaks by swirls.
- [[task-443-extractor-ground-blast]] — the task that made the ground blast.
- [[unreal-mcp-quirks]] — the Niagara compile trap met here.
- [[sys-objectives-shuttle]] — the height signal and the solo tick.
- [[color-code]] — colour rules.
- [[readability-rules]] — readability constraint.
