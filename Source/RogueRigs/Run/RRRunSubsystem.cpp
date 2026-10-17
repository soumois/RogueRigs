// Rogue Rigs - run state across the 4 levels of a run: level index, layout list, miner snapshot carried over a map travel.

#include "Run/RRRunSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RROreBankSubsystem.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "Run/RRRunSettings.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"

namespace
{
	/** Short log list of the snapshot cards: "Stat/Target" or "NewWeapon:<Class>". */
	FString DescribeCards(const TArray<FRRRunPickedCard>& Cards)
	{
		TArray<FString> Parts;
		for (const FRRRunPickedCard& Picked : Cards)
		{
			if (Picked.Card.IsNewWeapon())
			{
				Parts.Add(FString::Printf(TEXT("NewWeapon:%s"), *GetNameSafe(Picked.Card.WeaponClass.Get())));
				continue;
			}
			const FString Stat = StaticEnum<ERRUpgradeStat>()->GetNameStringByValue(static_cast<int64>(Picked.Card.Stat));
			const FString Target = Picked.Card.Target == ERRUpgradeTarget::Weapon ? GetNameSafe(Picked.TargetWeaponClass.Get())
				: StaticEnum<ERRUpgradeTarget>()->GetNameStringByValue(static_cast<int64>(Picked.Card.Target));
			Parts.Add(FString::Printf(TEXT("%s/%s %.3f"), *Stat, *Target, Picked.Card.Value));
		}
		return FString::Join(Parts, TEXT(", "));
	}

	/** Short log list of weapon classes. */
	FString DescribeWeaponClasses(const TArray<TSubclassOf<ARRWeapon>>& Classes)
	{
		TArray<FString> Names;
		for (const TSubclassOf<ARRWeapon>& Class : Classes)
		{
			Names.Add(GetNameSafe(Class.Get()));
		}
		return FString::Join(Names, TEXT(", "));
	}

	/** Exact classes of the miner's valid weapons, in acquisition order. */
	TArray<TSubclassOf<ARRWeapon>> GetWeaponClasses(const ARRMiner& Miner)
	{
		TArray<TSubclassOf<ARRWeapon>> Classes;
		for (const TObjectPtr<ARRWeapon>& Weapon : Miner.GetWeapons())
		{
			if (Weapon)
			{
				Classes.Add(Weapon->GetClass());
			}
		}
		return Classes;
	}

	/** First valid weapon of Miner whose exact class is Class, or null. */
	ARRWeapon* FindWeaponOfClass(const ARRMiner& Miner, const UClass* Class)
	{
		for (const TObjectPtr<ARRWeapon>& Weapon : Miner.GetWeapons())
		{
			if (Weapon && Weapon->GetClass() == Class)
			{
				return Weapon;
			}
		}
		return nullptr;
	}
}

URRRunSubsystem* URRRunSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext && GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URRRunSubsystem>() : nullptr;
}

void URRRunSubsystem::BeginRun(const FString& Level1LayoutName)
{
	bRunInProgress = true;
	LevelIndex = 1;
	Level1LayoutOverride = Level1LayoutName;
	LastSnapshot = FRRRunMinerSnapshot();
	bSnapshotPending = false;
	CaptureWorld.Reset();
	SecuredOreAtRunStart = GetBankSecuredOre();
	SecuredOreAtLevelStart = SecuredOreAtRunStart;
	const FString Layout = GetCurrentLayoutName();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: run started, level 1/%d on layout %s."), GetLevelCount(), *Layout);
	OnRunStarted.Broadcast(LevelIndex, Layout);
}

