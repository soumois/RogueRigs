---
id: task-016-03-bp-objectives
description: "Console and shuttle visuals, marked-rock outline, hack gauge, alert bar and countdown in the HUD, success and run-lost screens, PIE."
parent: task-016-console-extraction
kind: task
status: in-progress
---
# Objectives Blueprints and HUD

- goal: the objectives are visible and the loop is playable.
- agent: bp
- depends_on: task-016-02-shuttle
- inputs: [[objective-console]], [[extraction]], [[color-code]], [[readability-rules]], [[wbp-hud]], [[bp-drone]], [[unreal-mcp]], [[cpp-bp-split]].
- expected_output: grey-box console and shuttle Blueprints (white + teal), pulsing white outline on marked rocks, world gauge on the console, HUD alert bar and countdown, end screens with a "Rejouer" button calling restart; new Blueprint docs and [[wbp-hud]] updated. PIE (alert sped up with the cheat): console drops, rocks cleared, hack, shuttle lands, extraction success; and a loss by timeout; no log error.
- done_when: PIE checks done with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 120000
- milestone: M1
- result: open

## Relations
- [[task-016-console-extraction]] — parent global task.
