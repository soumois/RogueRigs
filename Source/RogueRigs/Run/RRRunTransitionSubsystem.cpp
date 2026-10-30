// Rogue Rigs - level-to-level transition of a run: intermission after an extraction, Continue, map travel and the loading hold
// until the next level is ready (task-206-03).

#include "Run/RRRunTransitionSubsystem.h"

#include "Drone/RRDrone.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Grid/RRArenaGrid.h"
#include "HAL/PlatformTime.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Progression/RRLevelUpComponent.h"
#include "Projectiles/RRProjectile.h"
#include "RogueRigs.h"
#include "Run/RRIntermissionShopSubsystem.h"
#include "Run/RRRunSettings.h"
#include "Run/RRRunSubsystem.h"
#include "RunLog/RRRunLogSubsystem.h"
#include "UObject/UObjectGlobals.h"
#include "Weapons/RRTurret.h"
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
	// The cards screen opens: the next level is prepared behind it, in this world (task-318-01).
	StartNextLevelPreparation();
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
	bNewRunLoading = false;
	if (PrepStep != ERRNextLevelPrepStep::None)
	{
		// In-place path (task-318-01): no map load. Ready: fade out, switch, fade in, no loading screen (task-318-02, intermission
		// gdd); else the loading screen until the preparation ends, then the switch and the fade in.
		SetLoadingStage(ERRRunLoadingStage::PreparingLevel);
		if (IsNextLevelReady())
		{
			UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: Continue: next level %d already prepared in this world (ready %.1f ms before Continue), frame %llu: fade path, no loading screen."),
				LoadingLevelIndex, (LoadingStartSeconds - PrepReadySeconds) * 1000.0, LoadingStartFrame);
			StartScreenFade(ERRTransitionFade::FadingOut, GetDefault<URRRunSettings>()->TransitionFadeOutSeconds);
		}
		else
		{
			bStartWhenPrepared = true;
			UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: Continue: next level %d still being prepared (progress %.2f), frame %llu: loading path, loading screen until ready (progress %s)."),
				LoadingLevelIndex, GetNextLevelProgress(), LoadingStartFrame, IsLoadingProgressMeasured() ? TEXT("measured") : TEXT("not measured"));
			OnLoadingStarted.Broadcast(LoadingLevelIndex, LoadingLayoutName);
		}
		return true;
	}

	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: Continue: loading started for level %d (layout %s), frame %llu: map travel (progress %s)."), LoadingLevelIndex,
		*LoadingLayoutName, LoadingStartFrame, IsLoadingProgressMeasured() ? TEXT("measured") : TEXT("not measured"));
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

bool URRRunTransitionSubsystem::StartNewRun(UWorld* MenuWorld, const FString& MapName)
{
	URRRunSubsystem* Run = GetRun();
	if (!MenuWorld || MapName.IsEmpty() || !Run)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: New Run refused (%s)."),
			!MenuWorld ? TEXT("no world") : (MapName.IsEmpty() ? TEXT("no map name") : TEXT("no run subsystem")));
		return false;
	}
	if (Phase != ERRRunTransitionPhase::None)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: New Run refused (transition phase %s already running)."),
			*StaticEnum<ERRRunTransitionPhase>()->GetNameStringByValue(static_cast<int64>(Phase)));
		return false;
	}
	// ResetRun drops any transition through HandleRunReset; the loading below starts from a clean state.
	Run->ResetRun();

	Recap = FRRIntermissionRecap();
	IntermissionWorld = MenuWorld;
	IntermissionMiner.Reset();
	MapToOpen = MapName;
	Phase = ERRRunTransitionPhase::Loading;
	bNewRunLoading = true;
	LoadingLevelIndex = 1;
	// The arena grid of level 1 builds from its own LayoutName; the settings entry of level 1 names the same layout.
	LoadingLayoutName = Run->GetLayoutNameForLevel(LoadingLevelIndex);
	LoadingStartSeconds = FPlatformTime::Seconds();
	LoadingStartFrame = GFrameCounter;
	LoadingFirstPaintFrame = 0;
	bTravelRequested = false;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: New Run: loading started for level %d (layout %s) in the menu world %s, frame %llu: map travel to %s (progress %s)."),
		LoadingLevelIndex, *LoadingLayoutName, *GetNameSafe(MenuWorld), LoadingStartFrame, *MapToOpen,
		IsLoadingProgressMeasured() ? TEXT("measured") : TEXT("not measured"));
	OnLoadingStarted.Broadcast(LoadingLevelIndex, LoadingLayoutName);
	OpenArenaMap();
	return true;
}

