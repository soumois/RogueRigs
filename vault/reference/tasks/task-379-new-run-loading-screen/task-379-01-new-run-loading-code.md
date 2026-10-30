---
id: task-379-01-new-run-loading-code
description: "Cpp: New Run goes through the transition Loading phase (loading screen in the menu world, hold in the arena, fade in); loading widget tells Blueprint whether progress is measured"
parent: task-379-new-run-loading-screen
kind: task
status: done
---
# Main menu New Run: show the loading screen: cpp

- goal: URRMainMenuWidget::StartNewRunInWorld (button and rr.Menu.NewRun) calls a new URRRunTransitionSubsystem entry point that resets the run, enters phase Loading for level 1, broadcasts OnLoadingStarted(1, level 1 layout name), waits for the loading screen paint (existing paint wait) and opens NewRunMapName; the existing PostLoadMapWithWorld hold, OnLevelReady and map-travel fade in (Q1, 0.5 s) then run in the arena world as for a level change. ARRMainMenuGameMode gets an EditDefaultsOnly TSubclassOf<URRLoadingScreenWidget> LoadingScreenWidgetClass (same category as MainMenuWidgetClass); ARRMainMenuPlayerController shows it on OnLoadingStarted (above the menu, the menu no longer takes input) and logs when the class is unset (travel then falls back after LoadingPaintWaitMaxFrames). ARRPlayerController re-shows the loading screen in the arena with the level 1 layout name. URRLoadingScreenWidget gets a BlueprintPure IsLoadingProgressMeasured() (true only on the in-world next-level preparation path; false on New Run and the map-travel fallback), valid when OnLoadingShown fires, plus a subsystem pure of the same meaning. The run starts as today: BeginRun level 1/4 on arena_m1_fine, ore bank 0, a second press ignored. PIE or -game directly on L_ArenaM1 and the between-level transitions are otherwise unchanged. Update only existing test expectations the change breaks; no new test. Return a change summary (public API, properties, log lines, commit hash, changed paths); do not edit doc nodes.
- agent: cpp
- depends_on: none
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), intermission (vault/reference/gdd/run/intermission.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), agent-cpp (vault/reference/agents/agent-cpp.md), `Source/RogueRigs/Menu/RRMainMenuWidget.cpp` (StartNewRunInWorld, lines 34-54), `Source/RogueRigs/Menu/RRMainMenuGameMode.h`, `RRMainMenuGameMode.cpp`, `RRMainMenuPlayerController.h`, `RRMainMenuPlayerController.cpp`, `Source/RogueRigs/Run/RRRunTransitionSubsystem.h` and `.cpp` (IsLevelLoading, OpenArenaMap, PostLoadMapWithWorld callback, paint wait, GetLoadingProgress), `Source/RogueRigs/UI/RRLoadingScreenWidget.h` and `.cpp` (ShowLoading, GetLoadingProgress), `Source/RogueRigs/Player/RRPlayerController.cpp` (BeginPlay re-show while IsLevelLoading, ShowLoadingScreen), Global request and gdd_check of task-379-new-run-loading-screen for the decided values (Q1, Q2 answer, Q5)
- expected_output: C++ changes in Source/RogueRigs/Menu/, Run/, UI/ and Player/ (and the test files the change breaks), built with the editor closed, one commit `task-379-01-new-run-loading-code: <summary>`; change summary returned.
- done_when: Build succeeds; a -game -nullrhi run with no map argument and -ExecCmds=rr.Menu.NewRun logs, in this order: main menu shown, run reset, loading started for level 1 in the menu world (progress not measured), the travel waiting for the paint or the no-paint fallback, opening L_ArenaM1, run started level 1/4 on arena_m1_fine, the arena hold (PreparingLevel, spawning held), level ready, HUD restored; a -game -nullrhi run directly on L_ArenaM1 starts level 1 with no loading phase; rr.Run.NextLevel from level 1 still gives the existing transition and logs whether its progress is measured; RogueRigs.* automation tests that cover the run, transition and menu pass.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 64, cache write 139240, cache read 2938811, output 35525
- usage: turns 32, context start 12k peak 139k, resumes 0, 13 min, units 0.65 M
- runs: 1
- milestone: M2
- started: 2026-10-30T01:34:27Z
- waiting_for: none
- result: Commit e8d430c2. StartNewRun entry point on URRRunTransitionSubsystem, IsLoadingProgressMeasured (subsystem and widget), menu game mode LoadingScreenWidgetClass, menu controller shows it, arena re-show. Build ok; done_when runs logged as expected (menu-world paint not exercised: class unset until 02). Tests 36/38; 2 failures (JuiceKillDropsStreamingCubes, LevelUpUpgradeLoop) unrelated, not checked on prior commit.

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
