---
id: task-486-04-doc
description: "Doc: Doc nodes of the two changed widgets and of the hover mechanism updated."
parent: task-486-grey-buttons-plate
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T03:43:37Z
- waiting_for: none
- result: open

## Relations
- [[task-486-grey-buttons-plate]] — parent global task.
