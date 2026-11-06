---
id: task-464-05-doc
description: "Doc: Update the bp-* and sys-* nodes of the changed effects and check the gdd nodes match the result"
parent: task-464-player-shot-colour
kind: task
status: in-progress
---
# Player shot colour: doc

- goal: Update the bp-* and sys-* nodes of the changed effects and check the gdd nodes match the result
- agent: doc
- depends_on: task-464-04-turret-muzzle-flash
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), turret (vault/reference/gdd/weapons/turret.md)
- expected_output: see done_when
- done_when: Every bp-* or sys-* node of a changed asset states the new look; color-code, burst-rifle and turret match what is on screen; the reference image is listed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T09:16:19Z
- waiting_for: none
- result: open

## Relations
- [[task-464-player-shot-colour]] — parent global task.
