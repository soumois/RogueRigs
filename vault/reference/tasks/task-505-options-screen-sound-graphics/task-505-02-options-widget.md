---
id: task-505-02-options-widget
description: "Bp: Options screen widget with Sound and Graphics tabs, built from the task-504 elements."
parent: task-505-options-screen-sound-graphics
kind: task
status: done
---
# Options screen with Sound and Graphics, opened from the main menu: bp

- goal: Options screen widget with Sound and Graphics tabs, built from the task-504 elements.
- agent: bp
- depends_on: task-505-01-settings-sound-graphics
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md)
- expected_output: see done_when
- done_when: A2 and the widget side of A3 and A4 hold: every option shows its value and changes it through C++, tabs switch, Reset to defaults and Back work.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-505
- usage: included in the executor run of task-505
- runs: 1
- milestone: M2
- started: 2026-11-11T22:10:34Z
- waiting_for: none
- result: done (868a1718): WBP_Options on URROptionsWidget; C++ builds the 14 rows (selectors, sliders, VSync toggle) into SoundList/GraphicsList, sizes controls from the row height; tabs, Reset (current tab only), Back, Escape, keep-or-revert panel with countdown checked in -game (Saved/Captures/task505_02_try2, try3_*, task505_02_check); labels in ScaleBox down-only; Gameplay tab shown disabled until task-506; rr.Options debug command

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