void URRRunTransitionSubsystem::OpenArenaMap()
{
	UWorld* World = IntermissionWorld.Get();
	URRRunSubsystem* Run = GetRun();
	// Level 1 is never requested (its grid reads its own LayoutName): New Run has no prepared layout to check.
	if (!bNewRunLoading && Run && !Run->IsLayoutReady(LoadingLevelIndex))
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
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: opening %s for level %d in frame %llu (loading started frame %llu, first loading paint frame %llu; %.1f ms after %s)."),
			*MapToOpen, LoadingLevelIndex, GFrameCounter, LoadingStartFrame, LoadingFirstPaintFrame, (FPlatformTime::Seconds() - LoadingStartSeconds) * 1000.0,
			GetLoadingStartLabel());
	}
	else
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: opening %s for level %d in frame %llu (loading started frame %llu, no loading paint after %llu frames; %.1f ms after %s)."),
			*MapToOpen, LoadingLevelIndex, GFrameCounter, LoadingStartFrame, GFrameCounter - TravelWaitStartFrame, (FPlatformTime::Seconds() - LoadingStartSeconds) * 1000.0,
			GetLoadingStartLabel());
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
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: map loaded for level %d (%.1f ms after %s): grid built %d x %d on layout %s, snapshot %s."),
			LoadingLevelIndex, (MapLoadedSeconds - LoadingStartSeconds) * 1000.0, GetLoadingStartLabel(), GridSize.X, GridSize.Y, *GridLayout,
			bNewRunLoading ? TEXT("none (new run)") : (Run && Run->HasPendingSnapshot() ? TEXT("still pending") : TEXT("applied")));
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
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: level %d held (stage PreparingLevel: run flow off, spawning held) until the pool prewarm is done."), LoadingLevelIndex);

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
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: level %d ready on layout %s (%s; %.1f ms after the map load, %.1f ms after %s): run flow starts."),
		Level, *Layout, Reason, (Now - MapLoadedSeconds) * 1000.0, (Now - LoadingStartSeconds) * 1000.0, GetLoadingStartLabel());
	bNewRunLoading = false;
	HeldRunFlow.Reset();
	HeldSpawner.Reset();
	HeldWorld.Reset();
	IntermissionWorld.Reset();
	Phase = ERRRunTransitionPhase::None;
	SetLoadingStage(ERRRunLoadingStage::None);
	LoadingLevelIndex = 0;
	OnLevelReady.Broadcast(Level, Layout);
	// The loading screen is gone: black fades away over the new level (task-318-02).
	StartScreenFade(ERRTransitionFade::FadingIn, GetDefault<URRRunSettings>()->TransitionFadeInSeconds);
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
	StopNextLevelPreparation();
	StopScreenFade();
	bTravelRequested = false;
	bNewRunLoading = false;
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

float URRRunTransitionSubsystem::GetNextLevelProgress() const
{
	// Work steps: WaitingForLayout, ClearingLevel, RebuildingTerrain, PlacingMiner; Ready is 1.
	const int32 Done = static_cast<int32>(PrepStep) - static_cast<int32>(ERRNextLevelPrepStep::WaitingForLayout);
	const int32 Total = static_cast<int32>(ERRNextLevelPrepStep::Ready) - static_cast<int32>(ERRNextLevelPrepStep::WaitingForLayout);
	return PrepStep == ERRNextLevelPrepStep::None ? 0.f : FMath::Clamp(static_cast<float>(Done) / static_cast<float>(Total), 0.f, 1.f);
}

