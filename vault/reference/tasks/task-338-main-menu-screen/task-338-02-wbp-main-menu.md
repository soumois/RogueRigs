---
id: task-338-02-wbp-main-menu
description: "WBP_MainMenu widget with the imported background texture, title and three buttons."
parent: task-338-main-menu-screen
kind: task
status: todo
---
# Main menu: WBP_MainMenu

- goal: WBP_MainMenu widget with the imported background texture, title and three buttons.
- agent: bp
- depends_on: task-338-01-main-menu-screen, task-337-01-main-menu-background (the owner-picked image in vault/assets/)
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), naming (vault/reference/tech/conventions/naming.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), main-menu (vault/reference/gdd/run/main-menu.md, decided layout); the change summary of task-338-01; the picked background image in vault/assets/ (file name in task-337-01 result)
- expected_output: PENDING OWNER ANSWERS (proposed defaults Q6 and Q7): the background imported as a UI texture T_MainMenuBackground under /Game/RogueRigs/UI/; WBP_MainMenu (parent: the C++ widget base of task-338-01) under /Game/RogueRigs/UI/: full-screen image, dark gradient behind a left column with the title "ROGUE RIGS" and the buttons "New Run", "Options", "Exit" (Roboto Bold, white with outline, orange 1/0.35/0.1 on hover and focus, labels hardcoded), New Run calls StartNewRun, Exit calls QuitGame, Options greyed out and disabled, keyboard focus on New Run at construct; no animation; compiled with warnings as errors, saved, committed by path; change summary for the doc subtask.
- done_when: widget compiles with warnings as errors and is saved; tree and graph read back; texture imported; change summary returned
- editor_state: open
- token_ceiling: 1.0 M units
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
