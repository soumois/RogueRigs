---
id: sys-level-up
description: "Level-up flow as built: URRLevelUpComponent on the miner, 20 % heal GE, effect event, delayed pause, chained screens, ore-paid reroll, Blueprint API."
parent: systems-core-index
kind: doc
---
# Level-up flow

- responsibility: what happens on each level gained ([[xp-level-up]] sequence): heal, feedback event, delayed pause on the upgrade choice, chained screens, resume. Offers and application stay in [[sys-upgrades]].
- main_classes: `URRLevelUpComponent` (`Source/RogueRigs/Progression/RRLevelUpComponent.h`), default subobject `LevelUp` of `ARRMiner` (`GetLevelUp()`, BlueprintPure); `URRGE_Heal` (`GAS/RRGE_Heal.h`, instant, Health `AddBase` from SetByCaller `Data.Heal`, clamped to MaxHealth by the health set).
- entry_points:
  - Flow: binds `URRProgressionComponent::OnLevelUp` at BeginPlay. Per level: heal `HealFraction` × MaxHealth (skipped when dead), `OnLevelUpEffect(NewLevel)`, one choice queued. The first queued choice arms a `ChoiceDelay` game-time timer (game still running). At its end (dead miner: queue dropped): the owner's player controller `SetPause(true)` (actors, auto-fire, enemies and timers freeze), input mode Game and UI (no mouse lock, cursor shown), `BuildOfferWithOwnerLuck(CardCount)`, `OnUpgradeOfferShown(Cards, bCanReroll)` (bCanReroll = `CanReroll()`: the next reroll is affordable now). Levels gained meanwhile join the queue. `CancelChoices()` (run end, [[sys-objectives]]): clears the delay and queue, closes an open screen (`OnUpgradeOfferClosed`, unpause, input restored) and blocks later screens (`AreChoicesBlocked`; level-ups still heal).
  - `ChooseUpgrade(Index)` (BlueprintCallable, bool): `ApplyUpgrade` on card Index; next queued choice opens at once (new offer) or `OnUpgradeOfferClosed`, unpause (only a pause it set), Game Only input (capture mode of `DefaultInput.ini`), cursor as before. False: no screen, bad index, card refused.
  - Paid reroll (task-315-01, [[xp-level-up]]): `RequestReroll()` (BlueprintCallable, bool) checks `CanReroll`, pays `GetRerollCost()` through `URRRunFlowSubsystem::SpendOre` (carried first, then secured, `OnOreChanged` once, [[sys-objectives-ore-counter]]), calls `URRUpgradeComponent::Reroll`, increments `RerollCount` and broadcasts `OnUpgradeOfferShown(NewCards, CanReroll())`; false (nothing spent) without a screen or enough ore. No limit per screen. Cost = `URRRerollCostLibrary::GetRerollCostForCount(RerollCount)` ([[sys-upgrades]]): 15, 20, 25… `RerollCount` returns to 0 when `ShowNextOffer` opens a level-up offer (offer level > 0, chained screens included); a bonus chest offer (level 0) keeps it. `CanReroll()` = screen open, offer present and `CanSpendOre(GetRerollCost())`. The HUD forwards `CanRerollUpgrade()` and `GetUpgradeRerollCost()` ([[sys-hud]]).
  - Getters (BlueprintPure): `IsAwaitingChoice`, `IsChoiceDelayActive`, `GetPendingChoices`, `GetOfferCards`, `CanReroll`, `GetRerollCost` (int32), `GetRerollCount` (int32).
  - Burst (task-441, [[xp-level-up]]): `PlayLevelUpBurst` runs in `HandleLevelUp` after `OnLevelUpEffect`, once per moment: skipped when dead, once the run is over (`bChoicesBlocked`), or less than `BurstMinInterval` after the last burst, so several levels gained at once give one. It launches robots (`PushNearbyRobots`: each living `ARREnemy` within `PushRadius` in 2D gets `ApplyKnockback` away from the miner at `PushSpeed`, so `KnockbackResistance` applies and no damage is dealt, [[sys-enemies-base]]), then spawns through `URRNiagaraPoolSubsystem` (cap `BurstPoolSize` 2, prewarmed at BeginPlay) `LevelUpTextSystem` at capsule top + `LevelUpTextHeight` and `LevelUpWaveSystem` at the capsule bottom + `LevelUpWaveFloorOffset`, with `WaveRadiusParameter` (`User.WaveRadius`) = `PushRadius`. It does not go through [[sys-damage-numbers]], so the player toggle does not hide the text. Getter `GetLevelUpBurstCount`. Assets: [[bp-level-up-burst]].
  - Delegates (BlueprintAssignable): `OnLevelUpEffect(int32 NewLevel)`, once per level — BP_Miner binds it for the green heal ring and the sound hook (sound open); `OnUpgradeOfferShown`, `OnUpgradeOfferClosed` — forwarded by the HUD ([[sys-hud]]).
- data: component defaults, editable in BP_Miner: `ChoiceDelay` 0.8 s, `HealFraction` 0.2, `CardCount` 3, `HealEffectClass` `URRGE_Heal`, `LevelUpTextHeight` 40, `LevelUpWaveFloorOffset` 5, `BurstMinInterval` 0.7 s, `PushRadius` 300, `PushSpeed` 800 uu/s, about 74 uu of travel for a chaser in the test; `LevelUpTextSystem` and `LevelUpWaveSystem` null in C++, set by BP_Miner (values provisional, [[xp-level-up]]). Reroll cost tunables: `URRRerollSettings` ([[sys-upgrades]]).
- depends_on: [[sys-progression]], [[sys-upgrades]], [[sys-gas-core]], [[sys-player]].
- implements: [[xp-level-up]] (level-up sequence, chained screens, reroll button).
- gotchas: the bare test world has no game mode, so `RogueRigs.LevelUp.*` prove the flow state (`IsAwaitingChoice`); `RogueRigs.Integration.LevelUpUpgradeLoop` gives its world a game instance and `BP_RRGameMode` and proves the real pause (world time, spawner, rifle frozen) and resume. In PIE, `rr.Cheat.GrantXP` / `ChooseUpgrade` / `Reroll` drive the screen ([[sys-cheats]]); `Reroll` pays like the button (grant ore first with `rr.Cheat.GrantOre`). The run flow is a Game/PIE world subsystem: without it `CanReroll` is false. Enhanced Input actions do not trigger while paused (miner input stops); UMG keeps working. EndPlay ends a pause it still holds. A level gained while dead heals nothing and opens no screen.

## Relations
- [[systems-index]] — parent index.
- [[sys-progression]] — the OnLevelUp it listens to.
- [[sys-upgrades]] — the offer builder it drives.
- [[bp-level-up-burst]] — the text and wave it plays.
- [[sys-enemies-base]] — the knockback the push uses.
- [[sys-hud]] — the widget that shows the cards and calls ChooseUpgrade / RequestReroll.
- [[sys-automation-tests]] — RogueRigs.LevelUp.* and Integration.LevelUpUpgradeLoop prove the flow.
- [[sys-cheats]] — console variables that pick or reroll the cards in PIE.
- [[sys-objectives-ore-counter]] — the ore that pays the reroll (SpendOre).
