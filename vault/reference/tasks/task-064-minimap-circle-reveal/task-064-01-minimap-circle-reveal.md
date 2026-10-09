---
id: task-064-01-minimap-circle-reveal
description: "Minimap reveal is a circle of 18 cells around the miner, the same in every direction."
parent: task-064-minimap-circle-reveal
kind: task
status: todo
---
# Circular minimap reveal

- goal: Minimap reveal is a circle of 18 cells around the miner, the same in every direction.
- agent: cpp
- depends_on: none
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: URRMinimap seen rule: replace the projected view trapezoid (perspective camera, hence more reveal upward) with a circle of RevealRadiusCells (18, editable) around the miner cell, checked only when the miner changes cell; keep the per-frame cost low; update only existing tests that break; docs sys-hud-minimap.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-064-minimap-circle-reveal]] — parent global task.
