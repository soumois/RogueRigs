---
id: task-390-02-end-screen-menu-button
description: "Bp: WBP_AlertObjectives: run-won end screen button "Main Menu" calls the return-to-menu entry"
parent: task-390-run-won-back-to-main-menu
kind: task
status: wait-answers
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T21:42:28Z
- waiting_for: bp subtask 02 blocked: mcp__unreal tool not exposed to the bp agent; HTTP JSON-RPC route denied by permission classifier
- result: open

## Relations
- [[task-390-run-won-back-to-main-menu]] — parent global task.
