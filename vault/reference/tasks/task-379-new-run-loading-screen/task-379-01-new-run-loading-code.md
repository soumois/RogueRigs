---
id: task-379-01-new-run-loading-code
description: "Cpp: New Run goes through the transition Loading phase: loading screen in the menu world before OpenLevel, hold and loading screen in the arena until level 1 is ready, fade in"
parent: task-379-new-run-loading-screen
kind: task
status: todo
---
# Main menu New Run: show the loading screen: cpp

- goal: URRMainMenuWidget::StartNewRunInWorld (button and rr.Menu.NewRun) calls a new URRRunTransitionSubsystem entry point that resets the run, enters phase Loading for level 1, broadcasts OnLoadingStarted(1, level 1 layout name), waits for the loading screen paint (existing paint wait) and opens NewRunMapName; the existing PostLoadMapWithWorld hold, OnLevelReady and map-travel fade in (Q1, 0.5 s) then run in the arena world as for a level change. ARRMainMenuGameMode gets an EditDefaultsOnly TSubclassOf<URRLoadingScreenWidget> LoadingScreenWidgetClass (category like MainMenuWidgetClass); ARRMainMenuPlayerController shows it on OnLoadingStarted (above the menu, keyboard focus no longer on New Run) and logs when the class is unset (travel then falls back after LoadingPaintWaitMaxFrames). ARRPlayerController re-shows the loading screen in the arena with the level 1 layout name (not the empty intermission recap). Progress stays 0 on this path (Q2). The arena run starts as today: BeginRun level 1/4 on arena_m1_fine, ore bank 0, a second press ignored. PIE or -game started directly on L_ArenaM1 and the between-level transitions are unchanged. Update only existing test expectations the change breaks; no new test. Return a change summary (public API, properties, log lines, commit hash, changed paths); do not edit doc nodes.
- agent: cpp
- depends_on: none
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), agent-cpp (vault/reference/agents/agent-cpp.md), `Source/RogueRigs/Menu/RRMainMenuWidget.cpp` (StartNewRunInWorld, lines 34-54), `Source/RogueRigs/Menu/RRMainMenuGameMode.h`, `RRMainMenuGameMode.cpp`, `RRMainMenuPlayerController.h`, `RRMainMenuPlayerController.cpp`, `Source/RogueRigs/Run/RRRunTransitionSubsystem.h` and `.cpp` (IsLevelLoading, OpenArenaMap, PostLoadMapWithWorld callback, paint wait), `Source/RogueRigs/Player/RRPlayerController.cpp` (BeginPlay re-show while IsLevelLoading, ShowLoadingScreen), Global request and gdd_check of task-379-new-run-loading-screen for the decided values (Q1, Q2)
- expected_output: C++ changes in Source/RogueRigs/Menu/, Run/ and Player/ (and the test files the change breaks), built with the editor closed, one commit `task-379-01-new-run-loading-code: <summary>`; change summary returned.
- done_when: Build succeeds; a -game -nullrhi run with no map argument and -ExecCmds=rr.Menu.NewRun logs, in this order: main menu shown, run reset, loading started for level 1 in the menu world, the travel waiting for the paint or the no-paint fallback, opening L_ArenaM1, run started level 1/4 on arena_m1_fine, the arena hold (PreparingLevel, spawning held), level ready, HUD restored; a -game -nullrhi run directly on L_ArenaM1 starts level 1 with no loading phase; rr.Run.NextLevel from level 1 still gives the existing transition; RogueRigs.* automation tests that cover the run, transition and menu pass.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
