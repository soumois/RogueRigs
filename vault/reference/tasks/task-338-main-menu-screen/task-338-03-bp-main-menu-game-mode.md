---
id: task-338-03-bp-main-menu-game-mode
description: "BP_MainMenuGameMode, child of the C++ main menu game mode, with WBP_MainMenu assigned."
parent: task-338-main-menu-screen
kind: task
status: done
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
- model: sonnet
- tokens: input 20, cache write 22906, cache read 184887, output 4406
- usage: turns 10, context start 9k peak 27k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M3
- started: 2026-10-28T02:08:26Z
- waiting_for: none
- result: BP_MainMenuGameMode created in Blueprints/Player, MainMenuWidgetClass=WBP_MainMenu, no pawn, C++ menu controller kept; commit d06ee817

## Relations
- [[task-338-main-menu-screen]] — parent global task.
