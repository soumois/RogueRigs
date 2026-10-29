---
id: sys-automation-tests
description: "Headless automation tests hub: harness, command, gotchas and index of the test families. Open to find which child lists a given test."
parent: systems-tools-index
kind: doc
---
# Automation tests

- responsibility: prove gameplay rules headless, where MCP cannot drive PIE input: dig rules (task-006-05), render-only shake (task-012-01), weapon and damage rules (task-007-05), player damage rules (task-009-01), game over (task-009-02), flow field (task-009-03), enemies (task-009-04), spawner (task-009-05), spawn packs (task-014-01), the whole chaser loop with the final Blueprints and a first timing read (task-009-07), the explosion loop with the final Blueprints (task-011-07), XP and level (task-012-02), the XP loop with the final Blueprints (task-012-04), upgrades (task-014-02), the level-up flow (task-014-03), the level-up upgrade loop with a real pause and the cheat variables (task-014-05), the shotgun (task-015-01), the run flow, console and cheat property route (task-016-01), the console and extraction loop with the final Blueprints, a real pause and the end screen (task-016-04), the shooter (task-021-01), the shooter loop with the final Blueprints and the placed spawner mix (task-021-03), the heavy and the per-class spawner cap (task-022-01), the heavy loop with the final Blueprints and the four-class placed spawner mix (task-022-03), rifle pierce and the piercing upgrade (task-024-01), shotgun pellets (task-024-02), new-weapon cards (task-024-04), the weapons loop with the final Blueprints (task-024-06), damage numbers (task-025-01), HUD pressure events (task-026-02), the pressure loop with the placed spawner config and the real WBP_HUD widgets, and the light halo cheat (task-026-04), the juice loop with the final Blueprints (task-027-03).
- harness: per test, a transient Game `UWorld` (no game mode: `AWorldSettings::NotifyBeginPlay` dispatches BeginPlay), `BP_ArenaGrid` at the origin, `BP_Miner`, `BP_BurstRifle` (acquired without miner: aim = +X), still chasers as hit targets (`RRTestHarness::SpawnStillChaser`, task-031-01; the target dummy is removed), ticked at 1/60 s. Protected members read via pointer-to-member in never-instantiated derived structs. `RRIntegrationTests.cpp` helpers: `KillChasersOneByOne`, `CollectXPCubes`, `StartGameMode` (a `UGameInstance` wired to the world context, then `UWorld::SetGameMode` with `DefaultGameMode` = `BP_RRGameMode`: real pause, player state, widgets with a world; `FTestGameInstance` declared before the world). A `WBP_HUD` created by a test is held by a `TStrongObjectPtr`: `UWorld::Tick` may run `ConditionalCollectGarbage` and nothing else references the widget (a long loop crashed on a collected HUD).
- entry_points (tests): one child node per test family, listed under Relations; the test name `RogueRigs.<Family>.*` tells which one to open.
- data: `OreTypes.csv`, `arena_m1` layout (logic tests, 200 uu: their cells were chosen on it and their rules do not depend on the cell size), `arena_m1_fine` (tests of cell-counted rules: dig times and drops, drone cancel, spawn band, console, shuttle, and the `Integration.ConsoleExtractionWin` / `ShuttleTimeoutLoss` loops, miner on (121, 121)), `XPCurve.csv`, `ExplosionStats.csv`, `EnemyStats.csv`, `PlayerBaseStats.csv`, `WeaponStats.csv`, `UpgradeValues.csv`, `UpgradeRarities.csv`, the Blueprint classes above. Headless command in [[build-toolchain]].
- depends_on: [[sys-grid]], [[sys-drone]], [[sys-enemies]], [[sys-pickups]], [[sys-player]], [[sys-weapons]], [[sys-auto-fire]], [[sys-projectiles]], [[sys-damage]].
- implements: none (verification).
- gotchas: task-030-01 slowed every robot by 20 % (chaser 288 uu/s): the travel times measured above for robots (reach, arm, kill distances) predate it unless stated; the tests still pass with their checks unchanged (184 + 1 run green). Unity builds regroup test files when one is added: a file-scope `using namespace` (e.g. `RRLevelUpTests`) then makes same-named helpers ambiguous (`TickFrames` in `RRTurretTests.cpp`, qualified in task-029-07); keep helper calls qualified or usings inside functions. `Integration.Perf50Chasers` after task-012-01 (chasers at 360 uu/s): world tick avg 1.963 ms, median 1.386 ms, p95 4.395 ms, max 13.711 ms. The tick task manager runs a tick function once per `GFrameCounter`; `FTestWorld::Tick` increments it before each `World->Tick` (without it actors tick once). Timing tests call `AdvanceTime` without ticking the world. Tests depend on `arena_m1` cells (42–44, 38), (42–45, 41), (44, 42), (40–43, 40), (49, 45), (34, 34), (32–41, 42), (46, 40), (30–36, 42–44), (38, 39–44), barrels (42–43, 6), (73, 31–32), (30, 27), (24, 48–49) shot from (22–23, 49), (35, 65–66) with floor (36, 65); row 44 (32–40) with rocks (37–41, 45); flow field: (40, 40), (41, 40), (39, 35–37), (38, 48) and the five start cells above. An unpossessed character stays in `MOVE_None` and never moves: tests that walk the miner set `bRunPhysicsWithNoController` and `SetDefaultMovementMode()` (a player controller without local player is not locally controlled, so it does not help).
- test worlds (task-372-01, de889932, 0e85c9a0): `RRTestHarness::FTestWorld` calls `URRRunFlowSubsystem::SetArrivalClearsFootprint(false)`, so the arrival extractor does not clear the scenes built next to the miner. Dig, Explosions and Extraction groups: 31/31 pass with the change.

