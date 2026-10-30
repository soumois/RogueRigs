// Rogue Rigs - upgrade shop of the intermission (run-shop, task-316-01): 5 stat cards priced by rarity, bought with ore, paid
// reroll of the 5 slots; bought cards carried into the next level through the run snapshot.

#include "Run/RRIntermissionShopSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRShopSettings.h"
#include "Upgrades/RRRerollCost.h"
#include "Upgrades/RRUpgradeComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRIntermissionShopSubsystem)

namespace
{
	/** Short log text of a shop card: "Stat/Target Tier price N". */
	FString DescribeShopSlot(const FRRShopSlot& Slot)
	{
		const FString Stat = StaticEnum<ERRUpgradeStat>()->GetNameStringByValue(static_cast<int64>(Slot.Card.Stat));
		const FString Target = Slot.Card.Target == ERRUpgradeTarget::Weapon ? GetNameSafe(Slot.Card.TargetWeapon.Get())
			: StaticEnum<ERRUpgradeTarget>()->GetNameStringByValue(static_cast<int64>(Slot.Card.Target));
		const FString Tier = StaticEnum<ERRUpgradeRarity>()->GetNameStringByValue(static_cast<int64>(Slot.Card.Tier));
		return FString::Printf(TEXT("%s/%s %s %d"), *Stat, *Target, *Tier, Slot.Price);
	}
}

URRIntermissionShopSubsystem* URRIntermissionShopSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext && GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URRIntermissionShopSubsystem>() : nullptr;
}

void URRIntermissionShopSubsystem::Deinitialize()
{
	CloseShop();
	Super::Deinitialize();
}

bool URRIntermissionShopSubsystem::OpenShop(ARRMiner* Miner)
{
	CloseShop();
	if (!Miner || !Miner->GetUpgrades())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRIntermissionShopSubsystem: shop not opened (no miner or no upgrade component)."));
		return false;
	}
	ShopMiner = Miner;
	bShopOpen = true;
	RerollCount = 0;
	if (URRRunFlowSubsystem* RunFlow = Miner->GetWorld() ? Miner->GetWorld()->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
	{
		BoundRunFlow = RunFlow;
		RunFlow->OnOreChanged.AddUniqueDynamic(this, &URRIntermissionShopSubsystem::HandleOreChanged);
	}
	DrawSlots({});
	UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: shop opened, balance %d ore, reroll cost %d."), GetOreBalance(), GetRerollCost());
	OnShopChanged.Broadcast();
	return true;
}

void URRIntermissionShopSubsystem::CloseShop()
{
	if (URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get())
	{
		RunFlow->OnOreChanged.RemoveDynamic(this, &URRIntermissionShopSubsystem::HandleOreChanged);
	}
	BoundRunFlow.Reset();
	ShopMiner.Reset();
	Slots.Reset();
	RerollCount = 0;
	const bool bWasOpen = bShopOpen;
	bShopOpen = false;
	if (bWasOpen)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: shop closed."));
		OnShopChanged.Broadcast();
	}
}

void URRIntermissionShopSubsystem::DrawSlots(const TArray<FRRUpgradeCard>& Excluded)
{
	Slots.Reset();
	ARRMiner* Miner = ShopMiner.Get();
	URRUpgradeComponent* Upgrades = Miner ? Miner->GetUpgrades() : nullptr;
	if (!Upgrades)
	{
		return;
	}
	const int32 CardCount = FMath::Max(1, GetDefault<URRShopSettings>()->ShopCardCount);
	TArray<FString> Parts;
	for (const FRRUpgradeCard& Card : Upgrades->DrawStatCardsWithOwnerLuck(CardCount, Excluded))
	{
		FRRShopSlot& Slot = Slots.AddDefaulted_GetRef();
		Slot.Card = Card;
		Slot.Price = GetPriceForTier(Card.Tier);
		Parts.Add(DescribeShopSlot(Slot));
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: offer of %d/%d cards: %s."), Slots.Num(), CardCount, *FString::Join(Parts, TEXT(", ")));
}

bool URRIntermissionShopSubsystem::GetSlot(int32 SlotIndex, FRRShopSlot& OutSlot) const
{
	if (!Slots.IsValidIndex(SlotIndex))
	{
		OutSlot = FRRShopSlot();
		return false;
	}
	OutSlot = Slots[SlotIndex];
	return true;
}

int32 URRIntermissionShopSubsystem::GetSlotPrice(int32 SlotIndex) const
{
	return Slots.IsValidIndex(SlotIndex) ? Slots[SlotIndex].Price : 0;
}

bool URRIntermissionShopSubsystem::IsSlotEmpty(int32 SlotIndex) const
{
	return !Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].bSold;
}

