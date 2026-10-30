---
id: task-390-03-doc-menu-return
description: "Doc: update the run flow, main menu, run and end screen docs with the return to the menu"
parent: task-390-run-won-back-to-main-menu
kind: task
status: in-progress
---
# Run won: back to the main menu: doc

- goal: Record the return-to-menu entry, setting, debug command, the end screen button branch and the log verification.
- agent: doc
- depends_on: task-390-01-return-to-menu-code, task-390-02-end-screen-menu-button
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-run (vault/reference/tech/systems/world/sys-run.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), global request and gdd_check: vault/reference/tasks/task-390-run-won-back-to-main-menu.md
- expected_output: The listed doc nodes updated; commit `task-390-03-doc-menu-return: ...`.
- done_when: Entry, setting, debug command and button branch stated in the nodes; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T21:57:17Z
- waiting_for: none
- result: open

## Relations
- [[task-390-run-won-back-to-main-menu]] — parent global task.
