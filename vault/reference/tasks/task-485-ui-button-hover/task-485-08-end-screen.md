---
id: task-485-08-end-screen
description: "Bp: Same hover on the WBP_AlertObjectives end screen button (run-won Main Menu)."
parent: task-485-ui-button-hover
kind: task
status: todo
---
# Hover effect on every button of every screen: bp

- goal: Same hover on the WBP_AlertObjectives end screen button (run-won Main Menu).
- agent: bp
- depends_on: task-485-07-upgrade-reroll
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md)
- expected_output: see done_when
- done_when: The end screen button shows the hover of ui-button-hover, click unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
