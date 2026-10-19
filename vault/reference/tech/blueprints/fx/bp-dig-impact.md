---
id: bp-dig-impact
description: "NS_DigImpact, the looping Niagara system at the drone laser impact point (sparks, chips, dust); open for its exact User parameters, emitters and values."
parent: blueprints-fx-index
kind: doc
---
# NS_DigImpact

- asset_path: /Game/RogueRigs/FX/Dig/NS_DigImpact (Niagara system, no Blueprint); material /Game/RogueRigs/FX/Dig/M_DigChip
- parent_class: UNiagaraSystem
- purpose: effect where the drone laser cuts a wall: white-gold sparks, chips of the block colour, a little dust ([[drone]], [[color-code]]). Played by task-233-02 on one pooled component at `ARRDrone::GetDigImpactPoint()` ([[bp-drone]]), which sets the User parameters below.
- user_params (exact names; set from C++ with the matching setter):
  - `User.Intensity`: float, default 1.0, range 0..1. Multiplies the spawn rate of the three emitters (HLSL `User.Intensity * rate`); 0 spawns nothing, so C++ can fade by lowering it.
  - `User.ChipColor`: LinearColor (`SetVariableLinearColor`), default (0.35, 0.28, 0.5, 1) lavender rock. Read by emitter Chips as the particle colour (Color Direct Link); C++ sets lavender rock, violet Duskite, red heal per [[color-code]].
  - `User.WallNormal`: Vector3f (`SetVariableVec3`), default (1, 0, 0). Unit vector pointing away from the wall, world space. The three emitters use it as the AddVelocity cone axis (the cone is in simulation space, the emitters are world space, no rotation of the component needed). The default is a placeholder: always set it; a zero vector gives no direction.
- emitters (all CPU sim, loop Infinite, from the engine Fountain template, Sparks renderer VelocityAligned):
  - `Sparks`: spawn 35/s x Intensity (about 3.5 per 0.1 s), lifetime 0.18 to 0.38 s, speed 350 to 700, cone 35 deg around WallNormal, gravity Z -900, drag 0.1, colour HDR white-gold (6, 4.6, 1.6, 1) (no red hue), sprite 1.4 x 10 non-uniform (streak along velocity), default sprite material.
  - `Chips`: spawn 25/s x Intensity, lifetime 0.35 to 0.6 s, speed 150 to 380, cone 55 deg, gravity Z -980, drag 0.3, square sprite 2.5 to 4.5 random size and random rotation (faceted chunk look), material M_DigChip (Unlit, Opaque, emissive = particle colour, flag bUsedWithNiagaraSprites), colour = User.ChipColor.
  - `Dust`: spawn 12/s x Intensity, lifetime 0.5 to 0.9 s, speed 20 to 70, cone 60 deg, gravity Z +25 (rises slightly), drag 1.5, size 12 to 22, colour lavender-grey (0.5, 0.46, 0.58) alpha 0.22 (template ScaleColor alpha curve fades it), default sprite material.
- particle_cost: at most about 70 live particles at Intensity 1 (35 x 0.38 + 25 x 0.6 + 12 x 0.9 = 13 + 15 + 11), shadows off on all renderers.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: M_DigChip (Opaque unlit material, emissive from Particle Color); engine DefaultSpriteMaterial for Sparks and Dust.
- compile: system UpToDate, 0 errors, 0 warnings, 9 info notes (module version upgrade notes), saved.
- unverified: look and feel (nobody saw the viewport); speeds are guesses, tune the Velocity Speed minimum and maximum of each emitter after seeing it in game; the cone-axis direction is read back as linked to User.WallNormal but not tested in PIE; DefaultSpriteMaterial softness of the dust not seen.
- tools_that_worked: NiagaraToolset_System CreateNiagaraSystem (template NS_PickupSparkle, its emitter then removed), AddEmitter (Fountain x3), RemoveEmitter, AddUserVariables (Vector3f only takes the default with struct `/Script/CoreUObject.Vector3f`), SetStackInputData (Hlsl, Linked, Enum, floats, vectors, nested dynamic input `['Velocity Speed','Minimum']`), SetRendererData, GetStackIssues, GetSystemCompileState, MaterialTools create_material, add_expression, connect_to_output, ObjectTools set_properties (`bUsedWithNiagaraSprites`, not `bUsedWithParticleSprites`).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[drone]] — the laser design.
- [[color-code]] — chip colours.
- [[readability-rules]] — no red in the sparks.
- [[bp-drone]] — the drone that owns the impact point.
