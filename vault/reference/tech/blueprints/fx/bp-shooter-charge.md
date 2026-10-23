---
id: bp-shooter-charge
description: "NS_ShooterCharge, the red charge-up glow and inward sparks at the shooter's cannon tip during its 0.6 s laser; open for its emitters and values."
parent: blueprints-fx-index
kind: doc
---
# NS_ShooterCharge

- asset_path: /Game/RogueRigs/FX/NS_ShooterCharge (Niagara system, no Blueprint)
- parent_class: UNiagaraSystem
- purpose: red charge-up glow that grows at the cannon tip during the shooter's 0.6 s targeting laser ([[enemy-shooter]]). Not wired yet: BP_Shooter gets a component in the next subtask of task-298 ([[bp-shooter]]).
- user_params: none.
- emitters (both CPU sim, local space `bLocalSpace` true so it follows the muzzle, loop Infinite, inactive response Complete: Deactivate stops spawning and live particles finish, then the component ends; from the engine Fountain template):
  - `Glow`: spawn rate 1.6/s (one particle per 0.625 s, so a charge held past 0.6 s restarts), lifetime 0.6 s, one sprite at the emitter origin (shape, velocity, gravity, drag and ScaleColor modules removed), sprite size 24 cm (inside 15 to 30), ScaleSpriteSize uniform curve 0 to 1 linear over normalized age (grows from nothing to 24 cm over 0.6 s), colour HDR red (8, 0.25, 0.25, 1), default sprite material.
  - `Sparks`: spawn rate 20/s, lifetime 0.2 to 0.25 s, sphere shape radius 28 cm, AddVelocity mode From Point with speed -150 to -120 (negative = inward, reaches the centre in about 0.2 s), gravity 0, drag 0, sprite size 2 to 4 cm, colour HDR red (6, 0.25, 0.25, 1), template ScaleColor alpha fade kept, default sprite material.
- colour: red of the enemy shots and targeting laser ([[color-code]]); the HDR multiplier is provisional.
- particle_cost: at most about 6 live particles (Glow 1, Sparks 20 x 0.25 = 5), shadows off on the template renderers.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: engine DefaultSpriteMaterial.
- compile: system UpToDate, 0 errors, 0 warnings, 4 info notes (module version upgrade), saved.
- unverified: look and feel (nobody saw the viewport); sizes and speeds are guesses, tune after seeing it in game; the inward direction of the From Point velocity (negative speed) is read back but not seen; local-space following not tested in PIE.
- tools_that_worked: NiagaraToolset_System CreateNiagaraSystem (template NS_PickupSparkle, its emitter then removed), AddEmitter (Fountain x2), RemoveEmitter (`emitterToRemove`), RemoveModule (`moduleToRemove`), AddModule (ScaleSpriteSize), SetEmitterData (`emitter` + `emitterData.propertyValues` `{"bLocalSpace":true}`), SetStackInputData (nested `["Velocity Speed","Minimum"]`), GetStackIssues, GetSystemCompileState, AssetTools save_assets. The editor tools are reached through the `call_tool` wrapper (JSON-RPC, `toolset_name` + `tool_name`).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[enemy-shooter]] — the telegraph it shows.
- [[color-code]] — enemy shots and laser are red.
- [[bp-shooter]] — the Blueprint that will play it.
