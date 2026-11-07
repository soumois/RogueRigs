---
id: task-484-04-death-widget
description: "Bp: New death-screen widget from the validated mockup, with the appearance animation and the two buttons wired to C++."
parent: task-484-death-screen
kind: task
status: todo
---
# Death screen with Retry and Main Menu: bp

- goal: New death-screen widget from the validated mockup, with the appearance animation and the two buttons wired to C++.
- agent: bp
- depends_on: task-484-03-death-flow
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md)
- expected_output: see done_when
- done_when: The widget has a Canvas root with anchors (umg-layout), shows the texts and recap from C++, plays the appearance of death-screen with skip on input, focus on Retry, cursor shown, Retry and Main Menu call the C++ actions; compiled with warnings as errors, saved, PIE death with no script error.
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
- [[task-484-death-screen]] — parent global task.
