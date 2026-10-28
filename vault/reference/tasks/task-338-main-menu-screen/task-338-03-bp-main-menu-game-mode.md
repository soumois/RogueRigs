---
id: task-338-03-bp-main-menu-game-mode
description: "BP_MainMenuGameMode, child of the C++ main menu game mode, with WBP_MainMenu assigned."
parent: task-338-main-menu-screen
kind: task
status: in-progress
---
# Main menu: BP_MainMenuGameMode

- goal: BP_MainMenuGameMode, child of the C++ main menu game mode, with WBP_MainMenu assigned.
- agent: bp
- depends_on: task-338-01-main-menu-screen, task-338-02-wbp-main-menu
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), naming (vault/reference/tech/conventions/naming.md), bp-rr-game-mode (vault/reference/tech/blueprints/player/bp-rr-game-mode.md); the change summaries of task-338-01 and task-338-02
- expected_output: BP_MainMenuGameMode in /Game/RogueRigs/Blueprints/Player/ (parent: the C++ main menu game mode), MainMenuWidgetClass = WBP_MainMenu, the C++ menu player controller class kept, no pawn; no graph logic; compiled, saved, committed by path; change summary for the doc subtask.
- done_when: Blueprint compiles and is saved; defaults read back (widget class, controller class, no pawn); change summary returned
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-28T02:08:26Z
- waiting_for: none
- result: open

## Relations
- [[task-338-main-menu-screen]] — parent global task.
