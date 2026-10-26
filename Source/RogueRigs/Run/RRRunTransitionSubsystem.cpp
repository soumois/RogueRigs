// Rogue Rigs - level-to-level transition of a run: intermission after an extraction, Continue, map travel and the loading hold
// until the next level is ready (task-206-03).

#include "Run/RRRunTransitionSubsystem.h"

#include "Enemies/RREnemySpawner.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Grid/RRArenaGrid.h"
#include "HAL/PlatformTime.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Run/RRIntermissionShopSubsystem.h"
#include "Run/RRRunSettings.h"
#include "Run/RRRunSubsystem.h"
#include "RunLog/RRRunLogSubsystem.h"
#include "UObject/UObjectGlobals.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"

URRRunTransitionSubsystem* URRRunTransitionSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext && GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URRRunTransitionSubsystem>() : nullptr;
}

void URRRunTransitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<URRRunSubsystem>();
	Super::Initialize(Collection);
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URRRunTransitionSubsystem::HandlePostLoadMap);
	if (URRRunSubsystem* Run = GetRun())
	{
		Run->OnRunReset.AddUniqueDynamic(this, &URRRunTransitionSubsystem::HandleRunReset);
	}
}

void URRRunTransitionSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	if (URRRunSubsystem* Run = GetRun())
	{
		Run->OnRunReset.RemoveDynamic(this, &URRRunTransitionSubsystem::HandleRunReset);
	}
	ClearTransition();
	Super::Deinitialize();
}

URRRunSubsystem* URRRunTransitionSubsystem::GetRun() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetSubsystem<URRRunSubsystem>() : nullptr;
}

URRIntermissionShopSubsystem* URRRunTransitionSubsystem::GetShop() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetSubsystem<URRIntermissionShopSubsystem>() : nullptr;
}

bool URRRunTransitionSubsystem::HandleLevelExtracted(ARRMiner* Miner)
{
	URRRunSubsystem* Run = GetRun();
	if (!Run || !Run->IsRunInProgress() || Phase != ERRRunTransitionPhase::None)
	{
		return false;
	}
	UWorld* World = Miner ? Miner->GetWorld() : nullptr;
	Recap = FRRIntermissionRecap();
	Recap.FinishedLevelIndex = Run->GetLevelIndex();
	Recap.LevelCount = Run->GetLevelCount();

	if (Run->IsLastLevel())
	{
		// Last level (task-317-01): stats screen with the run won flag; its button leads to the run won end screen, no shop.
		bRunWon = true;
		Recap.bRunWon = true;
		Recap.OreSecuredThisLevel = Run->GetLevelSecuredOre();
		Recap.RunSecuredOre = Run->GetRunSecuredOre();
		MapToOpen.Reset();
	}
	else
	{
		if (!Run->CaptureLevelEndSnapshot(Miner) || !Run->AdvanceToNextLevel())
		{
			return false;
		}
		const FRRRunMinerSnapshot& Snapshot = Run->GetLastSnapshot();
		Recap.NextLevelIndex = Run->GetLevelIndex();
		Recap.OreSecuredThisLevel = Snapshot.OreSecuredThisLevel;
		Recap.RunSecuredOre = Snapshot.RunSecuredOre;
		Recap.NextLayoutName = Run->GetCurrentLayoutName();
		const FString& ConfiguredMap = GetDefault<URRRunSettings>()->ArenaMapName;
		MapToOpen = !ConfiguredMap.IsEmpty() ? ConfiguredMap : (World ? UGameplayStatics::GetCurrentLevelName(World, true) : FString());
	}
	// The run log counters still hold the finished level (reset only by the next level's first miner BeginPlay).
	FillCombatRecap(Miner ? static_cast<const UObject*>(Miner) : this);

	IntermissionWorld = World;
	IntermissionMiner = Miner;
	Phase = ERRRunTransitionPhase::Stats;
	if (Recap.bRunWon)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: level %d/%d extracted, run won: stats screen (ore secured this level %d, run %d; %d kills, %d weapons)."),
			Recap.FinishedLevelIndex, Recap.LevelCount, Recap.OreSecuredThisLevel, Recap.RunSecuredOre, Recap.TotalKills, Recap.DamageByWeapon.Num());
	}
	else
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: stats screen after level %d/%d: ore secured this level %d, run %d; %d kills, %d weapons; next level %d on %s (layout %s), map %s."),
			Recap.FinishedLevelIndex, Recap.LevelCount, Recap.OreSecuredThisLevel, Recap.RunSecuredOre, Recap.TotalKills, Recap.DamageByWeapon.Num(),
			Recap.NextLevelIndex, *Recap.NextLayoutName,
			Run->IsLayoutReady(Recap.NextLevelIndex) ? TEXT("ready") : (Run->IsLayoutPending(Recap.NextLevelIndex) ? TEXT("pending") : TEXT("failed")), *MapToOpen);
	}
	OnIntermissionStarted.Broadcast(Recap);
	return true;
}

