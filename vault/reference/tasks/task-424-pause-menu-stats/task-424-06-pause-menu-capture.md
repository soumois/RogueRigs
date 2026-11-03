---
id: task-424-06-pause-menu-capture
description: "Capture: -game captures of the pause menu at four resolutions against the validated mockup, Resume and Exit by debug command"
parent: task-424-pause-menu-stats
kind: task
status: in-progress
---
# Pause menu: miner stats screen with Resume and Exit: capture

- goal: Run -game captures per Q6 with rr.Debug.PauseMenu and compare with the validated mockup.
- agent: capture
- depends_on: task-424-05-pause-menu-widget
- inputs: vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md (written by task-424-01: validated mockup, element table), pause-menu (vault/reference/gdd/run/pause-menu.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: Frames in Saved/Captures/ and a per-point report seen / not seen / ambiguous with frame paths.
- done_when: C1 menu open over the paused, dimmed arena at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080, nothing clipped; C2 title PAUSED and the layout of the validated mockup; C3 global block with the 6 miner stats and the global weapon bonuses; C4 one block per owned weapon (two weapons), locked slots absent; C5 Resume and Exit buttons drawn with their sprites; C6 rr.Debug.PauseMenu resume closes the menu and play resumes; C7 rr.Debug.PauseMenu exit shows L_MainMenu.
- editor_state: Unreal closed (-game run)
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-03T07:06:00Z
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