float URRRunTransitionSubsystem::GetLoadingProgress() const
{
	return IsLevelLoading() && PrepStep != ERRNextLevelPrepStep::None ? GetNextLevelProgress() : 0.f;
}

void URRRunTransitionSubsystem::SetPrepStep(ERRNextLevelPrepStep NewStep)
{
	if (PrepStep == NewStep)
	{
		return;
	}
	PrepStep = NewStep;
	const float Progress = GetNextLevelProgress();
	if (IsLevelLoading())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: loading progress %.2f (step %s), frame %llu."), Progress,
			*StaticEnum<ERRNextLevelPrepStep>()->GetNameStringByValue(static_cast<int64>(NewStep)), GFrameCounter);
	}
	OnNextLevelProgressChanged.Broadcast(Progress);
}

float URRRunTransitionSubsystem::GetScreenFadeAlpha() const
{
	if (ScreenFade == ERRTransitionFade::None)
	{
		return 0.f;
	}
	const float T = ScreenFadeDuration > 0.f ? FMath::Clamp(static_cast<float>((FPlatformTime::Seconds() - ScreenFadeStartSeconds) / ScreenFadeDuration), 0.f, 1.f) : 1.f;
	return ScreenFade == ERRTransitionFade::FadingOut ? T : 1.f - T;
}

void URRRunTransitionSubsystem::StartScreenFade(ERRTransitionFade Fade, float Duration)
{
	StopScreenFade();
	if (Fade == ERRTransitionFade::None)
	{
		return;
	}
	ScreenFade = Fade;
	ScreenFadeDuration = FMath::Max(0.f, Duration);
	ScreenFadeStartSeconds = FPlatformTime::Seconds();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: screen fade %s started (%.2f s), frame %llu."),
		Fade == ERRTransitionFade::FadingOut ? TEXT("out") : TEXT("in"), ScreenFadeDuration, GFrameCounter);
	OnScreenFadeStarted.Broadcast(Fade, ScreenFadeDuration);
	if (ScreenFadeDuration <= 0.f)
	{
		FinishScreenFade();
		return;
	}
	// Real time on the core ticker: the fade out runs while the world is still paused behind the cards screen.
	TWeakObjectPtr<URRRunTransitionSubsystem> WeakThis(this);
	ScreenFadeHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float)
	{
		if (URRRunTransitionSubsystem* Self = WeakThis.Get())
		{
			Self->ScreenFadeHandle.Reset();
			Self->FinishScreenFade();
		}
		return false;
	}), ScreenFadeDuration);
}

void URRRunTransitionSubsystem::FinishScreenFade()
{
	const ERRTransitionFade Fade = ScreenFade;
	if (Fade == ERRTransitionFade::None)
	{
		return;
	}
	if (ScreenFadeHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(ScreenFadeHandle);
		ScreenFadeHandle.Reset();
	}
	const double ElapsedMs = (FPlatformTime::Seconds() - ScreenFadeStartSeconds) * 1000.0;
	ScreenFade = ERRTransitionFade::None;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: screen fade %s finished after %.1f ms, frame %llu%s."),
		Fade == ERRTransitionFade::FadingOut ? TEXT("out") : TEXT("in"), ElapsedMs, GFrameCounter,
		Fade == ERRTransitionFade::FadingOut ? TEXT(": switching to the prepared level") : TEXT(""));
	if (Fade == ERRTransitionFade::FadingOut)
	{
		// The screen stays black until the fade in starts in StartPreparedLevel, in this frame (the overlay keeps its full opacity).
		OnScreenFadeFinished.Broadcast(Fade);
		if (IsLevelLoading() && IsNextLevelReady())
		{
			StartPreparedLevel();
		}
		return;
	}
	OnScreenFadeFinished.Broadcast(Fade);
}

