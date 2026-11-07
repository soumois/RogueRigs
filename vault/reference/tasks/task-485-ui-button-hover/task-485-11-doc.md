---
id: task-485-11-doc
description: "Doc: Doc nodes of the eight changed widgets and of any shared mechanism updated."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: doc

- goal: Doc nodes of the eight changed widgets and of any shared mechanism updated.
- agent: doc
- depends_on: task-485-10-capture
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: Each of the eight wbp-* nodes describes its button hover and what was verified; a shared hover mechanism, if one exists, has its doc node (bp or sys) listed in its index and linked from ui-button-hover; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T03:04:01Z
- waiting_for: none
- result: Done (1c087c53). New sys-button-hover (systems-ui-index child, linked from ui-button-hover): subsystem, M_UI_ButtonPlate, settings, attach calls, debug commands. Eight wbp nodes (main-menu, pause-menu, run-lost, intermission, shop-screen, shop-card, upgrade-choice, alert-objectives) got a button_hover line with commit, mechanism and verified captures, and a relation to sys-button-hover. Lint clean.

## Relations
- [[task-485-ui-button-hover]] — parent global task.
