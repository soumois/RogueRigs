---
id: task-485-02-slice-capture
description: "Capture: Check the pause menu slice: hovered and not hovered frames at 1920 x 1080, sent to the owner."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: capture

- goal: Check the pause menu slice: hovered and not hovered frames at 1920 x 1080, sent to the owner.
- agent: capture
- depends_on: task-485-01-pause-slice
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md)
- expected_output: see done_when
- done_when: Sheet sent to the owner with: C1 pause menu at rest; C2 Resume hovered, plate visibly brighter and larger; C3 Exit hovered, same; C4 nothing else moved. The owner's "je valide" on screen replaces this capture.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T02:30:15Z
- waiting_for: none
- result: Done. One -game launch at 1920x1080 (L_ArenaM1, rr.Debug.PauseMenu open, rr.Debug.Cursor), frames Saved/Captures/task485_slice/rest_00.png, resume_00.png, exit_00.png; sheet crops.png (button row) and contact_sheet.png. C1 seen: both plates at rest, same look. C2 seen: Resume plate clearly brighter (cream lighter, same hue) and larger, Exit at rest. C3 seen: Exit brighter and larger, Resume back to rest. C4 seen: title, frame, stats and button positions identical in the three frames. Slice passed; sheet listed for the owner in the final report.

## Relations
- [[task-485-ui-button-hover]] — parent global task.
