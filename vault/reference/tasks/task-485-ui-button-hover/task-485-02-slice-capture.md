---
id: task-485-02-slice-capture
description: "Capture: Check the pause menu slice: hovered and not hovered frames at 1920 x 1080, sent to the owner."
parent: task-485-ui-button-hover
kind: task
status: in-progress
---
# Hover effect on every button of every screen: capture

- goal: Check the pause menu slice: hovered and not hovered frames at 1920 x 1080, sent to the owner.
- agent: capture
- depends_on: task-485-01-pause-slice
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md)
- expected_output: see done_when
- done_when: Sheet sent to the owner with: C1 pause menu at rest; C2 Resume hovered, plate visibly brighter and larger; C3 Exit hovered, same; C4 nothing else moved. The owner's "je valide" on screen replaces this capture.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T02:30:15Z
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
