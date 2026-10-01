---
id: sys-player
description: "Player C++ classes: miner (ASC, CSV stats, top-down camera, world-space move and aim, drone spawn, dig trigger), controller input routing, game mode."
parent: systems-index
kind: doc
---
# Player

- responsibility: the miner pawn (ASC, stats, camera, movement, aim, starting weapons), the input routing from Enhanced Input to the miner, the run game mode shell. No dash, shot, AoE or item logic; pickups only report collection to the miner ([[sys-pickups]]).
- main_classes (all in `Source/RogueRigs/Player/`, all `Abstract`, subclassed in Blueprint):
  - `ARRMiner : ACharacter, IAbilitySystemInterface` (`RRMiner.h`): `URRAbilitySystemComponent` plus `URRHealthAttributeSet`, `URRMovementAttributeSet`, `URRPlayerAttributeSet` as default subobjects (registered by the ASC's `InitializeComponent`). Spring arm with absolute rotation (never turns with the pawn) and camera. Editable: `BaseStatsRowName` (`Miner`), `CameraPitch` (−50), `CameraYaw` (0), `CameraArmLength` (2200), `CameraFieldOfView` (75), `AimRotationInterpSpeed` (0 = instant), `MouseAimPlaneHeight` (0), `StickAimMinInput` (0.1), `DroneClass` (none; set to the drone Blueprint), `DigReachMargin` (30 uu), `DigMinMoveInput` (0.1, provisional), `StartingWeapons` (array of `ARRWeapon` classes, empty; set to the weapon Blueprints), `WeaponAttachSocketName` (none = mesh origin). Holds the spawned drone in `Drone` (`GetDrone()`) and the arena grid in `ArenaGrid` (`GetArenaGrid()`). Camera lag and movement defaults (gravity 1.5, acceleration 1000, no ledge walk-off) are component properties, editable in Blueprint.
  - `ARRPlayerController` (`RRPlayerController.h`): editable `DefaultMappingContext`, `MappingContextPriority`, `MoveAction`, `StickAimAction`, `MouseAimAction`, `bHideCursorWhileStickAiming`. Cursor shown by default.
  - `ARRGameMode : AGameModeBase` (`RRGameMode.h`): empty shell; default pawn and controller classes set in Blueprint.
- entry_points:
  - `BeginPlay`: `InitAbilityActorInfo(this, this)`, subscribe to `MoveSpeed` changes, `FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(ASC, BaseStatsRowName)`, push `MoveSpeed` to `MaxWalkSpeed` ([[sys-gas-core]]), then spawn `DroneClass` at the miner (owner = miner) and call `SetOwnerMiner` ([[sys-drone]]), then store the first `ARRArenaGrid` of the world in `ArenaGrid` (none = warning, no digging), then spawns each `StartingWeapons` class (owner = miner), attaches it to the mesh at `WeaponAttachSocketName` and calls `ARRWeapon::Acquire` ([[sys-weapons]]); spawned weapons in `Weapons` (`GetWeapons()`). `EndPlay` destroys the drone and the weapons. `PossessedBy` calls `RefreshAbilityActorInfo`.
  - Pickups: `GetPickupRadius()` (current `PickupRadius` attribute, 0 without the set), `NotifyPickupCollected(ERRPickupType)` broadcasts `OnPickupCollected(Type)` (BlueprintAssignable); no effect applied.
  - `ApplyMoveInput(FVector2D)`, `ApplyStickAimInput(FVector2D)`, `UseMouseAim()`; `GetAimDirection()`, `IsUsingMouseAim()`, `GetRRAbilitySystemComponent()`.
  - Input axes: screen axes, X = right, Y = up, rotated by the arm's world yaw. A raw gamepad stick needs no modifier; WASD needs the usual swizzle on W/S and negate on S/A. Actions are bound on `Triggered`.
  - Dig trigger (end of `Tick`): `ApplyMoveInput` stores the world-space move input of the frame; `Tick` consumes it. If its length is above `DigMinMoveInput`, the direction is turned into grid space and reduced to its dominant axis (a diagonal picks one neighbour); the target is the miner's cell (`GetCellAtWorld`) + that step. If `IsBlockCell(target)` and the distance from the miner to the facing block face along the push axis is at most capsule radius + `DigReachMargin`, the miner calls `Drone->RequestDig(ArenaGrid, target, -step)` ([[sys-drone]]). The drone owns the cancel rule; releasing the push does not stop a dig.
  - Mouse aim: each tick the cursor ray is intersected with a horizontal plane at the capsule bottom + `MouseAimPlaneHeight`; a stick input switches to stick aim, any mouse aim trigger switches back.
- data: `PlayerBaseStats.csv` row `BaseStatsRowName`; `Config/DefaultGame.ini` stages `RogueRigs/Data` as non-UFS so tables and layouts ship.
- depends_on: [[sys-gas-core]], [[sys-grid]] (dig trigger cell queries), [[sys-pickups]] (`ERRPickupType`); modules `EnhancedInput`, `GameplayAbilities`.
- implements: [[miner]], [[player-stats]].
- gotchas:
  - `bUseControllerRotationYaw` and `bOrientRotationToMovement` are off: the pawn yaw follows only the aim.
  - Aim keeps its last direction while no aim input arrives; before the first input it is the spawn facing.
  - Inputs and assets are not wired until the Blueprint subclasses exist.
  - The `Variant_TwinStick` classes are reference only and stay untouched.

## Relations
- [[systems-index]] — parent index.
- [[sys-gas-core]] — ASC layout, attribute sets and the base-stat loader used at `BeginPlay`.
- [[miner]] — gameplay spec the pawn implements.
- [[sys-drone]] — the helper drone the miner spawns, destroys and asks to dig.
- [[sys-grid]] — the terrain the dig trigger queries.
- [[sys-pickups]] — pickups that read the pickup radius and fire the collection event.
- [[sys-weapons]] — the weapons the miner spawns and acquires at BeginPlay.
