---
id: task-551-03-doc
description: "Doc: Update the inventory and Blueprint docs: bolted bar now on pause, stats and options Graphics."
parent: task-551-options-graphics-divider-bolts
kind: task
status: todo
---
# Options Graphics divider: bolts on the frame borders like the pause screen: doc

- goal: Update the inventory and Blueprint docs: bolted bar now on pause, stats and options Graphics.
- agent: doc
- depends_on: task-551-02-capture
- inputs: ui-shared-pieces (vault/reference/tech/ui-shared-pieces.md), wbp-ui-bolted-bar (vault/reference/tech/blueprints/ui/wbp-ui-bolted-bar.md)
- expected_output: see done_when
- done_when: ui-shared-pieces lists the options Graphics divider under the bolted bars and no longer keeps T_LevelEnd_DividerV for it; wbp-ui-bolted-bar lists the options widget as a user; the options widget Blueprint doc node records the divider change; lint clean.
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
- [[task-551-options-graphics-divider-bolts]] — parent global task.
