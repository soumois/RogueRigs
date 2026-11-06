---
id: task-464-02-doc
description: "Doc: Update the bp-* and sys-* nodes of the recoloured assets and check color-code matches the result"
parent: task-464-player-shot-colour
kind: task
status: todo
---
# Player shot colour: doc

- goal: Update the bp-* and sys-* nodes of the recoloured assets and check color-code matches the result
- agent: doc
- depends_on: task-464-01-rifle-laser-green
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md)
- expected_output: see done_when
- done_when: Every bp-* or sys-* node of a changed asset states the new colour; color-code and burst-rifle match what is on screen; lint clean.
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
- [[task-464-player-shot-colour]] — parent global task.
