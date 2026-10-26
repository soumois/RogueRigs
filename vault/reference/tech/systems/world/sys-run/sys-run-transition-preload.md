---
id: sys-run-transition-preload
description: "Next level prepared in the same world behind the cards screen: approach, steps, readiness and progress API, map-free Continue. Open to change the preload."
parent: sys-run-transition
kind: doc
---
# Run transition — next level preload (task-318-01)

- responsibility: from the cards screen opening (`NextLevel`, levels 1-3), prepare the next level in the background so `Continue` starts it without a blocking map load ([[intermission]]).
- approach (choice and reason): rebuild in the same world, no `OpenLevel`. All run levels share L_ArenaM1 and differ only by the grid layout ([[sys-run-layout]]), so a second world is not needed; a background map load is not possible for the map already loaded (same package), and travelling earlier would only move the 2.3-3.3 s blocking load to the stats screen. Keeping the world also keeps the actor pool warm (no new prewarm) and keeps the miner itself as the carried state (cards, weapons, level, XP, health identical by construction; ore stays in the bank).
- main_classes: `URRRunTransitionSubsystem` (`Run/`), steps `ERRNextLevelPrepStep` (None, WaitingForLayout, ClearingLevel, RebuildingTerrain, PlacingMiner, Ready). Resets added: `URRRunFlowSubsystem::ResetForNextLevel`, `ARREnemySpawner::ResetForNextLevel`, `ARRArenaGrid::RebuildForRunLevel`, `URRFlowFieldSubsystem::RebuildForNewTerrain`, `URRExplosionSubsystem::ClearForNextLevel`, `URRRunSubsystem::ConsumeSnapshotInPlace`, `URRRunLogSubsystem::HandleLevelStartedInPlace`, `URRLevelUpComponent::ResumeChoices`; `ARRPlayerController::HandleNextLevelReady` recreates the HUD widget.
- entry_points (sequence; one step per frame on the core ticker, the world stays paused):
  1. WaitingForLayout until the layout requested at the extraction is built (failed: error, grid falls back to its own layout).
  2. ClearingLevel: run flow reset (console, chest, shuttle destroyed; alert, kills, objective flags, carried ore); drone dig released; every `ARREnemy`, `ARRPickup`, `ARRProjectile`, `ARRExplosionFX`, `ARRTurret` back to the pool (pool members) or destroyed; explosion queue and spent sources cleared; spawner clock, director and hordes reset.
  3. RebuildingTerrain: grid rebuilt from the prepared layout.
  4. PlacingMiner: miner (and drone) teleported to the game mode's player start, flow field recomputed.
  5. Ready: `OnNextLevelReady(LevelIndex, LayoutName)`; the controller replaces the HUD by a fresh one (collapsed until `OnLevelReady`).
  - `Continue`: shop closed, phase Loading, `OnLoadingStarted`, stage PreparingLevel; ready: `StartPreparedLevel` at once; else when the preparation ends. `StartPreparedLevel`: snapshot consumed in place (log line compares miner and snapshot values), run log level start, level-up choices allowed again, unpause, game input, phase None, `OnLevelReady`.
  - Pure (Blueprint): `IsNextLevelReady()`, `GetNextLevelProgress()` (0-1: completed work steps / 4), `GetNextLevelPrepStep()`.
- data: none.
- depends_on: [[sys-run-layout]], [[sys-grid]], [[sys-objectives-run-flow]], [[sys-spawner-pool-prewarm]], [[sys-hud-bindings]], [[sys-run-log]].
- implements: [[intermission]].
- gotchas: fallback to the map travel of [[sys-run-transition]] when the preparation cannot start (no world, grid or miner). Restart and death still reopen the map and reset to level 1. Differences with the travel path: turret launcher charges and active timed gameplay effects of the miner are kept; Blueprint level actors do not rerun BeginPlay. Each step is one synchronous frame: the terrain rebuild takes about 0.5 s in one frame, so the cards screen freezes for that frame (candidate for time-slicing). No automation test (owner rule). Verified by a -game -nullrhi log (task-318-01, Saved/Logs/task318_game.log): level 2 preparation started on Stats.Next, cleared 0.2 ms, terrain arena_l2 240 x 240 rebuilt in 534 ms, miner placed and flow field 2.2 ms, ready after 539 ms, 3.45 s before Continue; Continue to level ready 0.7 ms, snapshot values equal to the miner values. Level 3 with Continue pressed in the same frame as Stats.Next: ready after 497 ms, level started 504 ms after Continue. Restart and death paths unchanged, not exercised by that run.

## Relations
- [[sys-run-transition]] — parent: the transition this preload shortens.
- [[sys-run-layout]] — the layout the terrain step builds from.
- [[sys-grid]] — rebuilt in place.
- [[sys-objectives-run-flow]] — reset in place.
- [[sys-spawner-pool-prewarm]] — the pool kept warm across levels.
- [[sys-hud-bindings]] — the controller that recreates the HUD.
- [[sys-run-log]] — level counters restarted at Continue.
- [[intermission]] — the cards screen it hides behind.
- [[task-318-01-preload-next-level]] — the task that built it.
