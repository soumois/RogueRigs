---
id: bp-kamikaze-death-puff
description: "NS_KamikazeDeathPuff, the former 12-sprite warm robot puff kept for the kamikaze death; open for its values or to change that death."
parent: blueprints-fx-index
kind: doc
---
# NS_KamikazeDeathPuff

- asset_path: /Game/RogueRigs/FX/NS_KamikazeDeathPuff (Niagara system, no Blueprint; copy of NS_RobotDeathPuff taken before task-436-01 rebuilt it).
- parent_class: UNiagaraSystem
- purpose: keeps the kamikaze death unchanged (acceptance A6 of [[task-436-robot-death-particle]]). BP_Kamikaze referenced NS_RobotDeathPuff, and `ARRKamikaze::FinishDeath` calls `ARREnemy::HandleDeath`, which plays DeathPuffSystem on top of the explosion.
- components: one CPU sprite emitter `OmnidirectionalBurst` (engine DefaultSpriteMaterial, shadows off).
- exposed_defaults (task-167-02, provisional): SpawnBurst_Instantaneous count 12 at t = 0; particle lifetime 0.2 to 0.3 s; sphere location radius 35 cm; AddVelocity from point, speed 60 to 260 cm/s, drag 0.75; sprite size 14 to 50 cm uniform random; colour HDR warm orange (3, 1.3, 0.25); alpha fade from the template curve; gravity 0.
- one_shot: SystemState loop behaviour Once, loop duration 0.1 s, inactive response Complete; emitter Life Cycle Mode System.
- colour: warm spark orange ([[color-code]]).
- events: none
- logic_in_bp: none
- assets: DeathPuffSystem of [[bp-kamikaze]] only (pool 40).
- verified: duplicated, saved, read back on the BP_Kamikaze default object; BP_Kamikaze compiled. Look not judged.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-robot-death-puff]] — the system it was copied from.
- [[bp-kamikaze]] — its only user.
- [[color-code]] — colours.
