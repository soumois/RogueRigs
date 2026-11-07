---
id: task-486-04-doc
description: "Doc: Doc nodes of the two changed widgets and of the hover mechanism updated."
parent: task-486-grey-buttons-plate
kind: task
status: done
---
# Cream plate look for the two grey buttons (level-up Reroll, run-won Main Menu): doc

- goal: Doc nodes of the two changed widgets and of the hover mechanism updated.
- agent: doc
- depends_on: task-486-03-capture
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md)
- expected_output: see done_when
- done_when: wbp-upgrade-choice and wbp-alert-objectives describe the plate look and plate hover of their button (no more x1.25 grey tint), sys-button-hover no longer lists grey untextured buttons if none remain; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-486
- usage: included in the executor run of task-486
- runs: 1
- milestone: M2
- started: 2026-11-07T03:43:37Z
- waiting_for: none
- result: Done (26ad1c83). wbp-upgrade-choice: components line, button_hover rewritten (plate hover, no x1.25 tint), new reroll_plate line (RerollSize 440x110, style, fonts, Panel offset 18.5, captures), relation to wbp-run-lost. wbp-alert-objectives: components line, button_hover rewritten, new restart_plate line (RestartSize 440x108, style, Bold 30, EndPanel offset 25, captures). sys-button-hover: untextured grey mode now used by no attached button, kept as fallback. ui-button-hover already held the task-486 line (planner). Lint clean.

## Relations
- [[task-486-grey-buttons-plate]] — parent global task.
