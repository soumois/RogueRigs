---
id: task-338-01-main-menu-screen
description: "C++ main menu game mode, widget base (NewRun, Exit, Options disabled) and rr.Menu debug commands."
parent: task-338-main-menu-screen
kind: task
status: done
---
# Main menu: C++ classes

- goal: C++ main menu game mode, widget base (NewRun, Exit, Options disabled) and rr.Menu debug commands.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), naming (vault/reference/tech/conventions/naming.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), bp-rr-game-mode (vault/reference/tech/blueprints/player/bp-rr-game-mode.md), bp-rr-player-controller (vault/reference/tech/blueprints/player/bp-rr-player-controller.md), main-menu (vault/reference/gdd/run/main-menu.md, decided behaviour)
- expected_output: (owner "ok" 2026-10-28 to Q8 to Q10, see main-menu) a main menu game mode (no pawn, its own player controller: mouse cursor shown, UI-only input) that creates the widget class set in its `MainMenuWidgetClass` property; an Abstract widget base with BlueprintCallable `StartNewRun()` (ResetRun, then open the arena map L_ArenaM1 at level 1) and `QuitGame()`, pure `IsOptionsAvailable()` returning false; debug commands `rr.Menu.NewRun` and `rr.Menu.Exit` calling the same functions; build succeeds; change summary (public API, class paths, commit hash) for the doc subtask, no doc node written.
- done_when: build succeeds with the editor closed; PIE or -game on L_ArenaM1 still starts a run directly (no behaviour change outside the menu map); change summary returned
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 52, cache write 89786, cache read 1480068, output 21225
- usage: turns 26, context start 11k peak 90k, resumes 0, 8 min, units 0.37 M
- runs: 1
- milestone: M3
- started: 2026-10-28T01:53:55Z
- waiting_for: none
- result: C++ classes ARRMainMenuGameMode, ARRMainMenuPlayerController, URRMainMenuWidget, rr.Menu.NewRun/Exit, URRRunSettings::NewRunMapName; build ok, -game on L_ArenaM1 still starts run; commit fdcdae4c

## Relations
- [[task-338-main-menu-screen]] — parent global task.
