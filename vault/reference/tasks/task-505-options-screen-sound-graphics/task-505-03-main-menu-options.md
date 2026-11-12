---
id: task-505-03-main-menu-options
description: "Bp: Main menu Options button opens the options screen."
parent: task-505-options-screen-sound-graphics
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-12T04:15:45Z
- waiting_for: none
- result: open

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