void URRRunTransitionSubsystem::StopScreenFade()
{
	if (ScreenFadeHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(ScreenFadeHandle);
		ScreenFadeHandle.Reset();
	}
	ScreenFade = ERRTransitionFade::None;
}

bool URRRunTransitionSubsystem::StartNextLevelPreparation()
{
	StopNextLevelPreparation();
	UWorld* World = IntermissionWorld.Get();
	const URRRunSubsystem* Run = GetRun();
	if (!World || !Run || !RRWorld::FindArenaGrid(World) || !RRWorld::FindLivingMiner(World))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: next level %d cannot be prepared in this world (no world, run, grid or miner): Continue will open the map."),
			Recap.NextLevelIndex);
		return false;
	}
	SetPrepStep(ERRNextLevelPrepStep::WaitingForLayout);
	bStartWhenPrepared = false;
	PrepStartSeconds = FPlatformTime::Seconds();
	PrepReadySeconds = 0.0;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: next level %d preparation started in this world (layout %s %s), frame %llu."),
		Recap.NextLevelIndex, *Recap.NextLayoutName, Run->IsLayoutPending(Recap.NextLevelIndex) ? TEXT("pending") : TEXT("prepared"), GFrameCounter);
	// The world is paused behind the cards screen: the core ticker still runs. One step per frame keeps each hitch short.
	TWeakObjectPtr<URRRunTransitionSubsystem> WeakThis(this);
	PrepTickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float DeltaTime)
	{
		URRRunTransitionSubsystem* Self = WeakThis.Get();
		return Self ? Self->TickNextLevelPreparation(DeltaTime) : false;
	}));
	return true;
}

bool URRRunTransitionSubsystem::TickNextLevelPreparation(float DeltaTime)
{
	UWorld* World = IntermissionWorld.Get();
	URRRunSubsystem* Run = GetRun();
	if (!World || !Run || PrepStep == ERRNextLevelPrepStep::None || PrepStep == ERRNextLevelPrepStep::Ready)
	{
		if (PrepStep != ERRNextLevelPrepStep::Ready)
		{
			PrepStep = ERRNextLevelPrepStep::None;
		}
		PrepTickHandle.Reset();
		return false;
	}
	const int32 Level = Recap.NextLevelIndex;
	const double StepStart = FPlatformTime::Seconds();
	switch (PrepStep)
	{
	case ERRNextLevelPrepStep::WaitingForLayout:
		if (Run->IsLayoutPending(Level))
		{
			return true;
		}
		if (!Run->IsLayoutReady(Level))
		{
			// Provisional decision (task-206-03): the grid falls back to its own LayoutName, logged as an error.
			UE_LOG(LogRogueRigs, Error, TEXT("URRRunTransitionSubsystem: layout %s of level %d failed; the grid will fall back to its own layout."), *Recap.NextLayoutName, Level);
		}
		SetPrepStep(ERRNextLevelPrepStep::ClearingLevel);
		return true;
	case ERRNextLevelPrepStep::ClearingLevel:
	{
		const int32 Removed = ClearLevelActors(*World);
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: next level %d: level cleared (%d actors, objectives and run flow reset) in %.1f ms."),
			Level, Removed, (FPlatformTime::Seconds() - StepStart) * 1000.0);
		SetPrepStep(ERRNextLevelPrepStep::RebuildingTerrain);
		return true;
	}
	case ERRNextLevelPrepStep::RebuildingTerrain:
	{
		ARRArenaGrid* Grid = RRWorld::FindArenaGrid(World);
		const bool bBuilt = Grid && Grid->RebuildForRunLevel();
		const FIntPoint Size = Grid ? Grid->GetGridSize() : FIntPoint::ZeroValue;
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: next level %d: terrain rebuilt %d x %d on layout %s%s in %.1f ms."), Level, Size.X, Size.Y,
			Grid ? *Grid->GetLayoutName() : TEXT("none"), bBuilt ? TEXT("") : TEXT(" (FAILED, empty grid)"), (FPlatformTime::Seconds() - StepStart) * 1000.0);
		SetPrepStep(ERRNextLevelPrepStep::PlacingMiner);
		return true;
	}
	case ERRNextLevelPrepStep::PlacingMiner:
		PlaceMinerAtStart(*World);
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: next level %d: miner placed, flow field recomputed in %.1f ms."), Level,
			(FPlatformTime::Seconds() - StepStart) * 1000.0);
		break;
	default:
		break;
	}

	SetPrepStep(ERRNextLevelPrepStep::Ready);
	PrepReadySeconds = FPlatformTime::Seconds();
	PrepTickHandle.Reset();
	const URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>();
	const ARREnemySpawner* Spawner = RunFlow ? RunFlow->GetSpawner() : nullptr;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: next level %d ready in this world on layout %s (preparation %.1f ms, frame %llu; pool prewarm %s)%s."),
		Level, *Recap.NextLayoutName, (PrepReadySeconds - PrepStartSeconds) * 1000.0, GFrameCounter,
		!Spawner ? TEXT("no spawner") : (Spawner->IsPoolPrewarmComplete() ? TEXT("complete, pool kept warm") : TEXT("not complete")),
		bStartWhenPrepared ? TEXT(", Continue already pressed") : TEXT(", before Continue"));
	OnNextLevelReady.Broadcast(Level, Recap.NextLayoutName);
	if (bStartWhenPrepared && IsLevelLoading())
	{
		StartPreparedLevel();
	}
	return false;
}

