---
id: task-196-xp-chip-smaller
description: "Global task: Big XP chip 15 percent smaller: BP_PickupXPBig template scale 1.8 to 1.53 and its glow disc radius 80 to 68."
parent: tasks-done-index
kind: task
status: done
---
# Smaller big XP chip

- request: Réduit la taille de la puce de 15% (owner, 2026-10-15)
- goal: Big XP chip 15 percent smaller: BP_PickupXPBig template scale 1.8 to 1.53 and its glow disc radius 80 to 68.
- acceptance: Blueprint compiles; scale and Radius read back; docs updated; lint clean; committed by path; committed.
- gdd_check: xp-level-up read 2026-10-15: big XP block scale 1.8 becomes 1.53 (owner); glow disc radius scaled the same (default accepted). No other conflict.
- confirmed_by_owner: yes (2026-10-15: "ok pour les deux tâches avec ces valeurs")
- milestone: M1
- next_step: none
- result: Done. BP_PickupXPBig root template scale 1.53 (was 1.8) and MI_XPGlowDisc_Chip Radius 68 (was 80), read back and saved; TrailFX relative 0.3 unchanged (effective 0.459); docs updated; not seen in PIE; commit e73816a3

## Subtasks
1. task-196-01-xp-chip-smaller — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-196-01-xp-chip-smaller]] — step 1, bp.
