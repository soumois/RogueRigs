---
id: task-424-05-pause-menu-widget
description: "Bp: WBP_PauseMenu on the C++ base, after the validated mockup, Root Canvas and anchors; widget class wired"
parent: task-424-pause-menu-stats
kind: task
status: wait-answers
---
# Pause menu: miner stats screen with Resume and Exit: bp

- goal: Build WBP_PauseMenu in Content/RogueRigs/UI/ (parent URRPauseMenuWidget) after the validated mockup with the sprites of the element table, per [[umg-layout]] and Q7: title, global block, one block per owned weapon, Resume and Exit buttons bound to the C++ functions; set the widget class property where task-424-03 put it.
- agent: bp
- depends_on: task-424-03-pause-menu-cpp, task-424-04-pause-menu-sprites-import
- inputs: vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md (written by task-424-01: validated mockup, element table), pause-menu (vault/reference/gdd/run/pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), agent-bp (vault/reference/agents/agent-bp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: WBP_PauseMenu.uasset and the wired class default, compiled with no warning; a change summary with anchors, alignment, offsets and size mode of each top-level widget for the doc subtask; commit by path.
- done_when: WBP_PauseMenu compiles with no warning, uses only Canvas and anchors at the root, binds Resume and Exit to the C++ base, and opens through rr.Debug.PauseMenu open in PIE; committed by path.
- editor_state: Unreal open with the MCP plugin
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: owner launches Unreal Editor with the MCP plugin (bp subtask)
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