bool URRRunSubsystem::CaptureLevelEndSnapshot(ARRMiner* Miner)
{
	if (!bRunInProgress || !Miner)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: snapshot refused (%s)."), bRunInProgress ? TEXT("no miner") : TEXT("no run in progress"));
		return false;
	}

	FRRRunMinerSnapshot Snapshot;
	Snapshot.bValid = true;
	Snapshot.LevelIndex = LevelIndex;
	if (const URRUpgradeComponent* Upgrades = Miner->GetUpgrades())
	{
		for (const FRRUpgradeCard& Card : Upgrades->GetPickedUpgrades())
		{
			FRRRunPickedCard& Picked = Snapshot.PickedCards.AddDefaulted_GetRef();
			Picked.Card = Card;
			Picked.TargetWeaponClass = Card.TargetWeapon ? Card.TargetWeapon->GetClass() : nullptr;
			Picked.Card.TargetWeapon = nullptr;
		}
	}
	Snapshot.WeaponClasses = GetWeaponClasses(*Miner);
	if (const URRProgressionComponent* Progression = Miner->GetProgression())
	{
		Snapshot.MinerLevel = Progression->GetLevel();
		Snapshot.MinerXP = Progression->GetXP();
	}
	if (const URRHealthAttributeSet* HealthSet = Miner->GetHealthSet())
	{
		Snapshot.Health = HealthSet->GetHealth();
		Snapshot.MaxHealth = HealthSet->GetMaxHealth();
	}
	const int32 BankOre = GetBankSecuredOre();
	Snapshot.OreSecuredThisLevel = BankOre - SecuredOreAtLevelStart;
	Snapshot.RunSecuredOre = BankOre - SecuredOreAtRunStart;
	if (const URRRunFlowSubsystem* RunFlow = Miner->GetWorld() ? Miner->GetWorld()->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
	{
		Snapshot.CarriedOreAtCapture = RunFlow->GetCarriedOre();
	}
	if (Snapshot.CarriedOreAtCapture > 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: %d carried ore at capture is not carried over (secure it with EndRun(true) first)."), Snapshot.CarriedOreAtCapture);
	}

	LastSnapshot = Snapshot;
	bSnapshotPending = true;
	CaptureWorld = Miner->GetWorld();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: snapshot of level %d: miner level %d XP %.2f, health %.1f/%.1f, weapons [%s], %d cards [%s], ore secured this level %d, run %d, bank %d."),
		LevelIndex, Snapshot.MinerLevel, Snapshot.MinerXP, Snapshot.Health, Snapshot.MaxHealth, *DescribeWeaponClasses(Snapshot.WeaponClasses),
		Snapshot.PickedCards.Num(), *DescribeCards(Snapshot.PickedCards), Snapshot.OreSecuredThisLevel, Snapshot.RunSecuredOre, BankOre);
	OnLevelSnapshotCaptured.Broadcast(LastSnapshot);
	return true;
}

bool URRRunSubsystem::AdvanceToNextLevel()
{
	if (!bRunInProgress || IsLastLevel())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: advance refused (%s)."), bRunInProgress ? TEXT("last level") : TEXT("no run in progress"));
		return false;
	}
	++LevelIndex;
	SecuredOreAtLevelStart = GetBankSecuredOre();
	const FString Layout = GetCurrentLayoutName();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: advanced to level %d/%d, layout %s, snapshot %s."),
		LevelIndex, GetLevelCount(), *Layout, bSnapshotPending ? TEXT("pending") : TEXT("none"));
	OnLevelAdvanced.Broadcast(LevelIndex, Layout);
	return true;
}

void URRRunSubsystem::ResetRun()
{
	bRunInProgress = false;
	LevelIndex = 0;
	Level1LayoutOverride.Reset();
	LastSnapshot = FRRRunMinerSnapshot();
	bSnapshotPending = false;
	CaptureWorld.Reset();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: run reset; the next map begins level 1."));
	OnRunReset.Broadcast();
}

void URRRunSubsystem::HandleMinerBeginPlay(ARRMiner* Miner)
{
	UWorld* World = Miner ? Miner->GetWorld() : nullptr;
	if (!World || HandledWorld.Get() == World)
	{
		return;
	}
	HandledWorld = World;

	if (!bRunInProgress)
	{
		const ARRArenaGrid* Grid = RRWorld::FindArenaGrid(World);
		BeginRun(Grid ? Grid->GetLayoutName() : FString());
		return;
	}
	if (bSnapshotPending && CaptureWorld.Get() != World)
	{
		bSnapshotPending = false;
		ApplySnapshotToMiner(Miner, LastSnapshot);
	}
}

