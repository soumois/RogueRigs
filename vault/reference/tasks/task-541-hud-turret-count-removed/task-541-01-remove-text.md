---
id: task-541-01-remove-text
description: "Bp: Remove the turret count text (and its marker per Q4) from the HUD"
parent: task-541-hud-turret-count-removed
kind: task
status: todo
---
# HUD: remove the "Turrets: 2 / 2" text above the health bar: bp

- goal: Remove the turret count text (and its marker per Q4) from the HUD
- agent: bp
- depends_on: none
- inputs: turret (vault/reference/gdd/weapons/turret.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: A1 to A3 hold; nothing else of the HUD moves.
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
- [[task-541-hud-turret-count-removed]] — parent global task.