void URRRunTransitionSubsystem::FillCombatRecap(const UObject* WorldContext)
{
	const URRRunLogSubsystem* RunLog = URRRunLogSubsystem::Get(WorldContext);
	if (!RunLog)
	{
		return;
	}
	const FRRRunLevelStats Stats = RunLog->GetCurrentLevelStats();
	for (const TPair<FString, int32>& Pair : Stats.KillsByRobotType)
	{
		FRRRecapRobotKills& Line = Recap.KillsByRobotType.AddDefaulted_GetRef();
		Line.RobotType = FName(*Pair.Key);
		Line.DisplayName = FText::FromString(Pair.Key);
		Line.Count = Pair.Value;
		Recap.TotalKills += Pair.Value;
	}
	Recap.KillsByRobotType.Sort([](const FRRRecapRobotKills& A, const FRRRecapRobotKills& B) { return A.Count > B.Count; });

	for (const TPair<FString, float>& Pair : Stats.DamageDealtByWeapon)
	{
		FRRRecapWeaponDamage& Line = Recap.DamageByWeapon.AddDefaulted_GetRef();
		Line.WeaponClassName = FName(*Pair.Key);
		Line.Damage = Pair.Value;
		// The run log keys damage by weapon class name; the class is loaded since one of its instances dealt the damage.
		UClass* WeaponClass = FindFirstObject<UClass>(*Pair.Key, EFindFirstObjectOptions::NativeFirst);
		const FText ClassName = WeaponClass && WeaponClass->IsChildOf(ARRWeapon::StaticClass()) ? ARRWeapon::GetClassDisplayName(WeaponClass) : FText::GetEmpty();
		Line.DisplayName = ClassName.IsEmpty() ? FText::FromString(Pair.Key) : ClassName;
	}
	Recap.DamageByWeapon.Sort([](const FRRRecapWeaponDamage& A, const FRRRecapWeaponDamage& B) { return A.Damage > B.Damage; });
}

bool URRRunTransitionSubsystem::NextLevel()
{
	if (Phase != ERRRunTransitionPhase::Stats)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: NextLevel refused (not on the stats screen)."));
		return false;
	}
	if (Recap.bRunWon)
	{
		UWorld* World = IntermissionWorld.Get();
		Phase = ERRRunTransitionPhase::None;
		IntermissionWorld.Reset();
		IntermissionMiner.Reset();
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: stats screen closed, run won end screen."));
		OnRunWonScreenStarted.Broadcast();
		if (URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
		{
			RunFlow->ShowRunWonEndScreen();
		}
		return true;
	}
	Phase = ERRRunTransitionPhase::Shop;
	// The offer is drawn before the shop widget is created on OnShopScreenStarted (run-shop, task-316-01).
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OpenShop(IntermissionMiner.Get());
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: stats screen closed, shop screen before level %d."), Recap.NextLevelIndex);
	OnShopScreenStarted.Broadcast();
	return true;
}