bool URRIntermissionShopSubsystem::CanBuySlot(int32 SlotIndex) const
{
	const URRRunFlowSubsystem* RunFlow = GetRunFlow();
	return bShopOpen && ShopMiner.IsValid() && RunFlow && !IsSlotEmpty(SlotIndex) && RunFlow->CanSpendOre(Slots[SlotIndex].Price);
}

bool URRIntermissionShopSubsystem::BuySlot(int32 SlotIndex)
{
	if (!CanBuySlot(SlotIndex))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: buy of slot %d refused (shop closed, empty slot or not enough ore: balance %d)."),
			SlotIndex, GetOreBalance());
		return false;
	}
	ARRMiner* Miner = ShopMiner.Get();
	URRUpgradeComponent* Upgrades = Miner->GetUpgrades();
	const FRRShopSlot Bought = Slots[SlotIndex];
	const int32 BalanceBefore = GetOreBalance();
	if (!Upgrades || !Upgrades->ApplyUpgrade(Bought.Card))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRIntermissionShopSubsystem: card of slot %d (%s) refused by the miner, nothing spent."), SlotIndex, *DescribeShopSlot(Bought));
		return false;
	}
	// Affordability was checked above, so the payment succeeds; a failure would be a bug and is logged.
	if (!PayOre(Bought.Price, TEXT("card")))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRIntermissionShopSubsystem: card of slot %d applied but its price %d could not be paid."), SlotIndex, Bought.Price);
	}
	if (URRRunSubsystem* Run = URRRunSubsystem::Get(Miner))
	{
		Run->AppendCardToPendingSnapshot(Miner, Bought.Card);
	}
	Slots[SlotIndex].bSold = true;
	UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: slot %d bought (%s), balance %d."), SlotIndex, *DescribeShopSlot(Bought), GetOreBalance());
	// Purchase effects first (the slot is already sold), then the generic refresh.
	UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: OnSlotBought slot %d, price %d, balance before %d."), SlotIndex, Bought.Price, BalanceBefore);
	OnSlotBought.Broadcast(SlotIndex, Bought.Price, BalanceBefore);
	OnShopChanged.Broadcast();
	return true;
}

int32 URRIntermissionShopSubsystem::GetOreBalance() const
{
	const URRRunFlowSubsystem* RunFlow = GetRunFlow();
	return bShopOpen && RunFlow ? RunFlow->GetSpendableOre() : 0;
}

int32 URRIntermissionShopSubsystem::GetRerollCost() const
{
	return URRRerollCostLibrary::GetRerollCostForCount(RerollCount);
}

bool URRIntermissionShopSubsystem::CanReroll() const
{
	const URRRunFlowSubsystem* RunFlow = GetRunFlow();
	return bShopOpen && ShopMiner.IsValid() && RunFlow && RunFlow->CanSpendOre(GetRerollCost());
}

bool URRIntermissionShopSubsystem::RequestReroll()
{
	if (!CanReroll())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: reroll refused (shop closed or not enough ore: cost %d, balance %d)."), GetRerollCost(), GetOreBalance());
		return false;
	}
	const int32 Cost = GetRerollCost();
	if (!PayOre(Cost, TEXT("reroll")))
	{
		return false;
	}
	++RerollCount;
	TArray<FRRUpgradeCard> Replaced;
	for (const FRRShopSlot& Slot : Slots)
	{
		if (!Slot.bSold)
		{
			Replaced.Add(Slot.Card);
		}
	}
	DrawSlots(Replaced);
	UE_LOG(LogRogueRigs, Log, TEXT("URRIntermissionShopSubsystem: reroll %d paid %d, next cost %d, balance %d."), RerollCount, Cost, GetRerollCost(), GetOreBalance());
	OnShopChanged.Broadcast();
	return true;
}

int32 URRIntermissionShopSubsystem::GetPriceForTier(ERRUpgradeRarity Tier)
{
	return GetDefault<URRShopSettings>()->GetPriceForTier(Tier);
}

URRRunFlowSubsystem* URRIntermissionShopSubsystem::GetRunFlow() const
{
	return bShopOpen ? BoundRunFlow.Get() : nullptr;
}

bool URRIntermissionShopSubsystem::PayOre(int32 Amount, const TCHAR* Reason)
{
	URRRunFlowSubsystem* RunFlow = GetRunFlow();
	if (Amount <= 0)
	{
		return true;
	}
	// The caller broadcasts OnShopChanged once its state is final; the OnOreChanged of this payment is not relayed.
	TGuardValue<bool> PayingGuard(bPaying, true);
	if (!RunFlow || !RunFlow->SpendOre(Amount))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRIntermissionShopSubsystem: %s payment of %d ore refused."), Reason, Amount);
		return false;
	}
	return true;
}

void URRIntermissionShopSubsystem::HandleOreChanged(int32 CarriedOre, int32 SecuredOre)
{
	if (bShopOpen && !bPaying)
	{
		OnShopChanged.Broadcast();
	}
}
