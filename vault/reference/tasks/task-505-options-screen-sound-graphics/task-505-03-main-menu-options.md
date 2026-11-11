---
id: task-505-03-main-menu-options
description: "Bp: Main menu Options button opens the options screen."
parent: task-505-options-screen-sound-graphics
kind: task
status: wait-answers
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
- started: 2026-11-11T22:26:09Z
- waiting_for: budget: done 01, 02 (WBP_Options, -game checks); remaining 03 main menu Options (C++ ready uncommitted), 04 capture at four resolutions, 05 doc; estimate about 45 turns and 1.0 M units
- result: open

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
