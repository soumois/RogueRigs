---
id: bp-robot-debris
description: "NS_RobotDebris, the one-shot Niagara mesh system throwing 2 or 3 robot pieces on death; open for its parameters, modules and contract deviations."
parent: blueprints-fx-index
kind: doc
---
# NS_RobotDebris

- asset_path: /Game/RogueRigs/FX/NS_RobotDebris (Niagara system, no Blueprint; created from the NS_RobotDeathPuff template, its sprite emitter removed, emitter `Debris` added from the engine template UpwardMeshBurst)
- parent_class: UNiagaraSystem
- purpose: debris pieces thrown by a dying chaser or shield chaser at the end of the 0.15 s shrink ([[enemy-death]]); played at the robot feet by `URRNiagaraPoolSubsystem` ([[sys-actor-pool]], [[sys-enemies-base]] `DebrisSystem`, pool 40).
- user_params: `User.PieceMeshIndex` (Array Int32), `User.PieceOffset` (Array Vector), `User.HitDirection` (Vector). Created through a Linked stack input (AddUserVariables silently ignores data interface types).
- components: one CPU mesh emitter `Debris`, world space, mesh renderer with the 13 `SM_ChaserDebris_*` meshes in report order (0 Torso, 1/2 UpperArm L/R, 3/4 Forearm, 5/6 Arm, 7/8 Thigh, 9/10 LowerLeg, 11/12 Leg), mesh materials kept (no override), shadows off.
- spawn: SpawnBurst_Instantaneous at t = 0, count = dynamic input GetInt32ArrayCount(User.PieceMeshIndex). InitializeParticle: lifetime 1.2 s (direct), colour white, scale 1, Mesh Renderer Mesh Index = Direct Set with SelectIntFromArray(User.PieceMeshIndex, index = ReturnExecIndex) (writes Particles.MeshIndex). Initial orientation Mode System (robot yaw).
- spawn_sets (SetParameters, HLSL expressions): `Particles.PieceLocal` = 13-entry table of `centre_offset_cm_unreal` keyed by Particles.MeshIndex; `Particles.Position` += PieceLocal rotated by `Engine.Owner.Rotation`; `Particles.Velocity` = HitDirection x (250 + 200 rand) + up (300 + 200 rand) + horizontal spread 100; `Particles.SpinRate` random vector.
- update: ParticleState (kill at lifetime), GravityForce -980 world, UpdateMeshOrientation rate 1.5 rev/s about Particles.SpinRate, Drag 0.3, ScaleMeshSize = saturate((1 - NormalizedAge) / 0.25) (shrinks over the last 0.3 s), Collision (CPU Analytical Planes, one plane, normal +Z, position `Engine.Owner.Position`, world space, radius 6 cm, restitution 0.35, friction 0.5; no scene query), SolveForcesAndVelocity last (Collision requires it after).
- one_shot: system loop Once, duration 0.1 s, inactive response Complete (inherited from the puff template, read back); emitter loop Once, duration 0.1 s; last particle dies at 1.2 s, then the pooled component returns.
- deviations: (1) `User.PieceOffset` is declared and linked but NOT read: no Niagara module or dynamic input reads an Array Vector element by index (SelectVectorFromArray picks at random; DI calls in an HLSL expression fail to compile). The offset is rebuilt from a baked table of the 13 offsets keyed by MeshIndex and rotated by the component rotation, which reproduces the C++ yaw-rotated offset exactly as long as DebrisPieces keep the json offsets. (2) No sink: pieces only shrink in the last 0.3 s.
- events: none
- logic_in_bp: none
- assets: used by BP_Chaser ([[bp-chaser]]) and BP_ShieldChaser ([[bp-shield-chaser]]) as DebrisSystem.
- tools_that_worked: NiagaraToolset_System CreateNiagaraSystem, AddEmitter, RemoveEmitter, AddModule, RemoveModule, AddSetParametersModule, SetStackInputData (DynamicInput, Linked, HlslExpression, Enum), SetRendererData, GetStackIssues, GetSystemCompileState. Compile is asynchronous: wait a few seconds then read GetStackIssues (compile errors appear there and in compile events).
- tools_that_failed: AddUserVariables with a data interface default (no error, nothing added); HLSL expression calling a DI function.
- verified: compiles (UpToDate, no stack issue); renderer meshes, user params and loop settings read back; PIE with the system set on both robots: 11 chasers killed by the miner, no LogNiagara error or warning during the run. Pool prewarm count and the active then released state of a component are not readable through MCP: not verified. Look, motion feel and cost not judged.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[enemy-death]] — the design of the debris.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[sys-enemies-base]] — the C++ contract and DebrisSystem property.
- [[bp-chaser]] — user.
- [[bp-shield-chaser]] — user.
- [[bp-robot-death-puff]] — sibling FX the system was cloned from.
