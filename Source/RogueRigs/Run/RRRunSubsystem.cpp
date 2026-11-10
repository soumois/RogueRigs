// Rogue Rigs - run state across the 4 levels of a run: level index, layout list, miner snapshot carried over a map travel.

#include "Run/RRRunSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Async/Async.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Misc/Guid.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RROreBankSubsystem.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "Run/RRLayoutProvider.h"
#include "Run/RRRunSettings.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"

namespace
{
#if !UE_BUILD_SHIPPING
	/** Debug: generation of this level skipped as failed so the hand layout fallback runs (0 = off, -1 = every level). */
	TAutoConsoleVariable<int32> CVarForceGenFail(
		TEXT("rr.Layout.ForceGenFail"),
		0,
		TEXT("rr.Layout.ForceGenFail <level>: the run layout generator treats that level (-1: every level) as failed, so it falls back to its hand layout JSON. 0 = off."),
		ECVF_Cheat);
#endif

	/** The run's layout provider from the settings: the C++ generator (hand JSON fallback), or the hand JSON only. */
	TSharedPtr<IRRLayoutProvider> MakeDefaultLayoutProvider()
	{
		if (GetDefault<URRRunSettings>()->bGenerateRunLayouts)
		{
			return MakeShared<FRRGeneratorLayoutProvider>();
		}
		return MakeShared<FRRJsonLayoutProvider>();
	}

	/** A new random run seed in [1, 999999999] (task-496 Q4: random for every run, Retry included). */
	int32 DrawRunSeed()
	{
		return 1 + static_cast<int32>(GetTypeHash(FGuid::NewGuid()) % 999999999u);
	}

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

void URRRunSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LayoutProvider = MakeDefaultLayoutProvider();
}

void URRRunSubsystem::Deinitialize()
{
	if (bLayoutPending && LayoutTask.IsValid())
	{
		LayoutTask.Wait();
	}
	if (PendingFrameTicker.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(PendingFrameTicker);
		PendingFrameTicker.Reset();
	}
	ClearLayoutRequest();
	Super::Deinitialize();
}

bool URRRunSubsystem::RequestLayout(int32 InLevelIndex)
{
	const FString Name = GetLayoutNameForLevel(InLevelIndex);
	if (Name.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: no layout name for level %d, nothing requested."), InLevelIndex);
		return false;
	}
	return RequestLayoutByName(InLevelIndex, Name);
}

bool URRRunSubsystem::RequestLayoutByName(int32 InLevelIndex, const FString& InLayoutName)
{
	if (InLayoutName.IsEmpty())
	{
		return false;
	}
	ClearLayoutRequest();
	if (!LayoutProvider)
	{
		LayoutProvider = MakeDefaultLayoutProvider();
	}
	if (RunSeed == 0)
	{
		RunSeed = DrawRunSeed();
	}

	const uint32 Serial = ++RequestSerial;
	RequestedLevelIndex = InLevelIndex;
	RequestedLayoutName = InLayoutName;
	bLayoutPending = true;
	RequestStartSeconds = FPlatformTime::Seconds();

	FRRLayoutRequest Request;
	Request.LevelIndex = InLevelIndex;
	Request.LayoutName = InLayoutName;
	Request.RunSeed = RunSeed;
	Request.LevelSeed = DeriveLevelSeed(RunSeed, InLevelIndex);
#if !UE_BUILD_SHIPPING
	const int32 ForceFail = CVarForceGenFail.GetValueOnGameThread();
	Request.bForceGeneratorFailure = ForceFail == -1 || (ForceFail > 0 && ForceFail == InLevelIndex);
#endif
	const TSharedRef<IRRLayoutProvider> Provider = LayoutProvider.ToSharedRef();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: layout requested for level %d: %s (provider %s, run seed %d, level seed %d%s, worker thread)."),
		InLevelIndex, *InLayoutName, *Provider->GetProviderName(), Request.RunSeed, Request.LevelSeed,
		Request.bForceGeneratorFailure ? TEXT(", generation forced to fail") : TEXT(""));

	// Game-thread frames while the worker builds (task-496 A2): the longest one is logged with the result.
	LongestPendingFrameMs = 0.0;
	PendingFrames = 0;
	TWeakObjectPtr<URRRunSubsystem> WeakThis(this);
	if (!PendingFrameTicker.IsValid())
	{
		PendingFrameTicker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float DeltaTime)
		{
			URRRunSubsystem* Run = WeakThis.Get();
			if (!Run || !Run->bLayoutPending)
			{
				if (Run)
				{
					Run->PendingFrameTicker.Reset();
				}
				return false;
			}
			// The first tick closes the frame of the request itself: not counted.
			// Frame 1 is the first engine frame after the start (the menu request at startup is made in frame 0): its 130-150 ms
			// are the engine start, measured the same with a 0.6 s or a 4.4 s generation (task-496 menu runs); logged, not counted.
			if (Run->PendingFrames++ > 0 && GFrameCounter <= 1)
			{
				UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: first engine frame %.1f ms (frame %llu, engine start) while the level %d layout is built on a worker; not counted."),
					DeltaTime * 1000.0, GFrameCounter, Run->RequestedLevelIndex);
			}
			else if (Run->PendingFrames > 1)
			{
				Run->LongestPendingFrameMs = FMath::Max(Run->LongestPendingFrameMs, DeltaTime * 1000.0);
				if (DeltaTime > 0.1f)
				{
					UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: game-thread frame of %.1f ms (frame %llu) while the level %d layout is built on a worker."),
						DeltaTime * 1000.0, GFrameCounter, Run->RequestedLevelIndex);
				}
			}
			return true;
		}));
	}

	// The worker only touches the provider and the request copy; the result is handed to the game thread by serial, so a
	// request replaced or cleared meanwhile is simply dropped there.
	LayoutTask = UE::Tasks::Launch(UE_SOURCE_LOCATION, [Provider, Request, WeakThis, Serial]() -> TSharedPtr<FRRLayoutData>
	{
		TSharedPtr<FRRLayoutData> Layout = Provider->BuildLayout(Request);
		AsyncTask(ENamedThreads::GameThread, [WeakThis, Serial]()
		{
			if (URRRunSubsystem* Run = WeakThis.Get())
			{
				Run->CompleteLayoutRequest(Serial);
			}
		});
		return Layout;
	});
	return true;
}

