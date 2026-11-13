---
id: task-540-03-doc
description: "Doc: Update the menu widget doc for the removed focus outline"
parent: task-540-main-menu-no-focus-outline
kind: task
status: done
---
# Main menu: no orange focus outline around New Run: doc

- goal: Update the menu widget doc for the removed focus outline
- agent: doc
- depends_on: task-540-02-capture
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md)
- expected_output: see done_when
- done_when: The bp/sys node of the main menu no longer describes the outline; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-540
- usage: included in the executor run of task-540
- runs: 1
- milestone: M2
- started: 2026-11-13T23:06:41Z
- waiting_for: none
- result: Done (ac5bf31a): wbp-main-menu (focus look none, RefreshColours and EventTick gone, task-540 verification), sys-main-menu (RenderFocusRule=Never config line, Enter now seen), sys-button-hover (no focus look) updated; stale comment in RRMainMenuWidget.cpp fixed (comment only, no build). Lint clean.

## Relations
- [[task-540-main-menu-no-focus-outline]] — parent global task.
