---
id: task-506-04-gameplay-capture
description: "Capture: Capture the pause menu access and the Gameplay tab."
parent: task-506-options-gameplay-and-pause
kind: task
status: todo
---
# Options screen: Gameplay tab and access from the pause menu: capture

- goal: Capture the pause menu access and the Gameplay tab.
- agent: capture
- depends_on: task-506-03-pause-menu-options
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: Frames show: C1 pause menu with Resume, Options, Exit at the four resolutions; C2 Gameplay tab with its toggles at the four resolutions, nothing clipped; C3 hits with damage numbers off show no number; C4 aim indicator off shows no chevrons; sheet sent to the owner.
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
- [[task-506-options-gameplay-and-pause]] — parent global task.
