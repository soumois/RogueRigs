---
id: task-479-01-delay
description: "Cpp: The card screen opens about 1.2 s after a level-up; nothing else in the sequence changes."
parent: task-479-level-up-cards-delay
kind: task
status: done
---
# Level-up cards after 1.2 s instead of 1.8 s: cpp

- goal: The card screen opens about 1.2 s after a level-up; nothing else in the sequence changes.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), task-459-level-up-pause-delay (vault/reference/tasks/task-459-level-up-pause-delay.md)
- expected_output: see done_when
- done_when: Build ok; the delay measured in PIE or a test is about 1.2 s, chained level-ups wait once; LevelUp test family passes, figures in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-479
- usage: included in the executor run of task-479
- runs: 1
- milestone: M2
- started: 2026-11-06T20:48:00Z
- waiting_for: none
- result: ChoiceDelay 1.8 -> 1.2 s in URRLevelUpComponent (C++ default; BP_Miner does not override it). Build ok. Commit f72f7825. Measured with BP_Miner and the real game mode (Integration.LevelUpUpgradeLoop): screen opened 72 frames = 1.200 s game time after the level-up, game running until then. RogueRigs.LevelUp 9 of 9 pass (PauseAfterDelay 71 frames not open / 74 open; ChainedOffers two levels wait once then chain), plus Cheats.PropertyRoute/ConsoleVariables, Extraction.EndClosesLevelUpOffer, Upgrades.WeaponCardsOnLevelFive, Integration.LevelUpUpgradeLoop/WeaponCardEquipsShotgun/LevelUpChoiceAfterIntermission/ShuttleTimeoutLoss. Integration.ConsoleExtractionWin pass. Only budgets and labels changed in tests. Unrelated failure seen: Cheats.LightHalo (counts 2 point lights on the miner instead of 1, RRCheatTests.cpp 339 and 345), no link with the delay, not touched.

## Relations
- [[task-479-level-up-cards-delay]] — parent global task.
