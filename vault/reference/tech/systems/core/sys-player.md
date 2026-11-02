---
id: sys-player
description: "Player C++ classes: miner (ASC, CSV stats, top-down camera, world-space move and aim, drone spawn, dig trigger), controller input routing, game mode."
parent: systems-core-index
kind: doc
---
# Player

- responsibility: the miner pawn (ASC, stats, camera, movement, aim, starting weapons), the input routing from Enhanced Input to the miner, the run game mode (game over on death, level restart). No dash, shot, AoE or item logic; pickups only report collection to the miner ([[sys-pickups]]); XP and level live in its progression component ([[sys-progression]]).
- main_classes (all in `Source/RogueRigs/Player/`, all `Abstract`, subclassed in Blueprint): see [[sys-player-classes]] for the three classes.
- entry_points:
  - `BeginPlay`: `InitAbilityActorInfo(this, this)`, subscribe to `MoveSpeed` changes, `FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(ASC, BaseStatsRowName)`, push `MoveSpeed` to `MaxWalkSpeed` ([[sys-gas-core]]), then spawn `DroneClass` at the miner (owner = miner) and call `SetOwnerMiner` ([[sys-drone]]), then store the first `ARRArenaGrid` of the world in `ArenaGrid` (none = warning, no digging), then spawns each `StartingWeapons` class, then each `GuaranteedWeaponClasses` entry not provided (owner = miner), attaches it (mesh socket `WeaponAttachSocketName`, or the capsule at `WeaponAttachOffset` when none), makes the capsule ignore it when moving and calls `ARRWeapon::Acquire` ([[sys-weapons]]); spawned weapons in `Weapons` (`GetWeapons()`). Each acquisition goes through `AddWeapon(Class)` (BlueprintCallable; also for runtime weapons) and broadcasts `OnWeaponAdded(Weapon)`. `OwnsWeaponClass(Class)` (a valid weapon is that class or a subclass), `GetOwnedWeaponCount()`, `CanAcquireWeapon(Class)` (not owned and below `MaxWeapons`), `GetAcquirableWeapons()` (catalogue entries acquirable now), `GetWeaponCatalogue()`, `GetMaxWeapons()`. `FindTurretLauncher()`; `PlaceTurret()` (refused when dead) asks the launcher to place a turret ([[sys-turret]]). `EndPlay` destroys the drone and the weapons. `PossessedBy` calls `RefreshAbilityActorInfo`.
  - Damage ([[sys-damage]]): `BeginPlay` binds the health set `OnDamageReceived` and `OnDeath`. A hit leaving `Health` > 0 applies `IFramesEffectClass` to self (SetByCaller `Data.Duration` = `IFramesDuration`) in the same call stack, then the mesh blinks (hidden the first half of each cycle, `Tick`-driven on world time) and is shown again at the end. Death: input, aim, dig and movement stop (`StopMovementImmediately`), weapons `StopFiring`, blink ends, `OnDeath(Instigator)` (BlueprintAssignable) fires. Hit shake (task-417-01, 07c780b9, provisional): the same `OnDamageReceived` handler (`HandleDamageReceived`) first plays `HitShakeClass` (default `URRHitCameraShake`, 5 uu, 0.2 s, 25 Hz) through `URRCameraShakeLibrary::PlayCameraShake` when `Damage` > 0, lethal hit included, before the i-frames branch; hits refused by the i-frames never reach it ([[sys-camera-shake]]). `IsDead()`, `GetIFramesDuration()`, `GetHealthSet()`.
  - Light mask (task-272-07, task-272-13, task-274-01): default subobject `LightMask` (`URRPlayerLightMaskComponent`, `Source/RogueRigs/World/RRPlayerLightMaskComponent.h`) ticks in `TG_PostUpdateWork` and writes into the world instance of `MPC_PlayerLight` ([[mpp-player-light-mask]]). By default it writes only the vector `PlayerPosition` = actor location: `MPC_PlayerLight` is the owner's single tuning place (editing its defaults in the editor updates live). `bDriveTunables` (false, `EditAnywhere`, category `RogueRigs|LightMask`) is for console sweeps only: when true it also pushes each tick, overriding the collection defaults, the scalar tunables named exactly as the MPC parameters: `Enable` 1 (0/1), `FarBrightness` 0.005, `Radius` 200 uu, `Exponent` 3. The mask is a pure darkener (gain 1 at the miner, no tint); `CenterBrightness`, `TintStrength`, `WarmTint` and `FarTint` were removed from the component in task-274-01. `ParameterCollection` defaults to `/Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight`. The instance is cached per world; the engine skips unchanged values and silently ignores a name missing from the collection (returns false, no log); no tick on a dedicated server; a missing collection logs one warning and stops the tick. Console path: `setnopec /Game/RogueRigs/Maps/L_ArenaM1.L_ArenaM1:PersistentLevel.BP_Miner_C_0.LightMask <Prop> <Value>` (for example `bDriveTunables 1`, then `Radius 300`), read back by `getall RRPlayerLightMaskComponent <Prop>`; the next tick pushes it. Setting `bDriveTunables` back to 0 stops the pushes but the world instance keeps the last pushed values until the level reloads.
  - Aim indicator (task-328-03, reworked in task-415): default subobject `AimIndicator` (`URRAimIndicatorComponent`, a scene component that draws the aim laser from the weapon Muzzle), `GetAimIndicator()`; `HandleDeath` calls `HideIndicator()` ([[sys-aim-indicator]]).
  - Pickups: `GetPickupRadius()` (current `PickupRadius` attribute, 0 without the set), `NotifyPickupCollected(ERRPickupType)` broadcasts `OnPickupCollected(Type)` (BlueprintAssignable); no effect applied.
  - Screen up (task-371-01, 66b2c5e2): inline `GetScreenUpDirection()` returns the unit XY world direction of screen up seen through the fixed camera (camera arm yaw, from `ScreenInputToWorld`); [[sys-objectives-run-flow]] uses it to place the arrival extractor.
