---
id: task-379-04-new-run-loading-capture
description: "Capture: Rendered -game check from the main menu: loading screen with the animation over the menu, in the arena, then level 1 with the HUD"
parent: task-379-new-run-loading-screen
kind: task
status: done
---
# Main menu New Run: show the loading screen: capture

- goal: Rendered -game run started on the configured start-up map (capture.js --map none, the main menu), New Run through rr.Menu.NewRun; report each checklist point seen, not seen or ambiguous with the frame path.
- agent: capture
- depends_on: task-379-01-new-run-loading-code, task-379-02-menu-game-mode-loading-class, task-379-03-loading-animation-widget
- inputs: agent-capture (vault/reference/agents/agent-capture.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), Look before: `Saved/Captures/MainMenu/MainMenu_00.png` (menu, task-338-05) and `Saved/PreloadNextLevel/t318_loading_00.png` (WBP_Loading between levels, task-318-04), Debug commands: `rr.Menu.NewRun`, `rr.Debug.Shots`, `rr.Run.Status`
- expected_output: Frames and run log under Saved/NewRunLoading/, a report per point C1 to C5.
- done_when: C1 after New Run the next drawn frame shows WBP_Loading (dark backdrop, "DROPPING IN", orange "Level 1"), not the frozen menu; C2 the loading screen is drawn in the arena world (log: map loaded, phase loading) with no HUD and no arena visible behind it; C3 after level ready, level 1 is visible with the HUD (log: level ready, fade in); C4 no frame between the menu and level 1 shows the arena without the loading screen or the black fade; C5 on the loading screen the progress bar is not drawn and the Q4 image is drawn at different angles (or scales) on two frames of the arena-side hold. Each point reported seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 20, cache write 38184, cache read 239145, output 4603
- usage: turns 10, context start 14k peak 38k, resumes 1 (message x1), 7 min, units 0.09 M
- runs: 0
- milestone: M2
- started: 2026-10-30T04:22:55Z
- waiting_for: none
- result: Closed on owner word 2026-10-30 (pas besoin de capture): no capture run. Owner sees the spinning crystal and says it takes a while to start; lag cause unknown, unverified.

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
