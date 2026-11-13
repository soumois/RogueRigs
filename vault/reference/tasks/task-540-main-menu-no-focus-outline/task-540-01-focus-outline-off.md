---
id: task-540-01-focus-outline-off
description: "Bp: Remove the orange focus outline of the main menu buttons, focus and Enter kept"
parent: task-540-main-menu-no-focus-outline
kind: task
status: done
---
# Main menu: no orange focus outline around New Run: bp

- goal: Remove the orange focus outline of the main menu buttons, focus and Enter kept
- agent: bp
- depends_on: none
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), Saved/Scratch/owner-ui-batch-2026-11-13/1.webp
- expected_output: see done_when
- done_when: A1 to A3 hold in the widget; focus on New Run at open, Enter starts a run.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-540
- usage: included in the executor run of task-540
- runs: 1
- milestone: M2
- started: 2026-11-13T22:55:53Z
- waiting_for: none
- result: Done (2f08ef00): WBP_MainMenu NewRunFocus/ExitFocus images, RefreshColours function and its EventTick removed (EventConstruct still focuses NewRunButton, OnHovered still focuses the hovered button), compiled with warnings as errors, saved. First keyboard check showed the engine navigation focus rectangle (thin blue) once the orange outline was gone; one focused correction: Config/DefaultEngine.ini [/Script/Engine.UserInterfaceSettings] RenderFocusRule=Never (project-wide: no Slate focus rectangle on any screen). Checked in -game 1920x1080 with real key presses (Down, Up, Enter): no rectangle, Enter on New Run loads L_ArenaM1 (Saved/Captures/task540_enter2). Noted: Down from New Run lands on Exit, not Options (navigation order, pre-existing, not changed).

## Relations
- [[task-540-main-menu-no-focus-outline]] — parent global task.
