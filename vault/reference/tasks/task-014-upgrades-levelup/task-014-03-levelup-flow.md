---
id: task-014-03-levelup-flow
description: "On level-up: effect event, heal 20 % of max health, pause about 0.8 s later on the card offer, chained screens, pick resumes; tests."
parent: task-014-upgrades-levelup
kind: task
status: done
---
# Level-up flow

- goal: a level-up heals, plays its effect, then pauses on the choice.
- agent: cpp
- depends_on: task-014-02-upgrade-system
- inputs: [[xp-level-up]], [[sys-progression]], [[sys-hud]], [[sys-upgrades]], [[sys-player]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: On each level gained: heal 20 % of max health (GE); BlueprintImplementableEvent or delegate for the level-up effect (ring, text; empty sound hook); after `ChoiceDelay` 0.8 s set the game paused and raise `OnUpgradeOfferShown(Cards, bCanReroll)` for the widget; queued levels show one screen after another; `ChooseUpgrade(Index)` applies, then shows the next screen or unpauses; `RequestReroll()` forwards to the offer builder. Input mode switches to UI while paused (mouse visible) and back. [[sys-progression]], [[sys-hud]] updated. Tests `RogueRigs.LevelUp.*`: heal 20 % clamped to max; pause after the delay, not before; two levels → two offers in sequence; choosing unpauses after the last.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 106, cache write 143489, cache read 5033144, output 58691
- runs: 1
- milestone: M1
- result: URRLevelUpComponent on the miner: heal 20 % (URRGE_Heal, Data.Heal), OnLevelUpEffect(NewLevel), ChoiceDelay 0.8 s game time then pause via the miner controller, Game and UI input with cursor, OnUpgradeOfferShown(Cards, bCanReroll) and OnUpgradeOfferClosed on URRHUDWidget, ChooseUpgrade(Index), RequestReroll, CanRerollUpgrade; queued levels chain; unpause only if it paused. Headless world has no controller: real pause and input switch to verify in PIE (014-04). 4 new LevelUp tests; build 0, 74/74. New doc sys-level-up; sys-hud, sys-progression, sys-upgrades, sys-player, sys-automation-tests. Data.Heal not added to naming examples (rule node). Commit f4734ce, about 150k tokens (over the 90k ceiling).

## Relations
- [[task-014-upgrades-levelup]] — parent global task.
