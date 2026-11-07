---
id: task-485-03-run-lost
description: "Bp: Same hover on WBP_RunLost Retry and Main Menu."
parent: task-485-ui-button-hover
kind: task
status: todo
---
# Hover effect on every button of every screen: bp

- goal: Same hover on WBP_RunLost Retry and Main Menu.
- agent: bp
- depends_on: task-485-02-slice-capture
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md)
- expected_output: see done_when
- done_when: Retry and Main Menu show the hover of ui-button-hover in -game, appearance animation and skip unchanged, clicks unchanged; compiled with warnings as errors, saved.
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
