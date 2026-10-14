---
id: bp-robot-death-puff
description: "NS_RobotDeathPuff, the one-shot Niagara puff of 12 warm sprites played when a chaser or shield chaser dies; open for its emitter settings and pooling."
parent: blueprints-fx-index
kind: doc
---
# NS_RobotDeathPuff

- asset_path: /Game/RogueRigs/FX/NS_RobotDeathPuff (Niagara system, no Blueprint; built from the NS_PickupSparkle template, same folder as the other FX)
- parent_class: UNiagaraSystem
- purpose: hides the 0.15 s shrink of a dying robot ([[enemy-death]]); played at the robot centre by `URRNiagaraPoolSubsystem` ([[sys-actor-pool]], [[sys-enemies-base]] `DeathPuffSystem`, pool 40).
- components: one CPU sprite emitter `OmnidirectionalBurst` (engine DefaultSpriteMaterial, shadows off).
- exposed_defaults (task-167-02, provisional, values read back): SpawnBurst_Instantaneous count 12 at t = 0; particle lifetime 0.2 to 0.3 s; sphere location radius 35 cm around the actor location (robot centre, about 60 cm above the floor); AddVelocity from point, speed 60 to 260 cm/s, drag 0.75 (spread about 16 to 70 cm); sprite size 14 to 50 cm uniform random (the big ones form the puff that covers the 70 cm wide robot); colour HDR warm orange (3, 1.3, 0.25); alpha fade from the template curve; gravity 0.
- one_shot: SystemState loop behaviour Once, loop duration set to 0.1 s (was 2 s), inactive response Complete: the system ends once the last particle dies, about 0.3 to 0.4 s, so the pooled component returns. Emitter loop settings follow the system (Life Cycle Mode System), their inputs are gated.
- colour: warm spark orange, a detail colour; no new gameplay meaning ([[color-code]]).
- events: none
- logic_in_bp: none
- assets: used by BP_Chaser ([[bp-chaser]]) and BP_ShieldChaser ([[bp-shield-chaser]]) as DeathPuffSystem.
- verified: compiles (UpToDate, no errors); PIE: 40 pooled NS_RobotDeathPuff components prewarmed at BeginPlay, no LogNiagara error. Active then released state of a component is not readable through MCP (no property exposes it): not verified; the one-shot guarantee rests on the loop settings above. Look and cost not judged.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[enemy-death]] — the design of the death puff.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[sys-enemies-base]] — the C++ DeathPuffSystem property.
- [[color-code]] — colours.
- [[bp-chaser]] — user.
- [[bp-shield-chaser]] — user.