void URRRunSubsystem::CompleteLayoutRequest(uint32 Serial)
{
	check(IsInGameThread());
	if (Serial != RequestSerial || !bLayoutPending || !LayoutTask.IsValid())
	{
		return;
	}
	// The game-thread callback is queued from inside the task body: wait for the body's return (immediate in practice).
	LayoutTask.Wait();
	TSharedPtr<FRRLayoutData> Layout = LayoutTask.GetResult();
	LayoutTask = {};
	bLayoutPending = false;
	PreparedLayout = Layout;

	const double Milliseconds = (FPlatformTime::Seconds() - RequestStartSeconds) * 1000.0;
	const bool bSuccess = Layout.IsValid() && Layout->IsValid();
	if (bSuccess)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: layout ready for level %d: %s (%s), %d x %d, spawn (%d, %d), %.1f ms after the request; game thread meanwhile: %d frame(s), longest %.1f ms."),
			RequestedLevelIndex, *RequestedLayoutName, *Layout->Source, Layout->Width, Layout->Height, Layout->SpawnCell.X, Layout->SpawnCell.Y, Milliseconds,
			FMath::Max(0, PendingFrames - 1), LongestPendingFrameMs);
	}
	else
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunSubsystem: layout for level %d (%s) failed: %s"),
			RequestedLevelIndex, *RequestedLayoutName, Layout.IsValid() ? *Layout->Error : TEXT("no result"));
	}
	OnLayoutReady.Broadcast(RequestedLevelIndex, RequestedLayoutName, bSuccess);
}

void URRRunSubsystem::ClearLayoutRequest()
{
	++RequestSerial;
	RequestedLevelIndex = 0;
	RequestedLayoutName.Reset();
	bLayoutPending = false;
	LayoutTask = {};
	PreparedLayout.Reset();
}

bool URRRunSubsystem::IsLayoutReady(int32 InLevelIndex) const
{
	return GetPreparedLayout(InLevelIndex).IsValid();
}

bool URRRunSubsystem::IsLayoutPending(int32 InLevelIndex) const
{
	return bLayoutPending && InLevelIndex > 0 && RequestedLevelIndex == InLevelIndex;
}

TSharedPtr<const FRRLayoutData> URRRunSubsystem::GetPreparedLayout(int32 InLevelIndex) const
{
	if (bLayoutPending || InLevelIndex <= 0 || RequestedLevelIndex != InLevelIndex || !PreparedLayout.IsValid() || !PreparedLayout->IsValid())
	{
		return nullptr;
	}
	return PreparedLayout;
}

