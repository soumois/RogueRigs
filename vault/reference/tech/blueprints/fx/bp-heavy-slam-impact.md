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
- emitters (both CPU sim, loop behaviour Once, inactive response Complete, one SpawnBurst_Instantaneous at t=0, no SpawnRate; sprite renderers from the Fountain template):
  - `Dust`: burst 48, lifetime 0.45 to 0.5 s, ShapeLocation Ring / Disc (circle, ring radius 20) so particles start on a flat circle, AddVelocity From Point (radial, horizontal) speed `User.Radius * 6.67`, drag 6 (`x(t) = v/6 (1 - e^-6t)`: about 0.93 R at 0.3 s, about 1.1 R at the end), gravity 0, colour grey-brown (0.42, 0.35, 0.28) alpha 0.5, template ScaleColor alpha curve fades it over its life, random uniform size, default sprite material.
  - `Chips`: burst 14 (inside 10 to 20), lifetime 0.6 to 1.0 s, sphere shape radius 40, AddVelocity In Cone axis (0, 0, 1) angle 40 deg, speed 300 to 650, gravity Z -980, drag 0.3, random square size 4 to 8 with random rotation, colour fixed rock brown (0.30, 0.24, 0.20), material M_DigChip.
- colour: neutral rock browns and greys only.
- particle_cost: 62 particles at the burst (48 + 14), gone after about 1 s, shadows off on the template renderers.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: M_DigChip; engine DefaultSpriteMaterial for the dust.
- compile: system UpToDate, 0 errors, 0 warnings, 6 info notes (module version upgrade), saved.
- unverified: look and feel (nobody saw the viewport); ring radius and the "about 0.3 s" timing are computed from the drag formula, not measured; the From Point direction being horizontal for the Ring / Disc shape is read back as set, not seen; the alpha curve fades during the whole life, not only after 0.3 s; the ring is made of 48 camera-facing puffs, not a textured ring; not tested in PIE.
- tools_that_worked: AssetTools duplicate (NS_DigImpact), NiagaraToolset_System RemoveEmitter, AddModule (`SpawnBurst_Instantaneous`), RemoveModule (`SpawnRate`), AddUserVariables, RemoveUserVariables, SetStackInputData (Float, Int32, Vector3f, LinearColor, Enum, HlslExpression, nested `["Velocity Speed","Minimum"]`), GetModuleInputValues, GetStackIssues, GetSystemCompileState, NiagaraToolset_Info UEnum_Info (enum value names), AssetTools save_assets. An input hidden by a static switch (Cone Axis in From Point mode) refuses writes: switch the mode, set, switch back. A removed user variable returns while a hidden input still links it.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[enemy-heavy]] — the slam it shows.
- [[color-code]] — no gameplay colour.
- [[bp-heavy]] — its OnSlam spawns this system.
- [[bp-dig-impact]] — the system it was duplicated from, and M_DigChip.
