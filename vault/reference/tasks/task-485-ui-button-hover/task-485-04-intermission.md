---
id: task-485-04-intermission
description: "Bp: Same hover on WBP_Intermission Continue."
parent: task-485-ui-button-hover
kind: task
status: todo
---
# Hover effect on every button of every screen: bp

- goal: Same hover on WBP_Intermission Continue.
- agent: bp
- depends_on: task-485-03-run-lost
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md)
- expected_output: see done_when
- done_when: Continue (and CASH OUT on level 4) shows the hover of ui-button-hover, click unchanged; compiled with warnings as errors, saved.
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
