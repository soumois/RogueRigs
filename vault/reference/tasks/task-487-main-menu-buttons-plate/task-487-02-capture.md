---
id: task-487-02-capture
description: "Capture: -game capture of the main menu at rest and hovered at the four resolutions, compared with the task-485 capture."
parent: task-487-main-menu-buttons-plate
kind: task
status: done
---
# Main menu buttons that fit the screen (plate buttons): capture

- goal: -game capture of the main menu at rest and hovered at the four resolutions, compared with the task-485 capture.
- agent: capture
- depends_on: task-487-01-menu-plates
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Captures/task485_b1080/c_mm.png
- expected_output: see done_when
- done_when: Checklist, each seen on a frame at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (or closed on the owner "je valide"): C1 the three buttons are cream plates of one size, labels readable, no bare text or grey engine button; C2 New Run shows the orange focus outline at rest; C3 New Run hovered is brighter and slightly larger; C4 Options greyed, unchanged under the mouse; C5 title, background and gradient unchanged from the task-485 capture, nothing overlaps or is clipped. Sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-487
- usage: included in the executor run of task-487
- runs: 1
- milestone: M2
- started: 2026-11-07T04:49:38Z
- waiting_for: none
- result: Done. -game L_MainMenu, 4 launches (1920x1080 = Saved/Captures/task487_try2, task487_1280x720, task487_1920x1200, task487_2560x1080; frames checked at their real size). Sheets: Saved/Captures/task487_sheets/rest_sheet.png (task-485 before + rest at 4 res), hover_sheet.png (New Run hovered, Options hovered, 4 res). C1 seen (three cream plates of one width, dark brown labels readable, Options greyed but legible). C2 seen (orange outline on New Run at rest, all res). C3 seen (New Run hovered brighter and slightly larger). C4 seen (Options under the mouse unchanged; focus outline stays on the last hovered Exit, as designed). C5 seen (background, gradient and title look unchanged; nothing overlaps or is clipped); note: the column is taller so the vertically centred title sits about 25 px (1080p) higher than in task-485.

## Relations
- [[task-487-main-menu-buttons-plate]] — parent global task.