TSharedPtr<const FRRLayoutData> URRRunSubsystem::WaitForPreparedLayout(int32 InLevelIndex)
{
	if (IsLayoutPending(InLevelIndex))
	{
		CompleteLayoutRequest(RequestSerial);
	}
	return GetPreparedLayout(InLevelIndex);
}

void URRRunSubsystem::SetLayoutProvider(TSharedPtr<IRRLayoutProvider> InProvider)
{
	LayoutProvider = InProvider ? InProvider : MakeDefaultLayoutProvider();
}

int32 URRRunSubsystem::DeriveLevelSeed(int32 InRunSeed, int32 InLevelIndex)
{
	// Integer mix of (run seed, level): stable across platforms and builds; kept under 1e9 so the search (seed + k) cannot overflow.
	uint32 Value = static_cast<uint32>(InRunSeed) * 0x9E3779B1u ^ static_cast<uint32>(InLevelIndex) * 0x85EBCA77u;
	Value ^= Value >> 16;
	Value *= 0x7FEB352Du;
	Value ^= Value >> 15;
	Value *= 0x846CA68Bu;
	Value ^= Value >> 16;
	return 1 + static_cast<int32>(Value % 999999000u);
}

bool URRRunSubsystem::PrepareNewRunLayout()
{
	if (bRunInProgress)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: new run preparation refused (a run is in progress)."));
		return false;
	}
	RunSeed = DrawRunSeed();
	bNewRunPrepared = true;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: next run prepared: run seed %d, level 1 layout requested now."), RunSeed);
	if (!RequestLayout(1))
	{
		bNewRunPrepared = false;
		return false;
	}
	return true;
}

void URRRunSubsystem::BeginRun(const FString& Level1LayoutName)
{
	// A prepared next run (PrepareNewRunLayout) keeps its seed and its level 1 request; otherwise a new seed, nothing requested.
	const bool bPrepared = HasNewRunLayout();
	bNewRunPrepared = false;
	bRunInProgress = true;
	LevelIndex = 1;
	Level1LayoutOverride = Level1LayoutName;
	LastSnapshot = FRRRunMinerSnapshot();
	bSnapshotPending = false;
	CaptureWorld.Reset();
	if (!bPrepared)
	{
		ClearLayoutRequest();
		RunSeed = DrawRunSeed();
	}
	const FString Layout = GetCurrentLayoutName();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: run started, level 1/%d on layout %s, run seed %d (%s)."), GetLevelCount(), *Layout, RunSeed,
		!bPrepared ? TEXT("new seed, level 1 not prepared: the grid's own layout")
		: (bLayoutPending ? TEXT("prepared, level 1 layout pending") : (PreparedLayout.IsValid() && PreparedLayout->IsValid() ? TEXT("prepared, level 1 layout ready") : TEXT("prepared, level 1 layout failed"))));
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
	// Recap total = the balance held after this extraction, every purchase and reroll already subtracted (task-391-04).
	Snapshot.RunSecuredOre = GetBankSecuredOre();
	if (const URRRunFlowSubsystem* RunFlow = Miner->GetWorld() ? Miner->GetWorld()->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
	{
		Snapshot.OreSecuredThisLevel = RunFlow->GetExtractionSecuredOre();
		Snapshot.CarriedOreAtCapture = RunFlow->GetCarriedOre();
	}
	if (Snapshot.CarriedOreAtCapture > 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: %d carried ore at capture is not carried over (secure it with EndRun(true) first)."), Snapshot.CarriedOreAtCapture);
	}

	LastSnapshot = Snapshot;
	bSnapshotPending = true;
	CaptureWorld = Miner->GetWorld();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: snapshot of level %d: miner level %d XP %.2f, health %.1f/%.1f, weapons [%s], %d cards [%s], ore secured this level %d, bank balance %d."),
		LevelIndex, Snapshot.MinerLevel, Snapshot.MinerXP, Snapshot.Health, Snapshot.MaxHealth, *DescribeWeaponClasses(Snapshot.WeaponClasses),
		Snapshot.PickedCards.Num(), *DescribeCards(Snapshot.PickedCards), Snapshot.OreSecuredThisLevel, Snapshot.RunSecuredOre);
	OnLevelSnapshotCaptured.Broadcast(LastSnapshot);
	return true;
}

