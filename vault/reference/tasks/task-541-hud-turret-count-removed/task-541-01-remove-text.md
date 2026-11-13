---
id: task-541-01-remove-text
description: "Bp: Remove the turret count text (and its marker per Q4) from the HUD"
parent: task-541-hud-turret-count-removed
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-541
- usage: included in the executor run of task-541
- runs: 1
- milestone: M2
- started: 2026-11-13T23:08:22Z
- waiting_for: none
- result: WBP_Weapons (commit 3d9bde6c): TurretBox (TurretText 'Turrets: n / max' + TurretBarBox > TurretBar, the grey marker) removed; ShowSlot turret branch (TurretSlotPlusOne set, visibility, text, bar percent) and ProgressSlot TurretBar SetPercent deleted, exec rewired; variable TurretSlotPlusOne removed. Compiled (warnings as errors, clean), saved. Seen in -game 1920x1080: no text, no marker, launcher charge blocks and red recharge block shown, health bar and slots unchanged.

## Relations
- [[task-541-hud-turret-count-removed]] — parent global task.
