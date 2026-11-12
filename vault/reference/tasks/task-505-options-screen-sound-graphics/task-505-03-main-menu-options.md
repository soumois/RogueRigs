---
id: task-505-03-main-menu-options
description: "Bp: Main menu Options button opens the options screen."
parent: task-505-options-screen-sound-graphics
kind: task
status: done
---
# Options screen with Sound and Graphics, opened from the main menu: bp

- goal: Main menu Options button opens the options screen.
- agent: bp
- depends_on: task-505-02-options-widget
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), options-menu (vault/reference/gdd/run/options-menu.md)
- expected_output: see done_when
- done_when: A1 of the global holds.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-505
- usage: included in the executor run of task-505
- runs: 1
- milestone: M2
- started: 2026-11-12T04:15:45Z
- waiting_for: none
- result: Done (4c45d661). WBP_MainMenu: OptionsButton enabled and focusable, OptionsFocus outline (as NewRunFocus) toggled in RefreshColours. C++ URRMainMenuWidget: OptionsWidgetClass WBP_Options, OpenOptions on click, plate hover attached, hover gives focus (C++: MCP cannot add the bound OnHovered event), focus back on Options on close. Checked in -game 1920x1080 (Saved/Captures/task505_03_check/contact_sheet.png): plate active, screen opens, Back returns with outline on Options. Not seen: mouse hover brightening, Escape by key (rr.Options back used).

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