bool URRRunSubsystem::AppendCardToPendingSnapshot(ARRMiner* Miner, const FRRUpgradeCard& Card)
{
	if (!bRunInProgress || !bSnapshotPending || !LastSnapshot.bValid || !Miner)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunSubsystem: card not added to the snapshot (no run, no pending snapshot or no miner)."));
		return false;
	}
	FRRRunPickedCard& Picked = LastSnapshot.PickedCards.AddDefaulted_GetRef();
	Picked.Card = Card;
	Picked.TargetWeaponClass = Card.TargetWeapon ? Card.TargetWeapon->GetClass() : nullptr;
	Picked.Card.TargetWeapon = nullptr;
	if (const URRHealthAttributeSet* HealthSet = Miner->GetHealthSet())
	{
		LastSnapshot.Health = HealthSet->GetHealth();
		LastSnapshot.MaxHealth = HealthSet->GetMaxHealth();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: card %s added to the level %d snapshot (%d cards), health %.1f/%.1f."),
		*DescribeCards({ Picked }), LastSnapshot.LevelIndex, LastSnapshot.PickedCards.Num(), LastSnapshot.Health, LastSnapshot.MaxHealth);
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
	const FString Layout = GetCurrentLayoutName();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: advanced to level %d/%d, layout %s, snapshot %s."),
		LevelIndex, GetLevelCount(), *Layout, bSnapshotPending ? TEXT("pending") : TEXT("none"));
	// The next terrain is generated at once (task-496 Q2): AdvanceToNextLevel runs in the frame of the miner's contact with the
	// extractor board zone (ARRShuttle::TryBoard -> EndRun(true) -> HandleLevelExtracted); the terrain swap stays behind the cards screen.
	RequestLayout(LevelIndex);
	OnLevelAdvanced.Broadcast(LevelIndex, Layout);
	return true;
}

void URRRunSubsystem::ResetRun()
{
	// A next run already prepared and not begun (Main Menu then New Run) keeps its seed and level 1 layout; otherwise a new one.
	const bool bKeepPrepared = HasNewRunLayout();
	bRunInProgress = false;
	LevelIndex = 0;
	Level1LayoutOverride.Reset();
	LastSnapshot = FRRRunMinerSnapshot();
	bSnapshotPending = false;
	CaptureWorld.Reset();
	if (!bKeepPrepared)
	{
		ClearLayoutRequest();
		PrepareNewRunLayout();
	}
	else
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: run reset keeps the prepared next run (run seed %d, level 1 layout %s)."),
			RunSeed, bLayoutPending ? TEXT("pending") : TEXT("ready or failed"));
	}
	// Ore lasts one run (ore-economy, task-340): the secured bank starts the next run at 0.
	const UGameInstance* GameInstance = GetGameInstance();
	if (URROreBankSubsystem* Bank = GameInstance ? GameInstance->GetSubsystem<URROreBankSubsystem>() : nullptr)
	{
		Bank->ResetSecuredOre();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: run reset; the next map begins level 1, secured ore %d."), GetBankSecuredOre());
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

bool URRRunSubsystem::ConsumeSnapshotInPlace(ARRMiner* Miner)
{
	if (!Miner || !bSnapshotPending || !LastSnapshot.bValid)
	{
		return false;
	}
	bSnapshotPending = false;
	HandledWorld = Miner->GetWorld();
	const URRProgressionComponent* Progression = Miner->GetProgression();
	const URRHealthAttributeSet* HealthSet = Miner->GetHealthSet();
	const URRUpgradeComponent* Upgrades = Miner->GetUpgrades();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunSubsystem: snapshot kept in place on level %d by %s: miner level %d XP %.2f, health %.1f/%.1f, weapons [%s], %d cards picked (snapshot: level %d XP %.2f, health %.1f/%.1f, weapons [%s], %d cards), bank %d."),
		LevelIndex, *Miner->GetName(), Progression ? Progression->GetLevel() : 0, Progression ? Progression->GetXP() : 0.f,
		HealthSet ? HealthSet->GetHealth() : 0.f, HealthSet ? HealthSet->GetMaxHealth() : 0.f, *DescribeWeaponClasses(GetWeaponClasses(*Miner)),
		Upgrades ? Upgrades->GetPickedUpgrades().Num() : 0, LastSnapshot.MinerLevel, LastSnapshot.MinerXP, LastSnapshot.Health, LastSnapshot.MaxHealth,
		*DescribeWeaponClasses(LastSnapshot.WeaponClasses), LastSnapshot.PickedCards.Num(), GetBankSecuredOre());
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

int32 URRRunSubsystem::GetBankSecuredOre() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	const URROreBankSubsystem* Bank = GameInstance ? GameInstance->GetSubsystem<URROreBankSubsystem>() : nullptr;
	return Bank ? Bank->GetSecuredOre() : 0;
}
