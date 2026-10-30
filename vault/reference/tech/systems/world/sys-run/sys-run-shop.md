---
id: sys-run-shop
description: "Intermission shop as built: URRIntermissionShopSubsystem, 5 priced stat cards, buy, paid reroll, snapshot record, prices in URRShopSettings; Blueprint API for the shop screen."
parent: sys-run
kind: doc
---
# Run — intermission shop

- responsibility: the [[run-shop]] of the [[intermission]] (task-316-01): offer of 5 stat cards priced by tier, buying with ore, paid reroll; bought cards carried into the next level.
- main_classes: `URRIntermissionShopSubsystem` (`Source/RogueRigs/Run/RRIntermissionShopSubsystem.h`, `UGameInstanceSubsystem`), slot struct `FRRShopSlot` {`Card` (`FRRUpgradeCard`), `Price` (int32), `bSold`}; `URRShopSettings` (`Run/RRShopSettings.h`); `URRIntermissionWidget` helpers (`UI/`).
- entry_points:
  - Lifecycle ([[sys-run-transition]], task-317-01): the stats screen's `NextLevel` (levels 1-3) calls `OpenShop(Miner)` before `OnShopScreenStarted` (offer ready when the shop widget is created); `Continue` and a run reset call `CloseShop`. No shop after level 4.
  - Offer: `ShopCardCount` cards from `URRUpgradeComponent::DrawStatCardsWithOwnerLuck` (owner luck, offer level 0: stat cards only, [[sys-upgrades]]); price = tier price.
  - Buy: `ApplyUpgrade` on the miner, then `URRRunFlowSubsystem::SpendOre(Price)` (carried first, then secured; carried is 0 here, [[sys-objectives-ore-counter]]), then `URRRunSubsystem::AppendCardToPendingSnapshot` (card added to the pending snapshot, Health/MaxHealth refreshed, replayed on the next miner, [[sys-run]]); the slot becomes `bSold` (empty). Several buys allowed. Refused, nothing spent: shop closed, empty or bad slot, not enough ore, card refused.
  - Reroll: cost `URRRerollCostLibrary::GetRerollCostForCount(RerollCount)` (15, 20, 25…, [[sys-upgrades]]); own `RerollCount`, 0 at each `OpenShop`; redraws every slot (sold ones too), none sharing stat and target with the unsold cards replaced.
  - Ore spent here lowers the bank balance, and the next recap's `RunSecuredOre` (Total Ore) is that balance after the next extraction, so purchases and rerolls are subtracted (task-391-04); the current recap values stay as recorded.
- Blueprint API (for WBP_ShopCard and the shop screen, base `URRShopScreenWidget`, controller property `ShopScreenWidgetClass`):
  - Access: `URRIntermissionShopSubsystem::Get(WorldContext)` ("Get Intermission Shop", pure) or `URRShopScreenWidget::GetShop()` (pure; `URRIntermissionWidget::GetShop()` kept as legacy).
  - Pure: `IsShopOpen()` bool; `GetShopSlots()` TArray<FRRShopSlot>; `GetSlotCount()` int32; `GetSlot(int32 SlotIndex, FRRShopSlot& OutSlot)` bool; `GetSlotPrice(int32 SlotIndex)` int32; `IsSlotEmpty(int32 SlotIndex)` bool; `CanBuySlot(int32 SlotIndex)` bool (false = greyed); `GetOreBalance()` int32 (spendable ore); `GetRerollCost()` int32; `GetRerollCount()` int32; `CanReroll()` bool (false = greyed); static `GetPriceForTier(ERRUpgradeRarity Tier)` int32.
  - Callable: `BuySlot(int32 SlotIndex)` bool; `RequestReroll()` bool.
  - Event: `OnShopChanged()` (BlueprintAssignable, no parameter) after open, close, buy, reroll and every ore change while open. `URRShopScreenWidget` binds it in `NativeConstruct` and calls the BlueprintImplementableEvent `OnShopRefreshed()`; the Blueprint also calls its refresh once at construction (the offer already exists). Continue button: `URRShopScreenWidget::Continue()`.
  - Event `OnSlotBought(int32 SlotIndex, int32 Price, int32 BalanceBefore)` (`FRROnShopSlotBoughtSignature`, BlueprintAssignable, Category `Run|Shop`, task-342-01): broadcast once per successful `BuySlot` (card click or `rr.Shop.Buy`), after card applied, ore paid, snapshot written and slot marked sold, just before `OnShopChanged`. `BalanceBefore` = spendable ore before the payment (after = BalanceBefore - Price). Never fired on a refusal. Used by [[wbp-shop-screen]] and [[wbp-shop-card]] for the ore flight. Log `URRIntermissionShopSubsystem: OnSlotBought slot N, price P, balance before B.`
- data: `URRShopSettings` (`UCLASS(Config = Game, DefaultConfig)`, `GetDefault`): `ShopCardCount` 5, `PriceGrey` 25, `PriceGreen` 40, `PriceBlue` 60, `PricePurple` 80, `PriceYellow` 100 (provisional, owner 2026-10-26); override in `Config/DefaultGame.ini` `[/Script/RogueRigs.RRShopSettings]`. Reroll tunables: `URRRerollSettings`.
- depends_on: [[sys-run-transition]], [[sys-run]], [[sys-upgrades]], [[sys-objectives-ore-counter]].
- implements: [[run-shop]].
- gotchas: bought cards also enter the old miner's `GetPickedUpgrades` and the run log through `OnUpgradeApplied`. The level-up reroll during a level lowers the balance and so the next recap's Total Ore the same way. Log lines `URRIntermissionShopSubsystem: offer of N/5 cards: ...`, `slot N bought (...)`, `reroll N paid C`. Verified in a -game run (task-316-05, `Saved/IntermissionShop/`): offer 300 ore, slot 0 bought for 40 (balance 260, slot shows "Sold. No refunds."), reroll 1 paid 15 (balance 245, next 20), level 2 log `1/1 cards replayed`, bank 245. Greyed (unaffordable) states not captured. No automation test (owner rule).
- debug: `rr.Shop.Buy <Slot>` (0-based, `BuySlot`) and `rr.Shop.Reroll` (`RequestReroll`) in `Run/RRRunDebugCommands.cpp`, plus `rr.Run.EndLevel` then `rr.Run.Stats.Next` to open the shop without Continue ([[sys-cheats]]).

## Relations
- [[sys-run]] — parent: the run state and snapshot the shop writes to.
- [[sys-run-transition]] — opens and closes the shop with the intermission.
- [[sys-upgrades]] — card draw, application and the shared reroll cost rule.
- [[sys-objectives-ore-counter]] — SpendOre and OnOreChanged.
- [[run-shop]] — the rules implemented.
- [[intermission]] — the screen that hosts it.
- [[task-316-01-intermission-shop]] — the task that built it.
- [[task-317-01-stats-then-cards]] — the task that moved it to its own screen after the stats screen.
- [[task-342-01-shop-bought-event]] — the task that added OnSlotBought (verified by capture: 4 events for 4 buys, none on 3 refusals).
