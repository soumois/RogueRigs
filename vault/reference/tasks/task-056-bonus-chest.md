---
id: task-056-bonus-chest
description: "Global task: Bonus chest: one per level at 60 % alert, 8 s hack, XP, ore and a lucky upgrade choice."
parent: tasks-done-index
kind: task
status: done
---
# Bonus chest

- request: "Prévois le coffre bonus" (owner, 2026-10-07)
- goal: Bonus chest: one per level at 60 % alert, 8 s hack, XP, ore and a lucky upgrade choice.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. BP_BonusChest (child of ARRBonusChest, gold cube, gold hack ring, glow on OnRewardGranted), MI_GB_BonusChest, MI_GroundRingGold; BP_RRGameMode RunFlowConfig.BonusChestClass set (recompile needed after editing); WBP_HUD gold 'B' AlertIconChest with the BonusChest case. PIE: chest dropped at 60 % alert, no log error; hack completion and rewards not exercised (no cheat targets the chest); not seen on screen. Docs bp-bonus-chest (new), wbp-hud, bp-rr-game-mode, blueprints-index. Commits 12bd69e, e7e20f2, a90285c.

## Subtasks
1. task-056-01-bonus-chest — cpp
2. task-056-02-bp-bonus-chest — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-056-01-bonus-chest]] — step 1, cpp.
- [[task-056-02-bp-bonus-chest]] — step 2, bp.
