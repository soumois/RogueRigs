---
id: task-338-04-main-menu-map
description: "L_MainMenu map with the game mode override, set as GameDefaultMap."
parent: task-338-main-menu-screen
kind: task
status: in-progress
---
# Main menu: L_MainMenu map

- goal: L_MainMenu map with the game mode override, set as GameDefaultMap.
- agent: bp
- depends_on: task-338-03-bp-main-menu-game-mode
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), naming (vault/reference/tech/conventions/naming.md), main-menu (vault/reference/gdd/run/main-menu.md, decided start-up behaviour); the change summary of task-338-03; Config/DefaultEngine.ini
- expected_output: (owner "ok" 2026-10-28 to Q9, see main-menu) an empty map /Game/RogueRigs/Maps/L_MainMenu (no grid, no level actors) whose World Settings GameMode override is BP_MainMenuGameMode; Config/DefaultEngine.ini GameDefaultMap = L_MainMenu, EditorStartupMap and GlobalDefaultGameMode unchanged (L_ArenaM1, BP_RRGameMode); map and ini saved and committed by path; change summary for the doc subtask.
- done_when: the map opens in the editor with the override set; DefaultEngine.ini shows GameDefaultMap L_MainMenu and the other two lines unchanged; change summary returned
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-28T02:09:24Z
- waiting_for: none
- result: open

## Relations
- [[task-338-main-menu-screen]] — parent global task.
