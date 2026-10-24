---
id: bp-console-hack-complete
description: "NS_ConsoleHackComplete, the one-shot green ring flash and white-gold sparks spawned by BP_ObjectiveConsole when the hack completes; open for sizes, values and wiring."
parent: blueprints-fx-index
kind: doc
---
# NS_ConsoleHackComplete

- asset_path: /Game/RogueRigs/FX/NS_ConsoleHackComplete (Niagara system, no Blueprint)
- parent_class: UNiagaraSystem
- purpose: owner-confirmed big particle at the end of a console hack ([[objective-console]]): a green ring flash expanding past the console, with white-gold sparks rising above it. Sized for the task-311 console (3 x 3 cells of 67 uu, about 201 x 172 x 147 uu) inside the 7-cell hack zone (469 uu). Green is the console state colour exception, sparks are the white-gold of the reward ([[color-code]]). Duplicated from NS_HeavySlamImpact ([[bp-heavy-slam-impact]]), same method.
- user_params: none (the copied User.Radius and User.WallNormal were removed, nothing is linked to them).
- emitters (both CPU sim, Once, one SpawnBurst_Instantaneous at t=0; the emitter names Dust and Chips are kept from the source):
  - `Dust` (the ring): burst 64, lifetime 0.55 to 0.6 s, ShapeLocation Sphere radius 0 (origin, floor), AddVelocity Linear HLSL `float3(cos(Particles.UniqueID * 0.09817), sin(Particles.UniqueID * 0.09817), 0.0) * 600.0` (0.09817 = 2 pi / 64, 64 particles evenly in the ground plane, Z 0), drag 0, gravity 0: the ring crosses the footprint edge (radius ~110) at about 0.18 s and reaches about 330 (3 x) at 0.55 s. Colour (0.2, 1.6, 0.45), alpha 0.9, template ScaleColor fades it; uniform sprite size 70 to 90; default sprite material.
  - `Chips` (the sparks): burst 40, lifetime 0.8 to 1.3 s, Sphere radius 60, AddVelocity In Cone axis (0, 0, 1) angle 25 deg, speed 250 to 450, gravity Z -120, drag 0.5, colour (2.0, 1.6, 0.6) alpha 1 (white-gold, above 1 to glow), uniform sprite size 6 to 10, renderer material set to DefaultSpriteMaterial (was M_DigChip). Rise computed about 150 to 350 uu, above the 147 uu console top.
- wiring: BP_ObjectiveConsole EventGraph, event OnHackCompleteBurst(Location, ChipCount) -> `Niagara|SpawnSystemAtLocation` (system NS_ConsoleHackComplete, Location from the event, other pins default so auto destroy is the engine default true). ChipCount unused. See [[bp-objective-console]]. BP_BonusChest does not implement the event: no effect on the chest.
- colour: green ring, white-gold sparks.
- particle_cost: 104 particles at the burst, gone after about 1.3 s, shadows off.
- logic_in_bp: none (one event, one spawn call).
- compile: system UpToDate, 0 errors, 0 warnings, 6 info notes (module version upgrade), saved.
- unverified: look and feel (no capture allowed); ring radii and timing are computed (no drag), not measured; UniqueID angles assumed to start at 0 and step by 1; the auto destroy pin default was not read back; not tested in PIE (the event fires at CompleteHack, read back as wired only).
- tools_that_worked: AssetTools duplicate, save_assets; NiagaraToolset_System SetStackInputData (nested ["Velocity Speed","Minimum"]), RemoveUserVariables, SetRendererData, GetModuleInputValues, GetSystemCompileState, GetStackIssues; BlueprintTools read_graph_dsl, write_graph_dsl, compile_blueprint. The MCP tools were reached by JSON-RPC over HTTP (call_tool).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-objective-console]] — the Blueprint that spawns it.
- [[bp-heavy-slam-impact]] — the system it was duplicated from.
- [[objective-console]] — the hack it celebrates.
- [[color-code]] — console green exception.
