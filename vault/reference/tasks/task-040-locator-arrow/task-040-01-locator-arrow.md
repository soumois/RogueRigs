---
id: task-040-01-locator-arrow
description: "Console locator: real arrow about 64 px, pulsing, with the distance in metres."
parent: task-040-locator-arrow
kind: task
status: in-progress
---
# Clearer console locator

- goal: Console locator: real arrow about 64 px, pulsing, with the distance in metres.
- agent: bp
- depends_on: none
- inputs: objective-console (vault/reference/gdd/run/objective-console.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-hud (vault/reference/tech/systems/sys-hud.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), sys-cheats (vault/reference/tech/systems/sys-cheats.md)
- expected_output: WBP_HUD LocatorArrow: replace the ">" text with an arrow image about 64 px (create a simple arrow texture or use an engine arrow brush), pulsing scale, teal for the console and white for the shuttle, rotated by ArrowAngle at EdgePosition; a distance text in metres (WorldLocation to the miner / 100) next to it; docs wbp-hud; PIE: no log error, arrow and distance visible with the console off screen.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-040-locator-arrow]] — parent global task.
