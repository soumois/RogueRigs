---
id: task-505-02-options-widget
description: "Bp: Options screen widget with Sound and Graphics tabs, built from the task-504 elements."
parent: task-505-options-screen-sound-graphics
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
