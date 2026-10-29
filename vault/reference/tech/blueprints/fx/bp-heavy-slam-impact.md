---
id: bp-heavy-slam-impact
description: "NS_HeavySlamImpact, the one-shot grey-brown dust ring (expands to User.Radius) and rock chips at the heavy's slam; open for its user parameter, emitters and values."
parent: blueprints-fx-index
kind: doc
---
# NS_HeavySlamImpact

- asset_path: /Game/RogueRigs/FX/NS_HeavySlamImpact (Niagara system, no Blueprint); chip material /Game/RogueRigs/FX/Dig/M_DigChip
- parent_class: UNiagaraSystem
- purpose: impact effect when the heavy's slam lands ([[enemy-heavy]]): a flat dust ring expanding to the slam radius, and rock chips thrown up near the centre. Neutral rock colours, no gameplay colour ([[color-code]]). Duplicated from NS_DigImpact ([[bp-dig-impact]]) then reworked. Played by BP_Heavy OnSlam since task-300-02 ([[bp-heavy]]), which sets User.Radius to the slam Radius (provisional, owner 2026-10-23).
- user_params:
  - `User.Radius`: float (`SetVariableFloat`), default 361 (the current slam radius, cm). Scales the ring: the Dust emitter's AddVelocity speed is the HLSL `User.Radius * 6.67` and its sprite size is `User.Radius * 0.15` to `* 0.22`, so the ring edge reaches about the radius whatever the value. The chips do not scale.
  - `User.DustColor`: LinearColor (`SetVariableLinearColor`), default (0.18, 0.13, 0.09) alpha 0.7 (the old fixed dust colour). Linked to the Dust emitter InitializeParticle `Color` input (Direct Set), so the default look is unchanged. Added by task-309-03 so the chest can reuse the ring with another colour (technique G).
  - `User.ChipCount`: int (`SetVariableInt`), default 16 (the old burst). Linked to the Chips emitter SpawnBurst_Instantaneous `Spawn Count`. Added by task-309-03.
- emitters (both CPU sim, loop behaviour Once, inactive response Complete, one SpawnBurst_Instantaneous at t=0, no SpawnRate; sprite renderers from the Fountain template):
  - `Dust`: burst 48, lifetime 0.42 to 0.45 s, ShapeLocation Sphere radius 0 (every particle starts at the component origin, Z 0), AddVelocity Linear with the HLSL vector `float3(cos(Particles.UniqueID * 0.1309), sin(Particles.UniqueID * 0.1309), 0.0) * User.Radius * 2.6` (0.1309 = 2 pi / 48, so the 48 particles go out evenly in the ground plane, Z velocity exactly 0; the edge reaches R at 0.385 s, about 1.15 R at the end), drag 0, gravity 0, colour dark grey-brown (0.18, 0.13, 0.09) alpha 0.7, template ScaleColor alpha curve fades it, random uniform size `User.Radius * 0.15` to `* 0.22`, default sprite material.
  - `Chips`: burst 16 (inside 10 to 20), lifetime 0.6 to 1.0 s, sphere shape radius 40, AddVelocity In Cone axis (0, 0, 1) angle 40 deg, speed 300 to 650, gravity Z -980, drag 0.3, random square size 10 to 18 with random rotation, colour rock brown (0.45, 0.32, 0.20), material M_DigChip.
- fix_task-300-04 (capture 300-03: ring edge only about 150 uu, drawn raised, whitish, chips specks), before -> after: Dust velocity From Point speed `User.Radius * 6.67` with drag 6 and a Ring/Disc shape (radius 20) -> Linear explicit horizontal vector, drag 0, Sphere radius 0; Dust lifetime 0.45-0.5 -> 0.42-0.45; Dust colour (0.42, 0.35, 0.28) alpha 0.5 -> (0.18, 0.13, 0.09) alpha 0.7; Chips size 4-8 -> 10-18; Chips colour (0.30, 0.24, 0.20) -> (0.45, 0.32, 0.20); Chips burst 14 -> 16. Causes (not proven, no capture tool allowed): drag 6 with the Niagara Drag module (mass and integration) probably stopped the ring far short of the computed 0.93 R; the Ring/Disc circle orientation and From Point direction were unverified and may have put the ring off the ground plane (a raised, face-on donut matches the capture); the washed-out colour is probably exposure/bloom on a pale translucent sprite, so the colour is darker and more opaque; the chips were too small and too dark. The HLSL expressions compile (0 errors); whether `Particles.UniqueID` is populated at spawn is read back as compiling, not seen.
- colour: neutral rock browns and greys only.
- particle_cost: 62 particles at the burst (48 + 14), gone after about 1 s, shadows off on the template renderers.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: M_DigChip; engine DefaultSpriteMaterial for the dust.
- compile: system UpToDate, 0 errors, 0 warnings, 6 info notes (module version upgrade), saved. After task-309-03: still UpToDate, 0 errors, both new defaults read back equal to the old values (GetUserVariables), both links read back (GetStackInputData), BP_Heavy compiles (no error returned).
- unverified: look and feel after the fix (not captured); the ring reaching R on the ground and the 0.385 s timing are computed (no drag), not measured; that the 48 UniqueID angles spread evenly (IDs assumed to start at 0 and increase by 1 per burst); not tested in PIE.
- tools_that_worked: AssetTools duplicate (NS_DigImpact), NiagaraToolset_System RemoveEmitter, AddModule (`SpawnBurst_Instantaneous`), RemoveModule (`SpawnRate`), AddUserVariables, RemoveUserVariables, SetStackInputData (Float, Int32, Vector3f, LinearColor, Enum, HlslExpression, nested `["Velocity Speed","Minimum"]`), GetModuleInputValues, GetStackIssues, GetSystemCompileState, NiagaraToolset_Info UEnum_Info (enum value names), AssetTools save_assets. An input hidden by a static switch (Cone Axis in From Point mode) refuses writes: switch the mode, set, switch back. A removed user variable returns while a hidden input still links it.
- owner_agent: agent-bp
- owner_decision: "On garde l'impact au sol comme ça" (owner 2026-10-23): the light beige-tan dust, faded ring edge and small chips are accepted, no rework.

## Relations
- [[blueprints-fx-index]] — parent index.
- [[enemy-heavy]] — the slam it shows.
- [[color-code]] — no gameplay colour.
- [[bp-heavy]] — its OnSlam spawns this system.
- [[bp-dig-impact]] — the system it was duplicated from, and M_DigChip.
