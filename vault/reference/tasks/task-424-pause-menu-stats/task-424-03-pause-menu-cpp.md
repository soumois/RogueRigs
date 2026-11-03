---
id: task-424-03-pause-menu-cpp
description: "Cpp: pause input and flow, stats snapshot (global and per weapon), widget base with Resume and Exit, rr.Debug.PauseMenu"
parent: task-424-pause-menu-stats
kind: task
status: in-progress
---
# Pause menu: miner stats screen with Resume and Exit: cpp

- goal: C++ pause menu logic per pause-menu: toggle on Escape, P and gamepad Start with the blocked states, a stats snapshot for the widget, Resume and Exit, and a debug command for the capture.
- agent: cpp
- depends_on: none
- inputs: task-424-pause-menu-stats (vault/reference/tasks/task-424-pause-menu-stats.md) (request, Questions Q1 to Q3, Q7), pause-menu (vault/reference/gdd/run/pause-menu.md), player-stats (vault/reference/gdd/player/player-stats.md), sys-gas-core (vault/reference/tech/systems/core/sys-gas-core.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), main-menu (vault/reference/gdd/run/main-menu.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: In `Source/RogueRigs/UI/` (or the folder sys-hud and sys-main-menu use): a `URRPauseMenuWidget` UUserWidget base exposing BlueprintCallable/BlueprintPure a stats snapshot struct (global rows: label and formatted value for the six miner stats and five global weapon bonuses; per-weapon blocks: weapon name and its rows), `Resume()` and `ExitToMainMenu()` (reusing the run-won Main Menu path), and an open event for the Blueprint; pause toggle bound in C++ on the player controller (Escape, P, gamepad Start, works while paused, no new input .uasset), refused while another screen pauses the game; widget class loaded from `/Game/RogueRigs/UI/WBP_PauseMenu` the way sys-main-menu loads its widget, a missing asset logged, not a crash; global bonus sum tracked if not already available; console command `rr.Debug.PauseMenu open|resume|exit` in its own debug .cpp calling the same functions; header comments; change summary for the doc subtask (no vault doc node written).
- done_when: Editor target builds without warnings; the existing automation tests of the touched families (menu, run flow, level-up, player) pass or fail exactly as before; no new automated test; change summary lists files, public API, struct fields and the debug command; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-03T06:08:48Z
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
