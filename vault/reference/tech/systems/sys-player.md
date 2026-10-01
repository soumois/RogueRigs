---
id: sys-player
description: "Player C++ classes: miner with ASC and CSV base stats, fixed top-down camera, world-space move and aim, drone spawn, player controller input routing, game mode."
parent: systems-index
kind: doc
---
# Player

- responsibility: the miner pawn (ASC, stats, camera, movement, aim), the input routing from Enhanced Input to the miner, the run game mode shell. No dash, shot, AoE, pickup or item logic.
- main_classes (all in `Source/RogueRigs/Player/`, all `Abstract`, subclassed in Blueprint):
  - `ARRMiner : ACharacter, IAbilitySystemInterface` (`RRMiner.h`): `URRAbilitySystemComponent` plus `URRHealthAttributeSet`, `URRMovementAttributeSet`, `URRPlayerAttributeSet` as default subobjects (registered by the ASC's `InitializeComponent`). Spring arm with absolute rotation (never turns with the pawn) and camera. Editable: `BaseStatsRowName` (`Miner`), `CameraPitch` (−50), `CameraYaw` (0), `CameraArmLength` (2200), `CameraFieldOfView` (75), `AimRotationInterpSpeed` (0 = instant), `MouseAimPlaneHeight` (0), `StickAimMinInput` (0.1), `DroneClass` (none; set to the drone Blueprint). Holds the spawned drone in `Drone` (`GetDrone()`). Camera lag and movement defaults (gravity 1.5, acceleration 1000, no ledge walk-off) are component properties, editable in Blueprint.
  - `ARRPlayerController` (`RRPlayerController.h`): editable `DefaultMappingContext`, `MappingContextPriority`, `MoveAction`, `StickAimAction`, `MouseAimAction`, `bHideCursorWhileStickAiming`. Cursor shown by default.
  - `ARRGameMode : AGameModeBase` (`RRGameMode.h`): empty shell; default pawn and controller classes set in Blueprint.
- entry_points:
  - `BeginPlay`: `InitAbilityActorInfo(this, this)`, subscribe to `MoveSpeed` changes, `FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(ASC, BaseStatsRowName)`, push `MoveSpeed` to `MaxWalkSpeed` ([[sys-gas-core]]), then spawn `DroneClass` at the miner (owner = miner) and call `SetOwnerMiner` ([[sys-drone]]). `EndPlay` destroys the drone. `PossessedBy` calls `RefreshAbilityActorInfo`.
  - `ApplyMoveInput(FVector2D)`, `ApplyStickAimInput(FVector2D)`, `UseMouseAim()`; `GetAimDirection()`, `IsUsingMouseAim()`, `GetRRAbilitySystemComponent()`.
  - Input axes: screen axes, X = right, Y = up, rotated by the arm's world yaw. A raw gamepad stick needs no modifier; WASD needs the usual swizzle on W/S and negate on S/A. Actions are bound on `Triggered`.
  - Mouse aim: each tick the cursor ray is intersected with a horizontal plane at the capsule bottom + `MouseAimPlaneHeight`; a stick input switches to stick aim, any mouse aim trigger switches back.
- data: `PlayerBaseStats.csv` row `BaseStatsRowName`; `Config/DefaultGame.ini` stages `RogueRigs/Data` as non-UFS so tables and layouts ship.
- depends_on: [[sys-gas-core]]; modules `EnhancedInput`, `GameplayAbilities`.
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
- [[sys-drone]] — the helper drone the miner spawns and destroys.
