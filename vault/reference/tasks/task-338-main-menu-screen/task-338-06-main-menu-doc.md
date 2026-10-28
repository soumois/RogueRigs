---
id: task-338-06-main-menu-doc
description: "System and Blueprint doc nodes of the main menu: C++ classes, WBP_MainMenu, BP_MainMenuGameMode, L_MainMenu, rr.Menu commands."
parent: task-338-main-menu-screen
kind: task
status: done
---
# Main menu: documentation

- goal: System and Blueprint doc nodes of the main menu: C++ classes, WBP_MainMenu, BP_MainMenuGameMode, L_MainMenu, rr.Menu commands.
- agent: doc
- depends_on: task-338-01-main-menu-screen, task-338-02-wbp-main-menu, task-338-03-bp-main-menu-game-mode, task-338-04-main-menu-map, task-338-05-main-menu-capture
- inputs: create sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md, template vault/templates/system.md: game mode, controller, widget base, map, GameDefaultMap), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md, template vault/templates/blueprint.md), bp-main-menu-game-mode (vault/reference/tech/blueprints/player/bp-main-menu-game-mode.md, template vault/templates/blueprint.md); update systems-ui-index (vault/reference/tech/systems/ui/systems-ui-index.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md), blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md, rr.Menu.NewRun and rr.Menu.Exit), sys-run (vault/reference/tech/systems/world/sys-run.md, the "no menu" start line); decided values: the global request and gdd_check and main-menu (vault/reference/gdd/run/main-menu.md)
- expected_output: the three new doc nodes and the five updates, with the verified line from task-338-05; lint clean; committed by path.
- done_when: nodes exist and are linked both ways; lint clean; committed
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 18, cache write 41222, cache read 247339, output 9102
- usage: turns 9, context start 11k peak 45k, resumes 0, 1 min, units 0.12 M
- runs: 1
- milestone: M3
- started: 2026-10-28T02:26:08Z
- waiting_for: none
- result: Created sys-main-menu, wbp-main-menu, bp-main-menu-game-mode; updated 3 indexes, sys-cheats, sys-run; lint ok; commit 08114cac

## Relations
- [[task-338-main-menu-screen]] — parent global task.
