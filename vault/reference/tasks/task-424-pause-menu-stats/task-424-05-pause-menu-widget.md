---
id: task-424-05-pause-menu-widget
description: "Bp: WBP_PauseMenu on the C++ base, built after the validated mockup on a Root Canvas with anchors"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: bp

- goal: Build WBP_PauseMenu after the validated mockup, parented to URRPauseMenuWidget, Canvas and anchors.
- agent: bp
- depends_on: task-424-03-pause-menu-cpp, task-424-04-pause-menu-sprites-import
- inputs: task-424-pause-menu-stats (vault/reference/tasks/task-424-pause-menu-stats.md) (Questions Q3, Q8), pause-menu (vault/reference/gdd/run/pause-menu.md), `vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md` (new, written by subtask 01) (validated mockup, Elements section), umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md) (brushes, tints and layout of the existing stats screen), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (T_Shop_TitleBar, T_Shop_ButtonContinue settings), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md) (button font and focus), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), agent-bp (vault/reference/agents/agent-bp.md), change summary of task-424-03 (from the Executor)
- expected_output: `Content/RogueRigs/UI/WBP_PauseMenu.uasset`, parent URRPauseMenuWidget, Root Canvas per umg-layout: semi-transparent black backdrop, title PAUSED, global stats block and one block per weapon filled from the C++ snapshot on open, Resume and Exit buttons calling the C++ functions, brushes per the Elements mapping; widget names clear; change summary with the tree and anchors per top-level widget (no vault doc node written).
- done_when: Compiled with warnings as errors and saved; graphs only bind the C++ snapshot and call Resume and ExitToMainMenu (no gameplay logic in Blueprint); the widget opens from rr.Debug.PauseMenu open in a -game or PIE run without errors in the log; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
