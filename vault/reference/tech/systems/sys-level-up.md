---
id: sys-level-up
description: "Level-up flow as built: URRLevelUpComponent on the miner, 20 % heal GE, effect event, delayed pause on the cards, chained screens, Blueprint API."
parent: systems-index
kind: doc
---
# Level-up flow

- responsibility: what happens on each level gained ([[xp-level-up]] sequence): heal, feedback event, delayed pause on the upgrade choice, chained screens, resume. Offers and application stay in [[sys-upgrades]].
- main_classes: `URRLevelUpComponent` (`Source/RogueRigs/Progression/RRLevelUpComponent.h`), default subobject `LevelUp` of `ARRMiner` (`GetLevelUp()`, BlueprintPure); `URRGE_Heal` (`GAS/RRGE_Heal.h`, instant, Health `AddBase` from SetByCaller `Data.Heal`, clamped to MaxHealth by the health set).
- entry_points:
  - Flow: binds `URRProgressionComponent::OnLevelUp` at BeginPlay. Per level: heal `HealFraction` × MaxHealth (skipped when dead), `OnLevelUpEffect(NewLevel)`, one choice queued. The first queued choice arms a `ChoiceDelay` game-time timer (game still running). At its end (dead miner: queue dropped): the owner's player controller `SetPause(true)` (actors, auto-fire, enemies and timers freeze), input mode Game and UI (no mouse lock, cursor shown), `BuildOfferWithOwnerLuck(CardCount)`, `OnUpgradeOfferShown(Cards, bCanReroll)`. Levels gained meanwhile join the queue.
  - `ChooseUpgrade(Index)` (BlueprintCallable, bool): `ApplyUpgrade` on card Index; next queued choice opens at once (new offer, own reroll) or `OnUpgradeOfferClosed`, unpause (only a pause it set), Game Only input (capture mode of `DefaultInput.ini`), cursor as before. False: no screen, bad index, card refused.
  - `RequestReroll()` (BlueprintCallable, bool): `URRUpgradeComponent::Reroll`, then `OnUpgradeOfferShown(NewCards, false)`.
  - Getters (BlueprintPure): `IsAwaitingChoice`, `IsChoiceDelayActive`, `GetPendingChoices`, `GetOfferCards`, `CanReroll`.
  - Delegates (BlueprintAssignable): `OnLevelUpEffect(int32 NewLevel)` — BP_Miner binds it for the cyan ring and the sound hook (sound open); `OnUpgradeOfferShown`, `OnUpgradeOfferClosed` — forwarded by the HUD ([[sys-hud]]).
- data: component defaults, editable in BP_Miner: `ChoiceDelay` 0.8 s, `HealFraction` 0.2, `CardCount` 3, `HealEffectClass` `URRGE_Heal` (values provisional, [[xp-level-up]]).
- depends_on: [[sys-progression]], [[sys-upgrades]], [[sys-gas-core]], [[sys-player]].
- implements: [[xp-level-up]] (level-up sequence, chained screens, reroll button).
- gotchas: headless tests have no player controller or game mode, so no engine pause: `RogueRigs.LevelUp.*` prove the flow state (`IsAwaitingChoice`), the real freeze is a PIE check. Enhanced Input actions do not trigger while paused (miner input stops); UMG keeps working. EndPlay ends a pause it still holds. A level gained while dead heals nothing and opens no screen.

## Relations
- [[systems-index]] — parent index.
- [[sys-progression]] — the OnLevelUp it listens to.
- [[sys-upgrades]] — the offer builder it drives.
- [[sys-hud]] — the widget that shows the cards and calls ChooseUpgrade / RequestReroll.
- [[sys-automation-tests]] — RogueRigs.LevelUp.* prove the flow.
