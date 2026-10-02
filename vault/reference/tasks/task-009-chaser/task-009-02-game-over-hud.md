---
id: task-009-02-game-over-hud
description: "Game-over flow on miner death (input cut, event, level restart after 3 s) and a C++ HUD widget base fed with health and game-over events."
parent: task-009-chaser
kind: task
status: done
---
# Game over and HUD base

- goal: the run ends cleanly on death and the UI layer gets the data it needs from C++.
- agent: cpp
- depends_on: task-009-01-player-damage
- inputs: [[sys-player]], [[sys-damage]], [[miner]], [[run-structure]], [[cpp-bp-split]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARRGameMode` listens to the miner's death: disables player input, broadcasts `OnGameOver`, restarts the current level after an editable `RestartDelay` (default 3 s) via `RestartGame` or reopening the map. `URRHUDWidget` (abstract `UUserWidget`) with `BlueprintImplementableEvent`s `OnHealthChanged(Current, Max)` and `OnGameOver()`, bound by C++ to the miner's health attribute and the game mode; `ARRPlayerController` creates the widget from an editable class at begin play. New doc `sys-hud` (template `system.md`) linked from [[systems-index]]; [[sys-player]] updated. Tests: game over fires once on death, restart timer is armed with the configured delay (do not actually travel in the test).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 60000
- milestone: M1
- result: ARRGameMode RegisterMiner (called by the miner at BeginPlay), OnGameOver once, input disabled, RestartDelay 3 s then OpenLevel(current); URRHUDWidget (OnHealthChanged, OnGameOver; binds ASC Health and MaxHealth); ARRPlayerController HUDWidgetClass, created only with a local player. Test Player.GameOver. Build exit 0, 18/18 (orchestrator re-ran). New doc sys-hud; sys-player, sys-automation-tests, systems-index, build-toolchain. About 84k tokens.

## Relations
- [[task-009-chaser]] — parent global task.
