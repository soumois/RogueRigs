---
id: task-390-03-menu-return-capture
description: "Capture: -game frames: run-won screen with Main Menu, then the main menu, then New Run"
parent: task-390-run-won-back-to-main-menu
kind: task
status: todo
---
# Run won: back to the main menu: capture

- goal: Check on frames that a won run leads back to the main menu.
- agent: capture
- depends_on: task-390-02-end-screen-menu-button
- inputs: agent-capture (vault/reference/agents/agent-capture.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), task-390-01-return-to-menu-code (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-01-return-to-menu-code.md) change summary (debug command), debug commands: rr.Run.NextLevel, rr.Run.Stats.Next, the task-390-01 return-to-menu command, rr.Menu.NewRun, rr.Debug.Shots; launch on the start-up map (capture.js --map none) so the menu exists
- expected_output: Frames under Saved/Captures/task-390/, report per point.
- done_when: C1 run-won end screen (CLAIM STRIPPED) shows a button reading "Main Menu". C2 after it (button or command) the main menu is shown (ROGUE RIGS, New Run, Options, Exit). C3 New Run from there starts level 1 with the HUD ore counter at 0. C4 a run-lost screen still shows "Play again". Every point reported seen, not seen or ambiguous; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-390-run-won-back-to-main-menu]] — parent global task.
