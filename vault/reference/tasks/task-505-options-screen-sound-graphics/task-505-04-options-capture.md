---
id: task-505-04-options-capture
description: "Capture: Capture the options screen from the main menu."
parent: task-505-options-screen-sound-graphics
kind: task
status: todo
---
# Options screen with Sound and Graphics, opened from the main menu: capture

- goal: Capture the options screen from the main menu.
- agent: capture
- depends_on: task-505-03-main-menu-options
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: Frames at the four resolutions show: C1 main menu Options plate active, not greyed; C2 Sound tab with three sliders and values; C3 Graphics tab with every option, nothing clipped or overlapping; C4 the look of the validated mockup, side by side; C5 a changed value (preset Low) still shown after a restart; sheet sent to the owner.
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
