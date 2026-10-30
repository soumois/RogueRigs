---
id: task-379-02-menu-game-mode-loading-class
description: "Bp: BP_MainMenuGameMode: set LoadingScreenWidgetClass to WBP_Loading"
parent: task-379-new-run-loading-screen
kind: task
status: in-progress
---
# Main menu New Run: show the loading screen: bp

- goal: In BP_MainMenuGameMode, set the new LoadingScreenWidgetClass default to WBP_Loading_C; nothing else changes. Compile with warnings as errors, save, read the value back. Return a change summary (property and value, commit hash); do not edit doc nodes.
- agent: bp
- depends_on: task-379-01-new-run-loading-code
- inputs: bp-main-menu-game-mode (vault/reference/tech/blueprints/player/bp-main-menu-game-mode.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), agent-bp (vault/reference/agents/agent-bp.md), Change summary of task-379-01-new-run-loading-code (property name and category)
- expected_output: /Game/RogueRigs/Blueprints/Player/BP_MainMenuGameMode.uasset saved, one commit `task-379-02-menu-game-mode-loading-class: <summary>`.
- done_when: The property reads back WBP_Loading_C; the Blueprint compiles with no warning; MainMenuWidgetClass still WBP_MainMenu_C; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T01:47:57Z
- waiting_for: none
- result: open

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
