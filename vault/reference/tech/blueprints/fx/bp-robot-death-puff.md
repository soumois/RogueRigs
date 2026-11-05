---
id: bp-robot-death-puff
description: "NS_RobotDeathPuff and M_CartoonBlast materials: the one-shot cartoon flash of 18 sprites when a robot dies; open for emitters, timing, colours and pooling."
parent: blueprints-fx-index
kind: doc
---
# NS_RobotDeathPuff

- asset_path: /Game/RogueRigs/FX/NS_RobotDeathPuff (Niagara system, no Blueprint); materials /Game/RogueRigs/FX/M_CartoonBlastCore, M_CartoonBlastBlob, M_CartoonBlastLine.
- parent_class: UNiagaraSystem
- purpose: cartoon flash that hides the 0.15 s shrink of a dying robot ([[enemy-death]]); played at the robot centre by `URRNiagaraPoolSubsystem` ([[sys-actor-pool]], [[sys-enemies-base]] `DeathPuffSystem`, pool 40). Look after the owner reference `Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png`; no pixel of it is used, the materials have no texture.
- components (task-436-01, provisional, values read back): 4 CPU sprite emitters from the engine OmnidirectionalBurst template, shadows off, gravity 0, ScaleSpriteSize off, all with position offset z +60 cm (top of the robot in the top-down view). 18 sprites per death:
  - Lines (sort 0): 6 at t = 0.03 s, life 0.14 to 0.2 s, M_CartoonBlastLine, velocity aligned, size 7 x 70 to 120 cm, ring radius 70 cm, speed 550 to 800 cm/s, drag 4, colour (1.7, 0.8, 0.05).
  - Core (sort 1): 1 at t = 0, life 0.32 s, M_CartoonBlastCore, size 280 cm, random rotation; ShapeLocation, AddVelocity and Drag off.
  - Blobs (sort 2): 6 at t = 0.06 s, life 0.22 to 0.28 s, M_CartoonBlastBlob, size 34 to 50 cm, ring radius 50 cm, speed 450 to 650 cm/s, drag 4, colour (1.2, 0.3, 0.02).
  - Puffs (sort 3): 5 at t = 0.17 s, life 0.2 to 0.25 s, M_CartoonBlastBlob, size 42 to 66 cm, ring radius 75 cm, speed 40 to 90 cm/s, drag 2, colour (0.2, 0.022, 0.015).
- velocity: AddVelocity From Point with origin offset z +60, so the throw stays flat on the ring plane instead of rising.
- progress: ScaleColor `Scale Alpha` = `Particles.NormalizedAge`; the materials read the particle alpha as progress 0 to 1, not as a fade.
- materials (Translucent, Unlit, two sided, used with Niagara sprites; one Custom node each, inputs UV and C = ParticleColor RGBA; RGB to emissive, alpha to opacity; edges anti-aliased with fwidth):
  - Core: jagged edge from triangle waves of the angle; radius grows from 0.42 to 0.88 of the disc by progress 0.45; bands white (2.6, 2.5, 2.2), yellow (1.7, 1.15, 0.1), orange (1.25, 0.38, 0.02), red-orange (0.8, 0.1, 0.015), each cooling one step from progress 0.55 to 0.95 (outer one to dark red 0.17, 0.016, 0.012); all white up to progress 0.12; a jagged hole opens from progress 0.6 and closes the disc at the end; faint warm glow up to progress 0.9.
  - Blob: lumpy edge, grows until progress 0.18, shrinks from 0.55; inner band C.rgb, outer band half of it.
  - Line: thin diamond, shrinks in length from progress 0.45.
- one_shot: SystemState loop behaviour Once, loop duration 0.2 s (was 0.1, the Puffs burst at 0.17 s must fire), inactive response Complete; each emitter Life Cycle Mode System. The last particle ends at about 0.42 s, the pooled component then returns.
- colour: white, yellow, orange and red as in the reference, red included by the owner ([[readability-rules]] "Colour rules are not rigid"); [[color-code]].
- events: none
- logic_in_bp: none
- assets: DeathPuffSystem of BP_Chaser ([[bp-chaser]]), BP_ChaserV2, BP_ShieldChaser ([[bp-shield-chaser]]), BP_ShieldChaserV2, BP_Shooter. BP_Kamikaze has its own copy of the former puff: [[bp-kamikaze-death-puff]].
- verified: compile state UpToDate, 0 errors, 0 warnings; 3 materials compile; all values above read back; PIE start without a LogNiagara error.
- simulation_check (task-448-01, no asset change): 5 real kills in PIE (`rr.Debug.KillRobots`, slomo 0.05), `fx.Niagara.DumpComponents full`: per puff Lines 6 at t 0.03, life 0.144 to 0.199, 7 x 72 to 117 cm; Core 1 at t 0, life 0.32, 280 cm; Blobs 6 at t 0.06, life 0.221 to 0.279, 34 to 49 cm; Puffs 5 at t 0.17, life 0.201 to 0.249, 43 to 66 cm; colours as above, z 124; alpha = NormalizedAge on every row (error 0). No LogNiagara warning. Probe traps: an open level-up screen pauses the world (puffs stay Active with 0 particles); a background editor throttles PIE to about 3 fps.
- not_verified: the look in game (owner validates, task-436 A7, no capture); that the 0.42 s end really releases the component (not readable through MCP); cost at horde scale (no stress run). Formula preview, not a game frame: `Saved/Scratch/robot-death-fx/20261105-formula-preview-not-a-game-frame.png` (temporary).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[enemy-death]] — the design of the death flash.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[sys-enemies-base]] — the C++ DeathPuffSystem property.
- [[color-code]] — colours.
- [[readability-rules]] — the owner line allowing red here.
- [[bp-chaser]] — user.
- [[bp-shield-chaser]] — user.
- [[bp-heavy-death-puff]] — same look at 3x.
- [[bp-kamikaze-death-puff]] — copy of the former puff.