int32 URRRunTransitionSubsystem::ClearLevelActors(UWorld& World)
{
	ARRMiner* Miner = RRWorld::FindLivingMiner(&World);
	URRRunFlowSubsystem* RunFlow = World.GetSubsystem<URRRunFlowSubsystem>();
	if (RunFlow)
	{
		RunFlow->ResetForNextLevel();
	}
	if (ARRDrone* Drone = Miner ? Miner->GetDrone() : nullptr)
	{
		Drone->ReleaseDig();
	}

	// Everything a level spawns at runtime: robots, pickups, projectiles, explosion FX, placed turrets. Pool members go back idle
	// (the pool stays warm), the others are destroyed, as a map travel would drop them.
	URRActorPoolSubsystem* Pool = World.GetSubsystem<URRActorPoolSubsystem>();
	TArray<AActor*> ToRemove;
	for (TActorIterator<AActor> It(&World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || Actor->IsActorBeingDestroyed() || URRActorPoolSubsystem::IsPooledIdle(Actor))
		{
			continue;
		}
		if (Actor->IsA<ARREnemy>() || Actor->IsA<ARRPickup>() || Actor->IsA<ARRProjectile>() || Actor->IsA<ARRExplosionFX>() || Actor->IsA<ARRTurret>())
		{
			ToRemove.Add(Actor);
		}
	}
	int32 Released = 0;
	for (AActor* Actor : ToRemove)
	{
		if (!IsValid(Actor) || Actor->IsActorBeingDestroyed())
		{
			continue;
		}
		if (Pool && Pool->IsPoolMember(Actor))
		{
			if (!Pool->IsIdle(Actor) && Pool->Release(Actor))
			{
				++Released;
			}
		}
		else
		{
			Actor->Destroy();
		}
	}
	if (URRExplosionSubsystem* Explosions = World.GetSubsystem<URRExplosionSubsystem>())
	{
		Explosions->ClearForNextLevel();
	}
	if (ARREnemySpawner* Spawner = RunFlow ? RunFlow->GetSpawner() : nullptr)
	{
		Spawner->ResetForNextLevel();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: %d level actors removed (%d back to the pool, %d destroyed)."), ToRemove.Num(), Released,
		ToRemove.Num() - Released);
	return ToRemove.Num();
}

