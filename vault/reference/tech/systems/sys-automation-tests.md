---
id: sys-automation-tests
description: "C++ automation tests as built: RogueRigs.Dig.* (terrain, drops, drone, pickups) and RogueRigs.Weapons.* (burst timing, reload, projectiles, damage, dummy, shot origin, moving while firing), harness, gotchas."
parent: systems-index
kind: doc
---
# Automation tests

- responsibility: prove gameplay rules headless, where MCP cannot drive PIE input: dig rules (task-006-05), weapon and damage rules (task-007-05).
- main_classes (`Source/RogueRigs/Tests/`, code inside `WITH_DEV_AUTOMATION_TESTS`, simple tests, flags `EditorContext | EngineFilter`): `RRTestHarness.h` (namespace `RRTestHarness`: `FTestWorld`, `LoadTestClass`, `SpawnGrid`, `SpawnMiner` (deferred: an optional hook runs before BeginPlay), `FrameSeconds`, cells (43, 38) rock and (42, 38) floor); `RRDigTests.cpp`; `RRWeaponTests.cpp`; `URRTestEventRecorder` (UCLASS, so in every configuration; never used by gameplay): UFunctions for `OnPickupCollected`, `OnCellDestroyed`, auto-fire `OnShotFired` / `OnReloadStarted` / `OnReloadFinished` (samples `Ammo` of a set ASC), `OnProjectileStop`, `OnComponentHit`.
- harness: per test, a transient Game `UWorld` (no game mode: `AWorldSettings::NotifyBeginPlay` dispatches BeginPlay), `BP_ArenaGrid` at the origin, `BP_Miner`, `BP_BurstRifle` (acquired without miner: aim = +X), `BP_TargetDummy` (lifted so its bounds rest on the floor), ticked at 1/60 s. Protected members read via pointer-to-member in never-instantiated derived structs.
- entry_points (tests):
  - `RogueRigs.Dig.RockDestroy`, `HealOreDrops`, `DestroyCellsInRadius`, `DroneDig`, `DroneDigCancel`, `PickupCollect`: terrain damage and destruction times, ore drops, radius destruction, drone dig and cancel, pickup collection ([[sys-grid]], [[sys-drone]], [[sys-pickups]]).
  - `RogueRigs.Weapons.BurstTiming`: `AdvanceTime` alone at 1/60 s; CSV values; exact shot times and firing frame (±1) at `FireRate` 1.25 (gaps 0.08 s, pause 0.64 s) and 3 (0.033 s, 0.267 s) ([[sys-auto-fire]] example).
  - `Weapons.MagazineReload`: shot 30 at 7.36 s, 1.5 s reload with `Ammo` 0, `Ammo` 30 at its end, shot 31 at 8.86 s.
  - `Weapons.ProjectileStopsOnRock`: shot from (42, 38) stops on rock (43, 38) at frame 2, no damage to the rock or a dummy behind it.
  - `Weapons.ProjectileDamagesDummy`: dummy on (44, 41), shot from (42, 41): `CritChance` 0 → −10, not critical; `CritChance` 1 → −15, critical.
  - `Weapons.DummyRefill`: hits at 0 and 1 s, refill 180 frames after the last; a 5000 hit leaves 0 then refills at once, no `State.Dead`.
  - `Weapons.MinerStartingWeapon`: miner with `WeaponAttachSocketName` forced to none, walking on (42, 38): `BP_BurstRifle` on the capsule at `WeaponAttachOffset`, pointing along the aim, no actor collision, acquired, firing; capsule Overlap on `Projectile`; one shot spawns `ShotHeight` (50) above the floor, below `BlockHeight`, capsule radius + `ShotForwardOffset` (54 uu) ahead, and stops on rock (43, 38) at frame 1.
  - `Weapons.MinerMovesWhileFiring`: miner (weapons on the capsule) pushed +X by `AddMovementInput` for 1 s from (30, 43), once with the weapon stopped, once firing: 425 uu both (kinematic 420), no lateral drift, no projectile or non-floor capsule hit; firing must keep >= 90 % of the distance.
- data: `arena_m1` layout, `PlayerBaseStats.csv`, `WeaponStats.csv`, the Blueprint classes above. Headless command in [[build-toolchain]].
- depends_on: [[sys-grid]], [[sys-drone]], [[sys-pickups]], [[sys-player]], [[sys-weapons]], [[sys-auto-fire]], [[sys-projectiles]], [[sys-damage]], [[sys-target-dummy]].
- implements: none (verification).
- gotchas: the tick task manager runs a tick function once per `GFrameCounter`; `FTestWorld::Tick` increments it before each `World->Tick` (without it actors tick once). Timing tests call `AdvanceTime` without ticking the world. Tests depend on `arena_m1` cells (42–44, 38), (42–44, 41), (49, 45), (30–36, 42–44). An unpossessed character stays in `MOVE_None` and never moves: tests that walk the miner set `bRunPhysicsWithNoController` and `SetDefaultMovementMode()` (a player controller without local player is not locally controlled, so it does not help).

## Relations
- [[systems-index]] — parent index.
- [[build-toolchain]] — the headless command that runs these tests.
- [[sys-grid]] — the terrain rules tested.
- [[sys-drone]] — the dig state machine tested.
- [[sys-pickups]] — the drop and collection rules tested.
- [[sys-auto-fire]] — the burst and reload rules tested.
- [[sys-target-dummy]] — the dummy rules tested.
