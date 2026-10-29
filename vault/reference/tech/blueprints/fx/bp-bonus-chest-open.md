---
id: bp-bonus-chest-open
description: "NS_BonusChestOpen, the one-shot yellow turning rays and floating sparks played at the bonus chest lid opening; open for its emitters and values."
parent: blueprints-fx-index
kind: doc
---
# NS_BonusChestOpen

- asset_path: /Game/RogueRigs/FX/NS_BonusChestOpen (Niagara system, no Blueprint); material reused: /Game/RogueRigs/FX/Drone/M_DroneThruster (additive, unlit, radial soft falloff)
- parent_class: UNiagaraSystem
- purpose: effect played once at the lid opening of the bonus chest ([[bp-bonus-chest]], [[objective-bonus-chest]]): light rays turning in the open chest and sparks floating up. No flash (replaces Q12). Fired by the ARRBonusChest event OnLidOpened(Location); the wiring is task-309-05, not this node. Created from NS_HeavySlamImpact as template ([[bp-heavy-slam-impact]]), its two emitters and four user variables removed, two Fountain emitters added.
- user_params: none.
- colour: linear company yellow (1, 0.58, 0), the same as MI_ChestInterior (task-309-02, [[color-code]]), as the InitializeParticle Color of both emitters; alpha 1, faded to 0 over life by the template ScaleColor alpha curve (1.01 at t=0 to 0.002 at t=1).
- emitters (CPU sim, loop behaviour Once, inactive response Complete, one SpawnBurst_Instantaneous at t=0, no SpawnRate, sprite renderer with M_DroneThruster, shadows off):
  - `Rays`: burst 4, lifetime 3.0 s, spawn sphere radius 0 (all at the component origin), AddVelocity, GravityForce and Drag disabled. A SetVariables module in Particle Update sets `Particles.SpriteSize` (14, 260) cm (thin, tall), `Particles.SpriteAlignment` (0, 0, 1) and `Particles.SpriteFacing` HLSL `float3(cos(a), sin(a), 0)` with `a = Particles.UniqueID * 0.7853982 + Particles.Age * 0.5235988` (blades 45 deg apart, turning 0.5236 rad/s = 30 deg/s about the vertical axis). Renderer: Alignment CustomAlignment, FacingMode CustomFacingVector, pivot (0.5, 0) so the blade grows upward from the origin. Each ray is a vertical blade, so 4 blades read as a star of 8 half-rays.
  - `Sparks`: burst 20, lifetime 2.0 to 2.5 s, spawn sphere radius 20, AddVelocity cone axis (0, 0, 1) angle 25 deg speed 40 cm/s (fixed), gravity (0, 0, 0), drag 0, uniform sprite size 3 to 6 cm, default renderer settings except material.
- compile: system UpToDate, 0 errors, 0 warnings, 5 info notes (module version upgrade), saved. Values above read back with GetEmitterInputValues.
- particle_cost: 24 particles, gone after 3 s.
- unverified: the look and the turning (nobody saw it: the `Particles.UniqueID` angle spread assumes IDs 0 to 3, compile only; the blade orientation with CustomFacingVector is not seen); position of the effect inside the open chest (the caller sets the location, task-309-05); the 260 cm ray height against the chest size; not played in PIE.
- tools_that_worked: NiagaraToolset_System CreateNiagaraSystem (template NS_HeavySlamImpact), RemoveEmitter, AddEmitter (Fountain), RemoveUserVariables (needs name and type objects), RemoveModule, AddModule (SpawnBurst_Instantaneous), AddSetParametersModule then SetStackInputData HLSL, SetStackInputData (enums: Loop Behavior Once = `ENiagara_EmitterStateOptions::NewEnumerator1`; a plain name is silently ignored), SetModuleEnabled, SetRendererData, GetEmitterInputValues, GetStackIssues, GetSystemCompileState; AssetTools save_assets.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-bonus-chest]] — the chest that plays it.
- [[bp-heavy-slam-impact]] — the system it was created from.
- [[objective-bonus-chest]] — the objective design.
- [[color-code]] — company yellow.
