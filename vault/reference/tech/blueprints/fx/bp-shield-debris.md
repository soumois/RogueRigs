---
id: bp-shield-debris
description: "NS_ShieldDebris, the one-shot GPU Niagara burst of the five shield pieces on shield break; open for its offsets, velocity and deviations from NS_RobotDebris."
parent: blueprints-fx-index
kind: doc
---
# NS_ShieldDebris

- asset_path: /Game/RogueRigs/FX/NS_ShieldDebris (Niagara system, no Blueprint; created with CreateNiagaraSystem from NS_RobotDebris as template, task-222-03; NS_RobotDebris untouched)
- parent_class: UNiagaraSystem
- purpose: the five shield pieces (4 plate shards and the grip, `SM_ShieldDebris_*`) thrown from where the shield stood; same life (4.9 s), gravity, drag, spin, GPU collision and 4.5 s floor sink as [[bp-robot-debris]].
- user_params: `User.HitDirection` (Vector3f, default (1, 0, 0), task-224-02; added with AddUserVariables, a Vector is accepted), read back. `User.PieceMeshIndex` and `User.PieceOffset` stay removed (count and mesh index are fixed). The C++ caller sets HitDirection per activation (`GetShieldDebrisDirection`, task-224-01).
- components: one mesh emitter `Debris` (GPUComputeSim, fixed bounds (-2000, -2000, -200) to (2000, 2000, 1500) cm, inherited), world space, mesh renderer with the five meshes in json order (0 `SM_ShieldDebris_Shard_A`, 1 `_Shard_B`, 2 `_Shard_C`, 3 `_Shard_D`, 4 `_Grip`, folder /Game/RogueRigs/Art/ShieldChaser/Debris/), mesh materials kept, shadows off.
- spawn: SpawnBurst_Instantaneous at t = 0, Spawn Count = constant 5 (was the array count). InitializeParticle `Mesh Renderer Mesh Index` = HLSL expression `ExecIndex()` (was SelectIntFromArray on User.PieceMeshIndex), so each piece spawns once with mesh index = exec index. Lifetime 4.9 s, initial orientation Mode System (owner yaw).
- offsets: `Particles.PieceLocal` is a table keyed by MeshIndex (cm, unreal, from `Art/Source/shield-chaser/debris/shield_debris_pieces.json` `centre_offset_cm_unreal`, actor origin on the floor, front +X), added to the spawn position rotated by the owner quaternion (Position expression unchanged from NS_RobotDebris, including the +-10 cm jitter and the Z clamp to the owner Z). No other rotation: the meshes are already in the guard orientation.

| MeshIndex | piece | offset (x, y, z) cm |
|---|---|---|
| 0 | Shard_A | 32.42, -5.63, 42.89 |
| 1 | Shard_B | 30.15, 6.01, 87.22 |
| 2 | Shard_C | 30.15, -17.27, 90.82 |
| 3 | Shard_D | 33.17, -25.97, 28.51 |
| 4 | Grip | 26.75, -13.55, 51.5 |

- velocity (task-224-02, was owner forward +-15 degrees): the NS_RobotDebris expression, copied verbatim and read back identical: `Emitter.ThrowScale * ( float3(User.HitDirection.x*cos(Particles.ThrowAngle) - User.HitDirection.y*sin(Particles.ThrowAngle), User.HitDirection.x*sin(Particles.ThrowAngle) + User.HitDirection.y*cos(Particles.ThrowAngle), User.HitDirection.z)*(250.0+50.0*rand(1.0)) + float3(0.0,0.0,269.2+5.8*rand(1.0)) + float3(rand(2.0)-1.0, rand(2.0)-1.0, 0.0)*100.0 )` in the module `SetVariables_674A5FD1...` of the particle spawn script (ThrowScale 1.3 to 2.0 once per activation, ThrowAngle +-15 degrees per piece). Rest of the stack unchanged: Gravity -980, Drag 0.3, spin with half-life and rest, GPU Collision (distance fields, enabled while age < 4.5 s, radius 6, restitution 0.35, friction 0.5), sink velocity (0, 0, -140) from age 4.5 s.
- one_shot: system and emitter loop Once, duration 0.1 s, inactive response Complete (inherited).
- deviations: (1) one user parameter only (`User.HitDirection`), count and mesh index are fixed; (2) the owner rotation still places the offsets (guard-pose tilt) but no longer gives the throw direction; (3) the owner must be placed at the shield chaser actor origin with its yaw, because the offsets include the guard-pose tilt but not the actor yaw.
- events: none
- logic_in_bp: none
- assets: spawned by BP_ShieldChaser OnShieldBroken at the actor location and rotation, bAutoDestroy true ([[bp-shield-chaser]], task-222-04). Level change (task-389-01): the component is owned by the world settings (5.8), so `ARRShieldChaser::HandleShieldBroken` registers it with `URRNiagaraPoolSubsystem::TrackWorldEffect` and the ClearingLevel step stops it ([[sys-run-transition-preload]]).
- tools_that_worked: NiagaraToolset_System CreateNiagaraSystem (template NS_RobotDebris), SetRendererData (Meshes array replace, only `mesh` per entry), SetStackInputData (HlslExpression, NiagaraInt32 constant; the SpawnBurst module is in `EmitterUpdateScript`, not the spawn script), RemoveUserVariables, GetEmitterInputValues, GetStackIssues, GetSystemCompileState, AssetTools save_assets. Tools reach through `call_tool` with `toolset_name`.
- tools_that_failed: none new. `SetStackInputData` with scriptName `EmitterSpawnScript` for SpawnBurst_Instantaneous: module not found (wrong script).
- verified: task-224-02 change: user variable read back (Vector3f (1, 0, 0)), velocity expression read back identical to NS_RobotDebris, compiles UpToDate (all scripts incl. ParticleGPUComputeScript), no stack error or warning (2 info notes), saved; the compile query took over 2 minutes (GPU script). Throw direction and feel not run in PIE, unverified. Older: compiles UpToDate (system spawn/update, particle spawn/update, GPU compute scripts all UpToDate), no stack error or warning (2 version-upgrade info notes), renderer meshes read back (5 in order), Mesh Index expression read back, user variables empty, saved (not dirty). NOT verified: `ExecIndex()` giving index 0 to 4 in the GPU spawn script (compiled, but particle values on GPU are not readable), the throw feel, the pieces' look and collision; not run in PIE or in game.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-robot-debris]] — the system it was duplicated from.
- [[bp-shield-chaser]] — the Blueprint that spawns it.
- [[sys-run-transition-preload]] — the level change clear that stops it.
- [[enemy-death]] — design of the death debris.
