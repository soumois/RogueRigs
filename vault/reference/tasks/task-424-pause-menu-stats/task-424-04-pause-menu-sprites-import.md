---
id: task-424-04-pause-menu-sprites-import
description: "Bp: import the new T_Pause_* PNGs as UI textures in /Game/RogueRigs/UI/PauseMenu"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: bp

- goal: Import the new pause menu sprites as UI textures.
- agent: bp
- depends_on: task-424-02-pause-menu-elements
- inputs: `vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md` (new, written by subtask 01) (Elements section), task-381-01-level-end-sprites-import (vault/reference/tasks/task-381-level-end-screen-rebuild/task-381-01-level-end-sprites-import.md) (texture settings used), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), agent-bp (vault/reference/agents/agent-bp.md)
- expected_output: One UI texture per new PNG of `Art/UI/PauseMenu/` in `/Game/RogueRigs/UI/PauseMenu/`, settings as task-381-01 (UI group, no mips, sizes matching the PNGs); change summary. When subtask 02 made no new PNG, nothing is imported and the result says so.
- done_when: Each new PNG has its texture with matching size and UI settings, saved; or the result states that no new sprite exists; committed by path.
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