- data: `PlayerBaseStats.csv` row `BaseStatsRowName`; `Config/DefaultGame.ini` stages `RogueRigs/Data` as non-UFS so tables and layouts ship.
- depends_on: [[sys-gas-core]], [[sys-grid]] (dig trigger cell queries), [[sys-pickups]] (`ERRPickupType`); modules `EnhancedInput`, `GameplayAbilities`.
- implements: [[miner]], [[player-stats]].
- gotchas:
  - `bUseControllerRotationYaw` and `bOrientRotationToMovement` are off: the pawn yaw follows only the aim.
  - Collision: the capsule responds Overlap to the `Projectile` channel ([[sys-projectiles]]), so no projectile, own or enemy, ever blocks it, while projectile overlap events (hits by overlap) still fire. Ignore would also suppress those overlaps.
  - A held weapon with blocking collision inside the capsule pushes the miner sideways each move (depenetration): measured in task-008-01, `BP_BurstRifle` `RifleMesh` (`BlockAllDynamic`) on `hand_r` turned 1 s of +X input into 66 uu forward and 350 uu sideways. Weapons have no actor collision and the capsule ignores them when moving.
  - Aim keeps its last direction while no aim input arrives; before the first input it is the spawn facing.
  - Inputs and assets are not wired until the Blueprint subclasses exist.
  - The `Variant_TwinStick` classes are reference only and stay untouched.

## Relations
- [[sys-player-classes]] — Player classes: ARRMiner, ARRPlayerController, ARRGameMode with their properties. Open to change miner, controller or game mode setup.
- [[sys-player-input-aim]] — Player input axes, mouse and stick aim, dig trigger by contact. Open to change controls or aiming.
- [[systems-index]] — parent index.
- [[sys-gas-core]] — ASC layout, attribute sets and the base-stat loader used at `BeginPlay`.
- [[miner]] — gameplay spec the pawn implements.
- [[sys-drone]] — the helper drone the miner spawns, destroys and asks to dig.
- [[sys-grid]] — the terrain the dig trigger queries.
- [[sys-pickups]] — pickups that read the pickup radius and fire the collection event.
- [[sys-weapons]] — the weapons the miner spawns and acquires at BeginPlay.
- [[sys-hud]] — the HUD widget the controller creates, fed by the miner health and the game over.
- [[sys-progression]] — the XP and level component the miner owns.
- [[sys-turret]] — the turret launcher the place-turret input drives.
- [[sys-camera-shake]] — the shared shake played on this camera through URRCameraShakeLibrary::PlayCameraShake.
- [[sys-aim-indicator]] — the aim laser component the miner owns.
- [[mpp-player-light-mask]] — the post-process light mask whose collection the LightMask component writes each tick.
