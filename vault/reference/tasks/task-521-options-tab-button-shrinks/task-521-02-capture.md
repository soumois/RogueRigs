---
id: task-521-02-capture
description: "Capture: Options tabs after clicks, per Q3"
parent: task-521-options-tab-button-shrinks
kind: task
status: done
---
# Options : l'onglet cliqué devient plus petit: capture

- goal: Options tabs after clicks, per Q3
- agent: capture
- depends_on: task-521-01-tab-size-fix
- inputs: agent-capture (vault/reference/agents/agent-capture.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), options-screen (vault/reference/gdd/run/options-screen.md)
- expected_output: see done_when
- done_when: C1 each tab clicked in turn: the three tabs the same size; C2 Reset to defaults and Back the same size before and after a click; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-521
- usage: included in the executor run of task-521
- runs: 1
- milestone: M2
- started: 2026-11-13T13:33:05Z
- waiting_for: none
- result: 4 launches (1920x1080, 1280x720, 1920x1200, 2560x1080), each opening Options on Sound and clicking the tabs with rr.Debug.CursorOn + rr.Debug.Click, cursor moved away before each shot. C1 seen: after clicking Graphics, Gameplay, then Sound again, the three tabs have the same drawn size at every resolution, the cream plate fills the selected tab like the dark plates fill the others (owner image 2: Sound tab drawn smaller, now gone). C2 seen: Reset to defaults and Back the same size before and after a click (Back clicked closes the screen, reopened: same size). Hover on a tab: no growth by design, tint brightening not visible on the frame (unchanged behaviour). Frames: Saved/Captures/task521_1080/tabs.png (tab crops), Saved/Captures/task521_1080/contact_sheet.png, Saved/Captures/task521_resolutions.png. Owner GameUserSettings.ini restored after the forced resolutions.

## Relations
- [[task-521-options-tab-button-shrinks]] — parent global task.
