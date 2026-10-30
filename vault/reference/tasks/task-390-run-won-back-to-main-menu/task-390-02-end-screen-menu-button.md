---
id: task-390-02-end-screen-menu-button
description: "Bp: WBP_AlertObjectives: run-won end screen button "Main Menu" calls the return-to-menu entry"
parent: task-390-run-won-back-to-main-menu
kind: task
status: done
---
# Run won: back to the main menu: bp

- goal: In WBP_AlertObjectives ShowRunEnd, when bSuccess and IsRunWon: RestartText "Main Menu" and the button click calls the entry of task-390-01; every other case keeps "Play again" and RunFlow RestartLevel. Nothing else in the widget changes.
- agent: bp
- depends_on: task-390-01-return-to-menu-code
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), task-390-01-return-to-menu-code (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-01-return-to-menu-code.md) change summary (function name), global request and gdd_check: vault/reference/tasks/task-390-run-won-back-to-main-menu.md
- expected_output: WBP_AlertObjectives saved, compiled with warnings as errors; commit `task-390-02-end-screen-menu-button: ...`; change summary of the graph change for the doc subtask (no doc node written).
- done_when: Graph read back: run-won branch sets "Main Menu" and calls the new entry, other branches unchanged; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 94, cache write 132865, cache read 1687943, output 25895
- usage: turns 47, context start 11k peak 66k, resumes 0, 5 min, units 0.46 M
- runs: 1
- milestone: M2
- started: 2026-10-30T21:52:48Z
- waiting_for: none
- result: WBP_AlertObjectives saved and compiled (warnings as errors, clean); bReturnToMenu, Main Menu label, click branch; commit 99d91848. Not run in PIE.

## Relations
- [[task-390-run-won-back-to-main-menu]] — parent global task.
