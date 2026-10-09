---
id: task-062-01-alert-bar-x2
description: "Alert bar twice as wide and twice as tall, its icons scaled and repositioned."
parent: task-062-alert-bar-x2
kind: task
status: in-progress
---
# Bigger alert bar

- goal: Alert bar twice as wide and twice as tall, its icons scaled and repositioned.
- agent: bp
- depends_on: none
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: WBP_HUD: the alert bar width and height × 2 (keep it centred at the top), AlertLabel and the event icons (console, chest, hordes, shuttle) scaled × 2 and placed with the new width (PlaceIcon Position × the new bar width); the objective text and the stack under it moved down so nothing overlaps; docs wbp-hud; PIE: no log error, sizes read back.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-062-alert-bar-x2]] — parent global task.
