---
id: task-459-01-delay
description: "Cpp: The card screen opens about 1.8 s after a level-up; nothing else in the sequence changes."
parent: task-459-level-up-pause-delay
kind: task
status: done
---
# One more second before the level-up cards: cpp

- goal: The card screen opens about 1.8 s after a level-up; nothing else in the sequence changes.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), task-441-level-up-effect (vault/reference/tasks/task-441-level-up-effect.md)
- expected_output: see done_when
- done_when: Build ok; the delay measured in PIE or a test is about 1.8 s, chained level-ups wait once; LevelUp test family passes, figures in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-459
- usage: included in the executor run of task-459
- runs: 1
- milestone: M2
- started: 2026-11-06T04:09:56Z
- waiting_for: none
- result: ChoiceDelay 0.8 -> 1.8 s in URRLevelUpComponent (C++ default; BP_Miner does not override it). Build ok. 19 tests passed (EXIT 0): RogueRigs.LevelUp (10, PauseAfterDelay 107 frames not open / 110 open, ChainedOffers two levels wait once then chain), Integration.LevelUpUpgradeLoop with BP_Miner and real game mode: screen opened 108 frames = 1.800 s game time after the level-up, game running until then; plus the tests whose frame budgets the longer delay broke, updated by budget only: Cheats.PropertyRoute/ConsoleVariables, Extraction.EndClosesLevelUpOffer, Upgrades.WeaponCardsOnLevelFive, Integration.WeaponCardEquipsShotgun/LevelUpChoiceAfterIntermission/ConsoleExtractionWin/ShuttleTimeoutLoss. Commit b68fc3f9.

## Relations
- [[task-459-level-up-pause-delay]] — parent global task.
