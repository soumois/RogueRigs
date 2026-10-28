---
id: task-338-05-main-menu-capture
description: "-game capture of the main menu at 1920 x 1080 and log check that New Run reaches level 1 and Exit quits."
parent: task-338-main-menu-screen
kind: task
status: todo
---
# Main menu: capture and check

- goal: -game capture of the main menu at 1920 x 1080 and log check that New Run reaches level 1 and Exit quits.
- agent: cpp
- depends_on: task-338-04-main-menu-map
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md, capture_script and visual_check), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run (vault/reference/tech/systems/world/sys-run.md), main-menu (vault/reference/gdd/run/main-menu.md); the change summaries of task-338-01 to task-338-04
- expected_output: a capture through Tools/capture/capture.js of the menu on a default -game launch (no map argument) at 1920 x 1080, PNG and contact sheet in Saved/Captures/MainMenu/, described in the report; a second run where `rr.Menu.NewRun` reaches level 1 of a fresh run on L_ArenaM1 (log lines quoted), and `rr.Menu.Exit` closes the game; check that the capture and stress launch commands that pass L_ArenaM1 still open the arena (fix only what the new GameDefaultMap breaks); what was not seen (a real mouse click) listed.
- done_when: menu PNG exists and is described; New Run and Exit log lines quoted; existing launch commands still reach the arena; report lists the unseen items
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-338-main-menu-screen]] — parent global task.