bool URRRunTransitionSubsystem::Continue()
{
	URRRunSubsystem* Run = GetRun();
	if (Phase != ERRRunTransitionPhase::Shop || !Run)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: Continue refused (not on the shop screen)."));
		return false;
	}
	IntermissionMiner.Reset();
	Phase = ERRRunTransitionPhase::Loading;
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->CloseShop();
	}
	LoadingLevelIndex = Run->GetLevelIndex();
	LoadingLayoutName = Run->GetCurrentLayoutName();
	LoadingStartSeconds = FPlatformTime::Seconds();
	LoadingStartFrame = GFrameCounter;
	LoadingFirstPaintFrame = 0;
	bTravelRequested = false;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: Continue: loading started for level %d (layout %s), frame %llu."), LoadingLevelIndex,
		*LoadingLayoutName, LoadingStartFrame);
	OnLoadingStarted.Broadcast(LoadingLevelIndex, LoadingLayoutName);

	if (Run->IsLayoutPending(LoadingLevelIndex))
	{
		SetLoadingStage(ERRRunLoadingStage::WaitingForLayout);
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: layout %s still being prepared, waiting for OnLayoutReady."), *LoadingLayoutName);
		Run->OnLayoutReady.AddUniqueDynamic(this, &URRRunTransitionSubsystem::HandleLayoutReady);
		return true;
	}
	OpenArenaMap();
	return true;
}

void URRRunTransitionSubsystem::HandleLayoutReady(int32 LevelIndex, const FString& LayoutName, bool bSuccess)
{
	if (URRRunSubsystem* Run = GetRun())
	{
		Run->OnLayoutReady.RemoveDynamic(this, &URRRunTransitionSubsystem::HandleLayoutReady);
	}
	if (Phase != ERRRunTransitionPhase::Loading || LoadingStage != ERRRunLoadingStage::WaitingForLayout)
	{
		return;
	}
	OpenArenaMap();
}

void URRRunTransitionSubsystem::OpenArenaMap()
{
	UWorld* World = IntermissionWorld.Get();
	URRRunSubsystem* Run = GetRun();
	if (Run && !Run->IsLayoutReady(LoadingLevelIndex))
	{
		// Provisional decision (task-206-03): the travel goes on, the grid falls back to its own LayoutName with an error.
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunTransitionSubsystem: layout %s of level %d failed to load; the grid will fall back to its own layout."),
			*LoadingLayoutName, LoadingLevelIndex);
	}
	if (!World || MapToOpen.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunTransitionSubsystem: no world or map to open for level %d; loading dropped."), LoadingLevelIndex);
		ClearTransition();
		return;
	}
	SetLoadingStage(ERRRunLoadingStage::OpeningMap);
	TravelWaitStartFrame = GFrameCounter;
	if (IsTravelWaitOver())
	{
		RequestArenaMapTravel();
		return;
	}
	// The old-world loading widget added on OnLoadingStarted is drawn by Slate later in a frame; a travel requested before that
	// frame has been painted shows the frozen game during the blocking load (task-206-06). The core ticker also runs while paused.
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: travel to %s waits for the loading screen paint (frame %llu)."), *MapToOpen, GFrameCounter);
	TWeakObjectPtr<URRRunTransitionSubsystem> WeakThis(this);
	TravelWaitHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float DeltaTime)
	{
		URRRunTransitionSubsystem* Self = WeakThis.Get();
		return Self ? Self->TickTravelWait(DeltaTime) : false;
	}));
}

bool URRRunTransitionSubsystem::TickTravelWait(float DeltaTime)
{
	if (Phase != ERRRunTransitionPhase::Loading || LoadingStage != ERRRunLoadingStage::OpeningMap || bTravelRequested)
	{
		TravelWaitHandle.Reset();
		return false;
	}
	if (!IsTravelWaitOver())
	{
		return true;
	}
	TravelWaitHandle.Reset();
	RequestArenaMapTravel();
	return false;
}

bool URRRunTransitionSubsystem::IsTravelWaitOver() const
{
	if (LoadingFirstPaintFrame != 0 && GFrameCounter > LoadingFirstPaintFrame)
	{
		return true;
	}
	const int32 MaxFrames = FMath::Max(0, GetDefault<URRRunSettings>()->LoadingPaintWaitMaxFrames);
	return GFrameCounter - TravelWaitStartFrame >= static_cast<uint64>(MaxFrames);
}