## Relations
- [[sys-tests-dig-ore]] — Open to change terrain tests.
- [[sys-tests-weapons]] — Open to change rifle or projectile tests.
- [[sys-tests-weapon-types]] — Open to change turret launcher or shotgun pellet tests.
- [[sys-tests-player-cheats]] — Open to change player or cheat tests.
- [[sys-tests-progression]] — Open to change XP, upgrade or level-up tests.
- [[sys-tests-flowfield-spawner]] — Open to change navigation or spawn tests.
- [[sys-tests-enemies]] — Open to change robot behaviour tests.
- [[sys-tests-hud-text]] — Open to change HUD or text pipeline tests.
- [[sys-tests-pools-pickups]] — Open to change pooling, prewarm or pickup tests.
- [[sys-tests-juice-damage-numbers]] — Open to change feedback or visual-effect tests.
- [[sys-tests-run-integration]] — Open to change full-run flow tests.
- [[sys-tests-explosions-barrel]] — Open to change explosion or barrel tests.
- [[sys-tests-classes]] — Open to find which file holds a test.
- [[systems-index]] — parent index.
- [[sys-hud-events]] — the HUD event tests.
- [[build-toolchain]] — the headless command that runs these tests.
- [[sys-grid]] — the terrain rules tested.
- [[sys-drone]] — the dig state machine tested.
- [[sys-text-content]] — the text loader tested by RogueRigs.Text.*.
- [[sys-subtitles]] — the subtitle subsystem tested by RogueRigs.Subtitles.*.
- [[sys-pickups]] — the drop and collection rules tested.
- [[sys-auto-fire]] — the burst and reload rules tested.
- [[sys-spawner]] — the spawn rules tested.
- [[perf-risks]] — where the integration timing run is recorded.
- [[sys-progression]] — the XP and level rules tested.
- [[sys-level-up]] — the level-up flow rules tested.
- [[sys-ground-telegraph]] — the ground telegraph tested by RogueRigs.GroundTelegraph.*.
- [[sys-cheats]] — the cheat variables and properties tested.
- [[sys-objectives]] — the run flow and console rules tested.
- [[sys-enemies-heavy]] — the heavy rules tested.
- [[sys-damage-numbers]] — the damage number rules tested.
- [[sys-hud-pressure]] — the HUD pressure events tested.
- [[sys-enemies-shield-chaser]] — the shield rules tested.
- [[sys-actor-pool]] — the pool rules tested.
