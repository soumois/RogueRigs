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
  - Damage ([[sys-damage]]): `BeginPlay` binds the health set `OnDamageReceived` and `OnDeath`. A hit leaving `Health` > 0 applies `IFramesEffectClass` to self (SetByCaller `Data.Duration` = `IFramesDuration`) in the same call stack, then the mesh blinks (hidden the first half of each cycle, `Tick`-driven on world time) and is shown again at the end. Death: input, aim, dig and movement stop (`StopMovementImmediately`), weapons `StopFiring`, blink ends, `OnDeath(Instigator)` (BlueprintAssignable) fires. `IsDead()`, `GetIFramesDuration()`, `GetHealthSet()`.
  - Light mask (task-272-07): default subobject `LightMask` (`URRPlayerLightMaskComponent`, `Source/RogueRigs/World/RRPlayerLightMaskComponent.h`) ticks in `TG_PostUpdateWork` and writes into the world instance of `MPC_PlayerLight` ([[mpp-player-light-mask]]): `PlayerPosition` = actor location, plus its tunables (`UPROPERTY(EditAnywhere)`, category `RogueRigs|LightMask`, provisional): `bMaskEnabled` false (-> `MaskEnabled` 0/1), `MaskRadius` 220, `MaskExponent` 2, `NearGain` 0, `FarGain` 1, `AddIntensity` 0, `NearColour` (1, 0.76, 0.62), `FarColour` (1, 1, 1); `ParameterCollection` defaults to `/Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight`. The instance is cached per world; the engine skips unchanged values; no tick on a dedicated server; a missing collection logs one warning and stops the tick. Defaults leave the picture unchanged. Runtime tuning (verified task-272-07): `setnopec /Game/RogueRigs/Maps/L_ArenaM1.L_ArenaM1:PersistentLevel.BP_Miner_C_0.LightMask MaskRadius 300`, read back by `getall RRPlayerLightMaskComponent MaskRadius`; the next tick pushes it.
  - Pickups: `GetPickupRadius()` (current `PickupRadius` attribute, 0 without the set), `NotifyPickupCollected(ERRPickupType)` broadcasts `OnPickupCollected(Type)` (BlueprintAssignable); no effect applied.
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
- [[mpp-player-light-mask]] — the post-process light mask whose collection the LightMask component writes each tick.
