---
id: task-487-03-doc
description: "Doc: Doc nodes of the main menu widget and of the hover mechanism updated."
parent: task-487-main-menu-buttons-plate
kind: task
status: done
---
# Main menu buttons that fit the screen (plate buttons): doc

- goal: Doc nodes of the main menu widget and of the hover mechanism updated.
- agent: doc
- depends_on: task-487-02-capture
- inputs: wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md)
- expected_output: see done_when
- done_when: wbp-main-menu describes the plate buttons, focus outline, anchors per umg-layout and the hover; sys-button-hover lists the main menu buttons as plate buttons; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-487
- usage: included in the executor run of task-487
- runs: 1
- milestone: M2
- started: 2026-11-07T04:51:52Z
- waiting_for: none
- result: Done. wbp-main-menu: components table with anchors and slots, plate style, labels, focus outline images, RefreshColours visibility toggle, hover, task-487 verification; sys-button-hover: main menu NewRun/Exit listed as plate buttons (brightening and growth), relation to wbp-main-menu. Lint clean.

## Relations
- [[task-487-main-menu-buttons-plate]] — parent global task.