void URRRunTransitionSubsystem::RequestArenaMapTravel()
{
	UWorld* World = IntermissionWorld.Get();
	if (!World)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunTransitionSubsystem: intermission world gone before the travel of level %d; loading dropped."), LoadingLevelIndex);
		ClearTransition();
		return;
	}
	bTravelRequested = true;
	if (LoadingFirstPaintFrame != 0)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: opening %s for level %d in frame %llu (loading started frame %llu, first loading paint frame %llu; %.1f ms after Continue)."),
			*MapToOpen, LoadingLevelIndex, GFrameCounter, LoadingStartFrame, LoadingFirstPaintFrame, (FPlatformTime::Seconds() - LoadingStartSeconds) * 1000.0);
	}
	else
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: opening %s for level %d in frame %llu (loading started frame %llu, no loading paint after %llu frames; %.1f ms after Continue)."),
			*MapToOpen, LoadingLevelIndex, GFrameCounter, LoadingStartFrame, GFrameCounter - TravelWaitStartFrame, (FPlatformTime::Seconds() - LoadingStartSeconds) * 1000.0);
	}
	// OpenLevel only requests the travel: the engine loads the map at the start of a later frame.
	UGameplayStatics::OpenLevel(World, FName(*MapToOpen));
}

void URRRunTransitionSubsystem::NotifyLoadingScreenPainted(const UObject* Painter)
{
	if (Phase != ERRRunTransitionPhase::Loading || bTravelRequested || LoadingFirstPaintFrame != 0)
	{
		return;
	}
	LoadingFirstPaintFrame = GFrameCounter;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: first loading paint in frame %llu by %s (loading started frame %llu)."),
		LoadingFirstPaintFrame, *GetNameSafe(Painter), LoadingStartFrame);
}

void URRRunTransitionSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	if (Phase != ERRRunTransitionPhase::Loading || !LoadedWorld || !LoadedWorld->IsGameWorld() || LoadedWorld->GetGameInstance() != GetGameInstance()
		|| LoadedWorld == IntermissionWorld.Get())
	{
		return;
	}
	MapLoadedSeconds = FPlatformTime::Seconds();
	HeldWorld = LoadedWorld;
	SetLoadingStage(ERRRunLoadingStage::PreparingLevel);

	const URRRunSubsystem* Run = GetRun();
	const ARRArenaGrid* Grid = RRWorld::FindArenaGrid(LoadedWorld);
	const FString GridLayout = Grid ? Grid->GetLayoutName() : FString();
	const bool bLayoutUsed = Grid && GridLayout == LoadingLayoutName;
	const FIntPoint GridSize = Grid ? Grid->GetGridSize() : FIntPoint::ZeroValue;
	if (bLayoutUsed)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: map loaded for level %d (%.1f ms after Continue): grid built %d x %d on layout %s, snapshot %s."),
			LoadingLevelIndex, (MapLoadedSeconds - LoadingStartSeconds) * 1000.0, GridSize.X, GridSize.Y, *GridLayout,
			Run && Run->HasPendingSnapshot() ? TEXT("still pending") : TEXT("applied"));
	}
	else
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunTransitionSubsystem: map loaded for level %d but the grid uses %s instead of %s (fallback or no grid)."),
			LoadingLevelIndex, Grid ? *GridLayout : TEXT("no grid"), *LoadingLayoutName);
	}

	// Hold the gameplay until the pool is warm: no alert, no console, no timed spawn. The miner input is held by the controller.
	URRRunFlowSubsystem* RunFlow = LoadedWorld->GetSubsystem<URRRunFlowSubsystem>();
	HeldRunFlow = RunFlow;
	if (RunFlow)
	{
		bHeldRunFlowWasEnabled = RunFlow->IsRunFlowEnabled();
		RunFlow->SetRunFlowEnabled(false);
	}
	ARREnemySpawner* Spawner = RunFlow ? RunFlow->GetSpawner() : nullptr;
	HeldSpawner = Spawner;
	if (!Spawner)
	{
		FinishLoading(TEXT("no spawner, nothing to prewarm"));
		return;
	}
	Spawner->SetSpawningHeld(true);
	if (Spawner->IsPoolPrewarmComplete())
	{
		FinishLoading(TEXT("no pool prewarm in this play"));
		return;
	}
	Spawner->OnPoolPrewarmFinished.AddUniqueDynamic(this, &URRRunTransitionSubsystem::HandlePoolPrewarmFinished);
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: level %d held (run flow off, spawning held) until the pool prewarm is done."), LoadingLevelIndex);

	const float Timeout = GetDefault<URRRunSettings>()->LoadingHoldTimeoutSeconds;
	if (Timeout > 0.f)
	{
		TWeakObjectPtr<URRRunTransitionSubsystem> WeakThis(this);
		HoldTimeoutHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float)
		{
			if (URRRunTransitionSubsystem* This = WeakThis.Get())
			{
				This->HoldTimeoutHandle.Reset();
				if (This->IsLevelLoading() && This->LoadingStage == ERRRunLoadingStage::PreparingLevel)
				{
					UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: pool prewarm not reported within the hold limit, level released anyway."));
					This->FinishLoading(TEXT("hold timeout"));
				}
			}
			return false;
		}), Timeout);
	}
}

