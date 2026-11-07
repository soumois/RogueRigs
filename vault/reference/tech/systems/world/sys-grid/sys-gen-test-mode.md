---
id: sys-gen-test-mode
description: "Enemy-free generator test mode as built: ARRGenTestGameMode, rr.Level.New [seed], in-place async reload, seed line, logs and timings. Open to use or change the test map."
parent: sys-grid
kind: doc
---
# Generator test mode

- responsibility: the dev test map of [[task-494-generator-test-map]] (owner Q5, 2026-11-07): the miner and drone play on a layout from [[sys-layout-generator]], loaded by [[sys-layout-loader]], with no robots, console, bonus chest, shuttle or alert bar; a console command generates and loads a new layout as often as wanted (commits 6334081b, d4809047, dc57cc4a).
- main_classes: `ARRGenTestGameMode` (`Player/RRGenTestGameMode.h/.cpp`, Abstract child of `ARRGameMode`; Blueprint [[bp-gen-test-game-mode]]), `URRGenTestSeedWidget` (`UI/RRGenTestSeedWidget.h/.cpp`, native text, no Blueprint).
- entry_points:
  - Console, test map only: `rr.Level.New [seed]`. No seed = random seed 1 to 999999; a positive seed starts the search there (the accepted seed may be higher: 1234 gives 1236, 4321 gives 4327). Elsewhere it logs a warning and does nothing. Refused while a generation or load runs.
  - `RequestNewLayout(Seed)` (BlueprintCallable), `IsBusy()`, `GetCurrentSeed()`.
- flow: `StartPlay`, before the actors' BeginPlay, marks the map grid external, destroys every `ARREnemySpawner` and turns off the run flow (`SetRunFlowEnabled(false)`) and the arrival extractor (`URRRunFlowSubsystem::SetArrivalEnabled(false)`, added here). Each request: `FRRLayoutGenerator::Generate` on a worker task; on the game thread the JSON goes to `Saved/GeneratedLevels/<LayoutFilePrefix><seed>.json`; pickups, projectiles, explosion effects and turrets of the previous layout are removed (pool members released); the map grid is rebuilt in place by `URRLayoutLoaderSubsystem::StartLoadIntoGrid` (added here), so the miner, flow field, explosions and HUD keep the same grid pointer. The miner is frozen (movement none) during the load, and during the generation too for the first layout; at the end it and the drone go on the spawn cell, the flow field is rebuilt and the HUD minimap recreated (`URRHUDWidget::ResetMinimap`, added here).
- display: seed line at the top centre, 20 pt outlined ("Generating layout, seed N...", "Loading layout...", then "Seed N (rr.Level.New [seed] for a new layout)"); `HiddenHUDWidgetNames` collapses HUD panels by name (AlertObjectives). The load percentage is only logged (Q5).
- logs: `rr.Level.New:` lines with requested and accepted seed, seeds tried, generation time, counts, JSON crc, file path, actors removed, load time and frames, cell match; `rr.Level test mode watch` every `WatchLogSeconds` (robots, consoles, chests, shuttles, run flow, alert, seed).
- data: Blueprint defaults `StartSeed` (0 = random), `ParamsPath` (empty = generator default params), `LayoutFilePrefix` `gen_test_`, `HiddenHUDWidgetNames`, `WatchLogSeconds` 30.
- measures (task-494, -game 1600 x 900): request to playable 3.0 to 4.7 s, worst 6.9 s (14 seeds tried); generation 0.4 to 4.6 s, load 2.1 to 2.6 s over 125 to 155 frames; same seed twice gives the same JSON crc.
- depends_on: [[sys-layout-generator]], [[sys-layout-loader]], [[sys-grid]].
- implements: none in the gdd (dev tool).
- gotchas: the map grid logs "LayoutName is empty" once before StartPlay (harmless). During a reload the old blocks vanish before the new ones build, so the frozen miner floats over the void for about 2 s. The default params path is under `Tools/`: editor and `-game` only, the map is not cooked.

## Relations
- [[sys-grid]] — parent; the grid rebuilt in place.
- [[sys-layout-generator]] — writes each layout.
- [[sys-layout-loader]] — builds it in place over frames.
- [[bp-gen-test-game-mode]] — the Blueprint and the L_GenTest map.
- [[task-494-generator-test-map]] — the task that built it.
