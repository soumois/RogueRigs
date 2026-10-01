---
id: sys-automation-tests
description: "C++ automation tests as built: RogueRigs.Dig.* (terrain, drops, drone, pickups) and RogueRigs.Weapons.* (burst timing, reload, projectiles, damage, dummy), harness, gotchas."
parent: systems-index
kind: doc
---
# Automation tests

- responsibility: prove gameplay rules headless, where MCP cannot drive PIE input: dig rules (task-006-05), weapon and damage rules (task-007-05).
- main_classes (`Source/RogueRigs/Tests/`, code inside `WITH_DEV_AUTOMATION_TESTS`, simple tests, flags `EditorContext | EngineFilter`): `RRTestHarness.h` (namespace `RRTestHarness`: `FTestWorld`, `LoadTestClass`, `SpawnGrid`, `SpawnMiner`, `FrameSeconds`, cells (43, 38) rock and (42, 38) floor); `RRDigTests.cpp`; `RRWeaponTests.cpp`; `URRTestEventRecorder` (UCLASS, so in every configuration; never used by gameplay): UFunctions for `OnPickupCollected`, `OnCellDestroyed`, auto-fire `OnShotFired` / `OnReloadStarted` / `OnReloadFinished` (samples `Ammo` of a set ASC), `OnProjectileStop`.
- harness: per test, a transient Game `UWorld` (no game mode: `AWorldSettings::NotifyBeginPlay` dispatches BeginPlay), `BP_ArenaGrid` at the origin, `BP_Miner`, `BP_BurstRifle` (acquired without miner: aim = +X), `BP_TargetDummy` (lifted so its bounds rest on the floor), ticked at 1/60 s. Protected members read via pointer-to-member in never-instantiated derived structs.
- entry_points (tests):
  - `RogueRigs.Dig.RockDestroy`, `HealOreDrops`, `DestroyCellsInRadius`, `DroneDig`, `DroneDigCancel`, `PickupCollect`: terrain damage and destruction times, ore drops, radius destruction, drone dig and cancel, pickup collection ([[sys-grid]], [[sys-drone]], [[sys-pickups]]).
  - `RogueRigs.Weapons.BurstTiming`: `AdvanceTime` alone at 1/60 s; exact shot times and firing frame (±1) at `FireRate` 2 and 3 ([[sys-auto-fire]] example).
  - `Weapons.MagazineReload`: shot 30 at 4.66 s, 1.5 s reload with `Ammo` 0, `Ammo` 30 at its end, shot 31 at 6.16 s.
  - `Weapons.ProjectileStopsOnRock`: shot from (42, 38) stops on rock (43, 38) at frame 2, no damage to the rock or a dummy behind it.
  - `Weapons.ProjectileDamagesDummy`: dummy on (44, 41), shot from (42, 41): `CritChance` 0 → −10, not critical; `CritChance` 1 → −15, critical.
  - `Weapons.DummyRefill`: hits at 0 and 1 s, refill 180 frames after the last; a 5000 hit leaves 0 then refills at once, no `State.Dead`.
  - `Weapons.MinerStartingWeapon`: `BP_BurstRifle` attached at `hand_r`, acquired, firing; muzzle must sit between floor and block top. Fails: 130 uu ([[bp-burst-rifle]]).
- data: `arena_m1` layout, `PlayerBaseStats.csv`, `WeaponStats.csv`, the Blueprint classes above. Headless command in [[build-toolchain]].
- depends_on: [[sys-grid]], [[sys-drone]], [[sys-pickups]], [[sys-player]], [[sys-weapons]], [[sys-auto-fire]], [[sys-projectiles]], [[sys-damage]], [[sys-target-dummy]].
- implements: none (verification).
- gotchas: the tick task manager runs a tick function once per `GFrameCounter`; `FTestWorld::Tick` increments it before each `World->Tick` (without it actors tick once). Timing tests call `AdvanceTime` without ticking the world. Tests depend on `arena_m1` cells (42–44, 38), (42–44, 41), (49, 45).

## Relations
- [[systems-index]] — parent index.
- [[build-toolchain]] — the headless command that runs these tests.
- [[sys-grid]] — the terrain rules tested.
- [[sys-drone]] — the dig state machine tested.
- [[sys-pickups]] — the drop and collection rules tested.
- [[sys-auto-fire]] — the burst and reload rules tested.
- [[sys-target-dummy]] — the dummy rules tested.
