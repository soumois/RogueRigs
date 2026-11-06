// Rogue Rigs - upgrade shop of the intermission (run-shop, task-316-01): 5 stat cards priced by rarity, bought with ore, paid
// reroll of the 5 slots; bought cards carried into the next level through the run snapshot.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Upgrades/RRUpgradeCard.h"
#include "Weapons/RRWeapon.h"
#include "RRIntermissionShopSubsystem.generated.h"

class ARRMiner;
class URRRunFlowSubsystem;

/** One slot of the intermission shop: its card, its ore price, and whether it was bought (an empty slot). */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRShopSlot
{
	GENERATED_BODY()

	/** Card on sale (stat card only); unused once bSold. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Shop")
	FRRUpgradeCard Card;

	/** Ore price of the card (URRShopSettings price of its tier). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Shop")
	int32 Price = 0;

	/** True once the card was bought: the slot stays empty until the next reroll or the next intermission. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Shop")
	bool bSold = false;
};

/** Fired when anything the shop shows changed: offer opened or closed, card bought, reroll, ore balance changed. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnShopChangedSignature);

/**
 * Fired once per successful BuySlot (card click or rr.Shop.Buy), before OnShopChanged: SlotIndex bought, Price paid and
 * BalanceBefore (spendable ore before the payment; the balance after is BalanceBefore - Price). Starts the purchase effects
 * (task-342: ore flight from the balance to the Buy button). Never fired on a refused buy.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnShopSlotBoughtSignature, int32, SlotIndex, int32, Price, int32, BalanceBefore);

/**
 * Intermission shop (run-shop, intermission). Game instance subsystem driven by URRRunTransitionSubsystem: OpenShop when the
 * intermission starts (before OnIntermissionStarted, so the widget finds the offer at once), CloseShop on Continue or a run reset.
 *
 * Offer: URRShopSettings::ShopCardCount (5) stat cards drawn like a level-up offer with the miner's luck
 * (URRUpgradeComponent::DrawStatCardsWithOwnerLuck: no new-weapon card), each priced by its tier (URRShopSettings).
 * Buy: refused when the slot is empty, out of range or unaffordable; else applies the card to the miner
 * (URRUpgradeComponent::ApplyUpgrade), pays its price with URRRunFlowSubsystem::SpendOre (carried first, then secured; carried is
 * 0 at the intermission), adds it to the pending run snapshot (URRRunSubsystem::AppendCardToPendingSnapshot) so the next level's
 * miner replays it, and empties the slot. Several buys are allowed.
 * Reroll: cost URRRerollCostLibrary::GetRerollCostForCount(RerollCount) (shared rule: 15, 20, 25...), its own count reset to 0
 * at each OpenShop; redraws every slot (sold ones included), no card sharing stat and target with the unsold cards replaced.
 * Ore spent here lowers the secured balance, which is also the next stats screen Total Ore (task-391-04).
 * OnShopChanged fires after each of these and on every OnOreChanged of the intermission world's run flow while open.
 * OnSlotBought fires once per successful buy, just before its OnShopChanged (purchase effects, task-342).
 */
UCLASS()
class ROGUERIGS_API URRIntermissionShopSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Closes any open shop (unbinds the run flow). */
	virtual void Deinitialize() override;

	/** The shop subsystem of WorldContext's game instance; null without an initialized game instance (bare test worlds). */
	UFUNCTION(BlueprintPure, Category = "Run|Shop", meta = (WorldContext = "WorldContext", DisplayName = "Get Intermission Shop"))
	static URRIntermissionShopSubsystem* Get(const UObject* WorldContext);

	/**
	 * Opens the shop for Miner (the extracted miner, intermission world): reroll count 0, new offer, binds the run flow's
	 * OnOreChanged, OnShopChanged. False (shop closed) without a miner or its upgrade component.
	 */
	bool OpenShop(ARRMiner* Miner);

	/** Closes the shop: offer cleared, run flow unbound, OnShopChanged when it was open. */
	void CloseShop();

	/** True between OpenShop and CloseShop (the intermission). */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	bool IsShopOpen() const { return bShopOpen; }

	/** Slots of the offer in display order (ShopCardCount entries, fewer only when the upgrade pool is smaller); empty when closed. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	const TArray<FRRShopSlot>& GetShopSlots() const { return Slots; }

	/** Number of slots of the current offer (0 when closed). */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	int32 GetSlotCount() const { return Slots.Num(); }

	/** Slot SlotIndex in OutSlot; false (OutSlot default) when out of range. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	bool GetSlot(int32 SlotIndex, FRRShopSlot& OutSlot) const;

	/** Ore price of slot SlotIndex; 0 when out of range. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	int32 GetSlotPrice(int32 SlotIndex) const;

	/** True when slot SlotIndex was bought (empty slot) or is out of range. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	bool IsSlotEmpty(int32 SlotIndex) const;

	/** True when slot SlotIndex can be bought now: shop open, card present, price affordable. False means greyed. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	bool CanBuySlot(int32 SlotIndex) const;

	/** Buys slot SlotIndex (see the class comment). False, nothing spent or applied, when CanBuySlot is false or the card is refused. */
	UFUNCTION(BlueprintCallable, Category = "Run|Shop")
	bool BuySlot(int32 SlotIndex);

	/** Ore that can pay now (URRRunFlowSubsystem::GetSpendableOre of the intermission world: carried 0 + secured); 0 when closed. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	int32 GetOreBalance() const;

	/** Ore cost of the next shop reroll (shared rule, this shop's count). */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	int32 GetRerollCost() const;

	/** Shop rerolls paid since this intermission opened. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	int32 GetRerollCount() const { return RerollCount; }

	/** True when the next reroll can be paid now (shop open, GetRerollCost affordable). False means greyed. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	bool CanReroll() const;

	/** Pays GetRerollCost and redraws every slot; RerollCount + 1. False, nothing spent, when CanReroll is false. */
	UFUNCTION(BlueprintCallable, Category = "Run|Shop")
	bool RequestReroll();

	/** Ore price of a card of Tier (URRShopSettings: grey 25, green 40, blue 60, purple 80, yellow 100). */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	static int32 GetPriceForTier(ERRUpgradeRarity Tier);

	/** See FRROnShopChangedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Shop")
	FRROnShopChangedSignature OnShopChanged;

	/** See FRROnShopSlotBoughtSignature: one broadcast per successful buy, before OnShopChanged (the slot is already bSold). */
	UPROPERTY(BlueprintAssignable, Category = "Run|Shop")
	FRROnShopSlotBoughtSignature OnSlotBought;

protected:
	/** Fills Slots with fresh cards from the miner, none sharing stat and target with Excluded; logs the offer. */
	void DrawSlots(const TArray<FRRUpgradeCard>& Excluded);

	/** Run flow of the intermission world (null when closed or the world is gone). */
	URRRunFlowSubsystem* GetRunFlow() const;

	/** Pays Amount through the run flow and reports it to the run subsystem; false when refused. */
	bool PayOre(int32 Amount, const TCHAR* Reason);

	/** Run flow OnOreChanged callback while open: OnShopChanged (affordability). */
	UFUNCTION()
	void HandleOreChanged(int32 CarriedOre, int32 SecuredOre);

	/** True between OpenShop and CloseShop. */
	bool bShopOpen = false;

	/** Slots of the current offer. */
	UPROPERTY(Transient)
	TArray<FRRShopSlot> Slots;

	/** Shop rerolls paid since OpenShop. */
	int32 RerollCount = 0;

	/** True while PayOre spends: the resulting OnOreChanged is not relayed (the buy or reroll broadcasts once at its end). */
	bool bPaying = false;

	/** Miner the shop sells to (the extracted miner of the intermission world). */
	TWeakObjectPtr<ARRMiner> ShopMiner;

	/** Run flow whose OnOreChanged is bound while open. */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;
};
