---
id: task-379-02-menu-game-mode-loading-class
description: "Bp: BP_MainMenuGameMode: set LoadingScreenWidgetClass to WBP_Loading"
parent: task-379-new-run-loading-screen
kind: task
status: done
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
- model: sonnet
- tokens: input 22, cache write 72002, cache read 605562, output 3179
- usage: turns 10, context start 58k peak 72k, resumes 0, 0 min, units 0.17 M
- runs: 1
- milestone: M2
- started: 2026-10-30T03:09:42Z
- waiting_for: none
- result: LoadingScreenWidgetClass on BP_MainMenuGameMode set to WBP_Loading_C, MainMenuWidgetClass unchanged, compiled and saved, commit 21349fa8

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
