---
id: sys-run-shop
description: "Intermission shop as built: URRIntermissionShopSubsystem, 5 priced stat cards, buy, paid reroll, snapshot record, prices in URRShopSettings; Blueprint API for WBP_Intermission."
parent: sys-run
kind: doc
---
# Run — intermission shop

- responsibility: the [[run-shop]] of the [[intermission]] (task-316-01): offer of 5 stat cards priced by tier, buying with ore, paid reroll; bought cards carried into the next level.
- main_classes: `URRIntermissionShopSubsystem` (`Source/RogueRigs/Run/RRIntermissionShopSubsystem.h`, `UGameInstanceSubsystem`), slot struct `FRRShopSlot` {`Card` (`FRRUpgradeCard`), `Price` (int32), `bSold`}; `URRShopSettings` (`Run/RRShopSettings.h`); `URRIntermissionWidget` helpers (`UI/`).
- entry_points:
  - Lifecycle ([[sys-run-transition]]): `HandleLevelExtracted` calls `OpenShop(Miner)` before `OnIntermissionStarted` (offer ready when the widget is created); `Continue` and a run reset call `CloseShop`.
  - Offer: `ShopCardCount` cards from `URRUpgradeComponent::DrawStatCardsWithOwnerLuck` (owner luck, offer level 0: stat cards only, [[sys-upgrades]]); price = tier price.
  - Buy: `ApplyUpgrade` on the miner, then `URRRunFlowSubsystem::SpendOre(Price)` (carried first, then secured; carried is 0 here, [[sys-objectives-ore-counter]]), then `URRRunSubsystem::AppendCardToPendingSnapshot` (card added to the pending snapshot, Health/MaxHealth refreshed, replayed on the next miner, [[sys-run]]); the slot becomes `bSold` (empty). Several buys allowed. Refused, nothing spent: shop closed, empty or bad slot, not enough ore, card refused.
  - Reroll: cost `URRRerollCostLibrary::GetRerollCostForCount(RerollCount)` (15, 20, 25…, [[sys-upgrades]]); own `RerollCount`, 0 at each `OpenShop`; redraws every slot (sold ones too), none sharing stat and target with the unsold cards replaced.
  - Ore spent here calls `URRRunSubsystem::RecordIntermissionOreSpent`: the next recap's `OreSecuredThisLevel` and `RunSecuredOre` keep counting ore secured, not spent; the current recap values stay as recorded.
- Blueprint API (for WBP_ShopCard, WBP_Intermission):
  - Access: `URRIntermissionShopSubsystem::Get(WorldContext)` ("Get Intermission Shop", pure) or `URRIntermissionWidget::GetShop()` (pure).
  - Pure: `IsShopOpen()` bool; `GetShopSlots()` TArray<FRRShopSlot>; `GetSlotCount()` int32; `GetSlot(int32 SlotIndex, FRRShopSlot& OutSlot)` bool; `GetSlotPrice(int32 SlotIndex)` int32; `IsSlotEmpty(int32 SlotIndex)` bool; `CanBuySlot(int32 SlotIndex)` bool (false = greyed); `GetOreBalance()` int32 (spendable ore); `GetRerollCost()` int32; `GetRerollCount()` int32; `CanReroll()` bool (false = greyed); static `GetPriceForTier(ERRUpgradeRarity Tier)` int32.
  - Callable: `BuySlot(int32 SlotIndex)` bool; `RequestReroll()` bool.
  - Event: `OnShopChanged()` (BlueprintAssignable, no parameter) after open, close, buy, reroll and every ore change while open. `URRIntermissionWidget` binds it in `NativeConstruct` and calls the BlueprintImplementableEvent `OnShopRefreshed()`; the Blueprint also calls its refresh once at construction (the offer already exists).
- data: `URRShopSettings` (`UCLASS(Config = Game, DefaultConfig)`, `GetDefault`): `ShopCardCount` 5, `PriceGrey` 25, `PriceGreen` 40, `PriceBlue` 60, `PricePurple` 80, `PriceYellow` 100 (provisional, owner 2026-10-26); override in `Config/DefaultGame.ini` `[/Script/RogueRigs.RRShopSettings]`. Reroll tunables: `URRRerollSettings`.
- depends_on: [[sys-run-transition]], [[sys-run]], [[sys-upgrades]], [[sys-objectives-ore-counter]].
- implements: [[run-shop]].
- gotchas: bought cards also enter the old miner's `GetPickedUpgrades` and the run log through `OnUpgradeApplied`. The level-up reroll during a level still lowers the recap's secured figures when it spends secured ore (task-315 behaviour, unchanged). Log lines `URRIntermissionShopSubsystem: offer of N/5 cards: ...`, `slot N bought (...)`, `reroll N paid C`. Not verified in a running game yet (no widget binding, task-316-05 capture); no automation test (owner rule).

## Relations
- [[sys-run]] — parent: the run state and snapshot the shop writes to.
- [[sys-run-transition]] — opens and closes the shop with the intermission.
- [[sys-upgrades]] — card draw, application and the shared reroll cost rule.
- [[sys-objectives-ore-counter]] — SpendOre and OnOreChanged.
- [[run-shop]] — the rules implemented.
- [[intermission]] — the screen that hosts it.
- [[task-316-01-intermission-shop]] — the task that built it.