bool URRRunSubsystem::ApplySnapshotToMiner(ARRMiner* Miner, const FRRRunMinerSnapshot& Snapshot)
{
	if (!Miner || !Snapshot.bValid)
	{
		return false;
	}

	if (URRProgressionComponent* Progression = Miner->GetProgression())
	{
		Progression->RestoreProgress(Snapshot.MinerLevel, Snapshot.MinerXP);
	}

	// Replaying the picks in order rebuilds the same GE stacks: a NewWeapon card acquires its weapon (which replays the
	// all-weapons cards picked before it), a weapon card targets the owned weapon of the recorded class.
	int32 Replayed = 0;
	if (URRUpgradeComponent* Upgrades = Miner->GetUpgrades())
	{
		for (const FRRRunPickedCard& Picked : Snapshot.PickedCards)
		{
			FRRUpgradeCard Card = Picked.Card;
			if (!Card.IsNewWeapon() && Card.Target == ERRUpgradeTarget::Weapon)
			{
				Card.TargetWeapon = FindWeaponOfClass(*Miner, Picked.TargetWeaponClass.Get());
			}
			if (Upgrades->ApplyUpgrade(Card))
			{
				++Replayed;
			}
			else
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: card %s could not be replayed."), *DescribeCards({ Picked }));
			}
		}
	}

	// Weapons acquired outside the cards (none today) come back after the replay.
	for (const TSubclassOf<ARRWeapon>& WeaponClass : Snapshot.WeaponClasses)
	{
		if (WeaponClass && !FindWeaponOfClass(*Miner, WeaponClass.Get()))
		{
			Miner->AddWeapon(WeaponClass);
		}
	}

	if (UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent())
	{
		if (const URRHealthAttributeSet* HealthSet = Miner->GetHealthSet())
		{
			const float NewHealth = FMath::Clamp(Snapshot.Health, 1.f, HealthSet->GetMaxHealth());
			ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), NewHealth);
		}
	}

	const URRProgressionComponent* Progression = Miner->GetProgression();
	const URRHealthAttributeSet* HealthSet = Miner->GetHealthSet();
	const URRUpgradeComponent* Upgrades = Miner->GetUpgrades();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: snapshot applied on level %d to %s: miner level %d XP %.2f, health %.1f/%.1f, weapons [%s], %d/%d cards replayed (%d picked), bank %d."),
		LevelIndex, *Miner->GetName(), Progression ? Progression->GetLevel() : 0, Progression ? Progression->GetXP() : 0.f,
		HealthSet ? HealthSet->GetHealth() : 0.f, HealthSet ? HealthSet->GetMaxHealth() : 0.f, *DescribeWeaponClasses(GetWeaponClasses(*Miner)),
		Replayed, Snapshot.PickedCards.Num(), Upgrades ? Upgrades->GetPickedUpgrades().Num() : 0, GetBankSecuredOre());
	OnSnapshotApplied.Broadcast(Miner, LevelIndex);
	return true;
}

int32 URRRunSubsystem::GetLevelCount() const
{
	return FMath::Max(1, GetDefault<URRRunSettings>()->LevelLayouts.Num());
}

FString URRRunSubsystem::GetLayoutNameForLevel(int32 InLevelIndex) const
{
	if (InLevelIndex == 1 && !Level1LayoutOverride.IsEmpty())
	{
		return Level1LayoutOverride;
	}
	const TArray<FString>& Layouts = GetDefault<URRRunSettings>()->LevelLayouts;
	return Layouts.IsValidIndex(InLevelIndex - 1) ? Layouts[InLevelIndex - 1] : FString();
}

int32 URRRunSubsystem::GetRunSecuredOre() const
{
	return bRunInProgress ? GetBankSecuredOre() - SecuredOreAtRunStart : 0;
}

int32 URRRunSubsystem::GetBankSecuredOre() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	const URROreBankSubsystem* Bank = GameInstance ? GameInstance->GetSubsystem<URROreBankSubsystem>() : nullptr;
	return Bank ? Bank->GetSecuredOre() : 0;
}
