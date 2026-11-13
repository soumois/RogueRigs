---
id: task-540-02-capture
description: "Capture: Before/after capture of the main menu at 1920 x 1080"
parent: task-540-main-menu-no-focus-outline
kind: task
status: in-progress
---
# Main menu: no orange focus outline around New Run: capture

- goal: Before/after capture of the main menu at 1920 x 1080
- agent: capture
- depends_on: task-540-01-focus-outline-off
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/1.webp
- expected_output: see done_when
- done_when: C1 no orange rectangle around any of the three buttons at open; C2 none after one keyboard down then up; C3 buttons, title and background otherwise unchanged against the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T23:06:24Z
- waiting_for: none
- result: open

## Relations
- [[task-540-main-menu-no-focus-outline]] — parent global task.
