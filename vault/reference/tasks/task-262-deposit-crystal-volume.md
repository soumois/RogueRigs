---
id: task-262-deposit-crystal-volume
description: "Global task: The Duskite and heal deposit crystals read as volumes: capped emission peak, height gradient and rim light, textures allowed if needed."
parent: tasks-done-index
kind: task
status: done
---
# Deposit crystal volume

- request: the Duskite crystal looks flat with no volume (white blown out); textures may be used if necessary (owner, 2026-10-20)
- goal: The Duskite and heal deposit crystals read as volumes: capped emission peak, height gradient and rim light, textures allowed if needed.
- acceptance: Material compiles; the deposit instances use it (read back); emissive peak and rest values read back; the three deposit variants keep their slots; ore pickup instances unchanged; pulse desync inputs still wired; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. New M_DepositCrystalGlow (duplicate of M_XPNutGlow, which is untouched): same pulse inputs (per-instance 4 and 5), emission capped (GlowAmount 2, PulseAmount 0.5; peak about (2.5, 0.45, 4.5)), height gradient (GradientBase 0.35 over 70 uu) and Fresnel rim (power 3, amount 0.5), Roughness 0.4, no texture. MI_DuskiteDeposit_Crystal reparented (violet values kept); new MI_HealDepositCrystal (red, GlowAmount 1.5) on the heal deposit crystal slots; MI_HealCrystal_* kept for the rock crystal clusters; ore pickups unchanged. Compiles; node bp-deposit-crystal-material with tuning knobs. Not seen on screen.

## Subtasks
1. task-262-01-deposit-crystal-volume — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-262-01-deposit-crystal-volume]] — step 1, bp.