void URRRunTransitionSubsystem::HandlePoolPrewarmFinished(ARREnemySpawner* Spawner)
{
	if (Spawner != HeldSpawner.Get() || !IsLevelLoading())
	{
		return;
	}
	FinishLoading(TEXT("pool prewarm done"));
}

void URRRunTransitionSubsystem::FinishLoading(const TCHAR* Reason)
{
	if (ARREnemySpawner* Spawner = HeldSpawner.Get())
	{
		Spawner->OnPoolPrewarmFinished.RemoveDynamic(this, &URRRunTransitionSubsystem::HandlePoolPrewarmFinished);
		Spawner->SetSpawningHeld(false);
	}
	if (URRRunFlowSubsystem* RunFlow = HeldRunFlow.Get())
	{
		RunFlow->SetRunFlowEnabled(bHeldRunFlowWasEnabled);
	}
	if (HoldTimeoutHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(HoldTimeoutHandle);
		HoldTimeoutHandle.Reset();
	}
	const int32 Level = LoadingLevelIndex;
	const FString Layout = LoadingLayoutName;
	const double Now = FPlatformTime::Seconds();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: level %d ready on layout %s (%s; %.1f ms after the map load, %.1f ms after Continue): run flow starts."),
		Level, *Layout, Reason, (Now - MapLoadedSeconds) * 1000.0, (Now - LoadingStartSeconds) * 1000.0);
	HeldRunFlow.Reset();
	HeldSpawner.Reset();
	HeldWorld.Reset();
	IntermissionWorld.Reset();
	Phase = ERRRunTransitionPhase::None;
	SetLoadingStage(ERRRunLoadingStage::None);
	LoadingLevelIndex = 0;
	OnLevelReady.Broadcast(Level, Layout);
}

void URRRunTransitionSubsystem::SetLoadingStage(ERRRunLoadingStage NewStage)
{
	if (LoadingStage == NewStage)
	{
		return;
	}
	LoadingStage = NewStage;
	OnLoadingStageChanged.Broadcast(NewStage);
}

void URRRunTransitionSubsystem::HandleRunReset()
{
	bRunWon = false;
	if (Phase != ERRRunTransitionPhase::None)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: run reset during a transition, transition dropped."));
	}
	ClearTransition();
}

void URRRunTransitionSubsystem::ClearTransition()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->CloseShop();
	}
	if (URRRunSubsystem* Run = GetRun())
	{
		Run->OnLayoutReady.RemoveDynamic(this, &URRRunTransitionSubsystem::HandleLayoutReady);
	}
	if (ARREnemySpawner* Spawner = HeldSpawner.Get())
	{
		Spawner->OnPoolPrewarmFinished.RemoveDynamic(this, &URRRunTransitionSubsystem::HandlePoolPrewarmFinished);
		Spawner->SetSpawningHeld(false);
	}
	if (URRRunFlowSubsystem* RunFlow = HeldRunFlow.Get())
	{
		RunFlow->SetRunFlowEnabled(bHeldRunFlowWasEnabled);
	}
	if (HoldTimeoutHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(HoldTimeoutHandle);
		HoldTimeoutHandle.Reset();
	}
	if (TravelWaitHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TravelWaitHandle);
		TravelWaitHandle.Reset();
	}
	bTravelRequested = false;
	LoadingFirstPaintFrame = 0;
	HeldRunFlow.Reset();
	HeldSpawner.Reset();
	HeldWorld.Reset();
	IntermissionWorld.Reset();
	IntermissionMiner.Reset();
	Phase = ERRRunTransitionPhase::None;
	LoadingStage = ERRRunLoadingStage::None;
	LoadingLevelIndex = 0;
}
