---
id: task-551-03-doc
description: "Doc: Update the inventory and Blueprint docs: bolted bar now on pause, stats and options Graphics."
parent: task-551-options-graphics-divider-bolts
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-551
- usage: included in the executor run of task-551
- runs: 1
- milestone: M2
- started: 2026-11-14T08:47:34Z
- waiting_for: none
- result: Doc commit above: wbp-options GraphicsDividerV row (WBP_UI_BoltedBarV, anchors, offsets) and verification line; wbp-ui-bolted-bar used_by wbp-options; ui-shared-pieces lists the options Graphics divider under bolted bars, T_LevelEnd_DividerV now unreferenced (kept). options-screen gdd line already written by the planner, unchanged. Lint clean.

## Relations
- [[task-551-options-graphics-divider-bolts]] — parent global task.
