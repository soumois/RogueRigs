---
id: task-379-03-new-run-loading-capture
description: "Capture: Rendered -game check from the main menu: loading screen over the menu, in the arena, then level 1 with the HUD"
parent: task-379-new-run-loading-screen
kind: task
status: todo
---
# Main menu New Run: show the loading screen: capture

- goal: Rendered -game run started on the configured start-up map (capture.js --map none, the main menu), New Run through rr.Menu.NewRun; report each checklist point seen, not seen or ambiguous with the frame path.
- agent: capture
- depends_on: task-379-01-new-run-loading-code, task-379-02-menu-game-mode-loading-class
- inputs: agent-capture (vault/reference/agents/agent-capture.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), Look before: `Saved/Captures/MainMenu/MainMenu_00.png` (menu, task-338-05) and `Saved/PreloadNextLevel/t318_loading_00.png` (WBP_Loading between levels, task-318-04), Debug commands: `rr.Menu.NewRun`, `rr.Debug.Shots`, `rr.Run.Status`
- expected_output: Frames and run log under Saved/NewRunLoading/, a report per point C1 to C4.
- done_when: C1 after New Run the next drawn frame shows WBP_Loading (dark backdrop, "DROPPING IN", orange "Level 1"), not the frozen menu; C2 the loading screen is drawn in the arena world (log: map loaded, phase loading) with no HUD and no arena visible behind it; C3 after level ready, level 1 is visible with the HUD (log: level ready, fade in); C4 no frame between the menu and level 1 shows the arena without the loading screen or the black fade. Each point reported seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
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
- [[task-379-new-run-loading-screen]] — parent global task.
