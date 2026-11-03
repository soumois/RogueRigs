---
id: task-424-04-pause-menu-sprites-import
description: "Bp: import the new T_Pause_* PNGs as UI textures in /Game/RogueRigs/UI/PauseMenu"
parent: task-424-pause-menu-stats
kind: task
status: in-progress
---
# Pause menu: miner stats screen with Resume and Exit: bp

- goal: Import every new Art/UI/PauseMenu/T_Pause_*.png listed in the element table as a UI texture (no mips, UI group, sRGB) in /Game/RogueRigs/UI/PauseMenu; nothing to import when every element is reused (record it).
- agent: bp
- depends_on: task-424-02-pause-menu-elements
- inputs: vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md (written by task-424-01: validated mockup, element table), agent-bp (vault/reference/agents/agent-bp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: Texture .uasset files in Content/RogueRigs/UI/PauseMenu/, a change summary for the doc subtask, commit by path.
- done_when: Each new PNG of the element table has its texture asset with the UI settings; committed by path.
- editor_state: Unreal open with the MCP plugin
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-03T06:55:24Z
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