void URRRunTransitionSubsystem::PlaceMinerAtStart(UWorld& World)
{
	ARRMiner* Miner = RRWorld::FindLivingMiner(&World);
	if (!Miner)
	{
		return;
	}
	AGameModeBase* GameMode = World.GetAuthGameMode();
	Miner->ShowAfterExtraction();
	AController* Controller = Miner->GetController();
	const AActor* Start = GameMode && Controller ? GameMode->FindPlayerStart(Controller) : nullptr;
	if (Start)
	{
		const FRotator Rotation(0.f, Start->GetActorRotation().Yaw, 0.f);
		Miner->TeleportTo(Start->GetActorLocation(), Rotation, false, true);
		if (UCharacterMovementComponent* Movement = Miner->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
		}
		if (ARRDrone* Drone = Miner->GetDrone())
		{
			Drone->SetActorLocation(Miner->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		}
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: miner placed at %s (%s)."), *Miner->GetActorLocation().ToCompactString(), *Start->GetName());
	}
	else
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunTransitionSubsystem: no player start found, the miner keeps its place."));
	}
	if (URRFlowFieldSubsystem* FlowField = World.GetSubsystem<URRFlowFieldSubsystem>())
	{
		FlowField->RebuildForNewTerrain();
	}
	// The extractor that dropped the miner stands next to him under the black; it takes off when the fade in ends (task-371-01).
	if (URRRunFlowSubsystem* RunFlow = World.GetSubsystem<URRRunFlowSubsystem>())
	{
		RunFlow->PlaceArrivalShuttle();
	}
}

void URRRunTransitionSubsystem::StopNextLevelPreparation()
{
	if (PrepTickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(PrepTickHandle);
		PrepTickHandle.Reset();
	}
	PrepStep = ERRNextLevelPrepStep::None;
	bStartWhenPrepared = false;
}

void URRRunTransitionSubsystem::StartPreparedLevel()
{
	UWorld* World = IntermissionWorld.Get();
	ARRMiner* Miner = World ? RRWorld::FindLivingMiner(World) : nullptr;
	if (URRRunSubsystem* Run = GetRun())
	{
		Run->ConsumeSnapshotInPlace(Miner);
	}
	if (URRRunLogSubsystem* RunLog = Miner ? URRRunLogSubsystem::Get(Miner) : nullptr)
	{
		RunLog->HandleLevelStartedInPlace(Miner);
	}
	if (URRLevelUpComponent* LevelUp = Miner ? Miner->GetLevelUp() : nullptr)
	{
		LevelUp->ResumeChoices();
	}
	if (APlayerController* Controller = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr)
	{
		Controller->SetPause(false);
		// Back to the project's capture mode, as the level-up screen does when it closes.
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);
		Controller->SetInputMode(InputMode);
		Controller->SetShowMouseCursor(Controller->GetClass()->GetDefaultObject<APlayerController>()->bShowMouseCursor);
	}
	StopNextLevelPreparation();

	const int32 Level = LoadingLevelIndex;
	const FString Layout = LoadingLayoutName;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunTransitionSubsystem: level %d ready on layout %s (prepared in this world, no map load; %.1f ms after Continue): run flow starts."),
		Level, *Layout, (FPlatformTime::Seconds() - LoadingStartSeconds) * 1000.0);
	IntermissionWorld.Reset();
	Phase = ERRRunTransitionPhase::None;
	SetLoadingStage(ERRRunLoadingStage::None);
	LoadingLevelIndex = 0;
	OnLevelReady.Broadcast(Level, Layout);
	// Fade path: black since the fade out; loading path: the loading screen just left. Either way black fades away (task-318-02).
	StartScreenFade(ERRTransitionFade::FadingIn, GetDefault<URRRunSettings>()->TransitionFadeInSeconds);
}
