// Rogue Rigs - game mode of the generator test map (task-494-01) and its console command rr.Level.New [seed].

#include "Player/RRGenTestGameMode.h"

#include "Components/CapsuleComponent.h"
#include "Components/Widget.h"
#include "Drone/RRDrone.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RRLayoutGenParams.h"
#include "Grid/RRLayoutLoaderSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Misc/Crc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Player/RRPlayerController.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Projectiles/RRProjectile.h"
#include "RogueRigs.h"
#include "Blueprint/UserWidget.h"
#include "UI/RRGenTestSeedWidget.h"
#include "UI/RRHUDWidget.h"
#include "Weapons/RRTurret.h"
#include "World/RRWorld.h"

ARRGenTestGameMode::ARRGenTestGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

ARRGenTestGameMode* ARRGenTestGameMode::Get(const UWorld* World)
{
	return World ? Cast<ARRGenTestGameMode>(World->GetAuthGameMode()) : nullptr;
}

void ARRGenTestGameMode::StartPlay()
{
	UWorld* World = GetWorld();
	// Before the actors' BeginPlay: the map grid keeps no layout of its own, the loader builds it.
	int32 Grids = 0;
	for (TActorIterator<ARRArenaGrid> It(World); It; ++It)
	{
		It->SetExternalBuild(true);
		++Grids;
	}
	int32 Spawners = 0;
	for (TActorIterator<ARREnemySpawner> It(World); It; ++It)
	{
		It->Destroy();
		++Spawners;
	}
	if (URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>())
	{
		RunFlow->SetRunFlowEnabled(false);
		RunFlow->SetArrivalEnabled(false);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Level test mode: run flow and arrival off (no alert, console, bonus chest or shuttle), %d enemy spawner(s) removed, %d grid(s) left to the loader."),
		Spawners, Grids);
	if (Grids == 0)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("rr.Level test mode: the map has no ARRArenaGrid; place one (it is rebuilt in place by each layout)."));
	}
	Super::StartPlay();
}

void ARRGenTestGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(this))
	{
		Loader->OnLoadCompleted.AddUniqueDynamic(this, &ARRGenTestGameMode::HandleLoadCompleted);
	}
	RequestNewLayout(StartSeed);
}

void ARRGenTestGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(this))
	{
		Loader->OnLoadCompleted.RemoveDynamic(this, &ARRGenTestGameMode::HandleLoadCompleted);
	}
	if (SeedWidget)
	{
		SeedWidget->RemoveFromParent();
		SeedWidget = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void ARRGenTestGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bHUDHidden)
	{
		bHUDHidden = HideHUDWidgets();
	}
	if (!SeedWidget)
	{
		CreateSeedWidget();
	}
	if (State == EState::Generating && GenerateTask.IsValid() && GenerateTask.IsCompleted())
	{
		HandleGenerated();
	}
	if (bMinerFrozen)
	{
		// The miner may spawn or change movement mode after the freeze: keep it still until the layout is there.
		SetMinerFrozen(true, nullptr);
	}
	if (WatchLogSeconds > 0.f)
	{
		WatchElapsed += DeltaSeconds;
		if (WatchElapsed >= WatchLogSeconds)
		{
			WatchElapsed = 0.f;
			LogWatch();
		}
	}
}

bool ARRGenTestGameMode::RequestNewLayout(int32 Seed)
{
	if (IsBusy())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("rr.Level.New: layout %d still %s; request ignored."), RequestedSeed,
			State == EState::Generating ? TEXT("generating") : TEXT("loading"));
		return false;
	}
	FString Params = ParamsPath.IsEmpty() ? RRLayoutGen::GetDefaultParamsPath() : ParamsPath;
	if (FPaths::IsRelative(Params))
	{
		Params = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), Params));
	}
	FRRLayoutGenParams GenParams;
	FString Error;
	if (!RRLayoutGen::LoadParams(Params, GenParams, Error))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("rr.Level.New: %s"), *Error);
		ShowScreenLine(FString::Printf(TEXT("Layout params failed: %s"), *Error), FColor::Red);
		return false;
	}
	RequestedSeed = Seed > 0 ? Seed : FMath::RandRange(1, 999999);
	RequestSeconds = FPlatformTime::Seconds();
	State = EState::Generating;
	ShowScreenLine(FString::Printf(TEXT("Generating layout, seed %d..."), RequestedSeed), FColor::Yellow);
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.New: generating from seed %d (%s), params %s."), RequestedSeed, Seed > 0 ? TEXT("given") : TEXT("random"), *Params);

	// First layout: no floor yet under the miner, keep it still during the generation too.
	const ARRArenaGrid* Grid = FindMapGrid();
	if (!Grid || Grid->GetGridSize().X <= 0)
	{
		SetMinerFrozen(true, nullptr);
	}

	const int32 FirstSeed = RequestedSeed;
	GenerateTask = UE::Tasks::Launch(UE_SOURCE_LOCATION, [GenParams, FirstSeed]() -> TSharedPtr<FRRGeneratedLayout>
	{
		FRRLayoutGenerator Generator(GenParams);
		return MakeShared<FRRGeneratedLayout>(Generator.Generate(FirstSeed, nullptr));
	});
	return true;
}

void ARRGenTestGameMode::HandleGenerated()
{
	PendingLayout = GenerateTask.GetResult();
	GenerateTask = {};
	const double GenerationSeconds = FPlatformTime::Seconds() - RequestSeconds;
	if (!PendingLayout.IsValid() || PendingLayout->Fails.Num() > 0)
	{
		const int32 Tried = PendingLayout.IsValid() ? PendingLayout->SeedsTried : 0;
		UE_LOG(LogRogueRigs, Error, TEXT("rr.Level.New: no seed in [%d, %d] passes the generator checks (%.2f s); layout unchanged."),
			RequestedSeed, RequestedSeed + FMath::Max(0, Tried - 1), GenerationSeconds);
		ShowScreenLine(FString::Printf(TEXT("Seed %d: no layout passed, try another seed"), RequestedSeed), FColor::Red);
		State = EState::Idle;
		SetMinerFrozen(false, nullptr);
		return;
	}

	const FString Name = FString::Printf(TEXT("%s%d"), *LayoutFilePrefix, PendingLayout->Seed);
	PendingLayoutPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("GeneratedLevels"), Name + TEXT(".json")));
	const FString Json = FRRLayoutGenerator::ToJson(Name, *PendingLayout);
	if (!FFileHelper::SaveStringToFile(Json, *PendingLayoutPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("rr.Level.New: cannot write %s; layout unchanged."), *PendingLayoutPath);
		State = EState::Idle;
		SetMinerFrozen(false, nullptr);
		return;
	}
	const FRRLayoutGenStats& S = PendingLayout->Stats;
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.New: seed %d accepted (requested %d, %d seed(s) tried), generation %.2f s (search %.2f s); %d x %d, rock %d, heal_ore %d, barrel %d, void %d; JSON crc %08x, wrote %s."),
		PendingLayout->Seed, RequestedSeed, PendingLayout->SeedsTried, GenerationSeconds, PendingLayout->SearchSeconds, PendingLayout->Width, PendingLayout->Height,
		S.Counts.FindRef(TEXT("rock")), S.Counts.FindRef(TEXT("heal_ore")), S.Counts.FindRef(TEXT("barrel")), S.Counts.FindRef(TEXT("void")),
		FCrc::StrCrc32(*Json), *PendingLayoutPath);

	ARRArenaGrid* Grid = FindMapGrid();
	URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(this);
	const int32 Removed = ClearLayoutActors();
	SetMinerFrozen(true, nullptr);
	if (!Loader || !Loader->StartLoadIntoGrid(PendingLayoutPath, Grid))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("rr.Level.New: the loader did not start (grid %s); layout unchanged."), *GetNameSafe(Grid));
		State = EState::Idle;
		SetMinerFrozen(false, nullptr);
		return;
	}
	State = EState::Loading;
	LoadStartSeconds = FPlatformTime::Seconds();
	ShowScreenLine(FString::Printf(TEXT("Loading layout, seed %d..."), PendingLayout->Seed), FColor::Yellow);
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.New: %d actor(s) of the previous layout removed; loading into %s."), Removed, *GetNameSafe(Grid));
}

void ARRGenTestGameMode::HandleLoadCompleted(ARRArenaGrid* Grid, bool bSuccess)
{
	if (State != EState::Loading)
	{
		return;
	}
	State = EState::Idle;
	const double LoadSeconds = FPlatformTime::Seconds() - LoadStartSeconds;
	const int32 Seed = PendingLayout.IsValid() ? PendingLayout->Seed : RequestedSeed;
	if (!bSuccess || !IsValid(Grid))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("rr.Level.New: load of seed %d failed (%s)."), Seed, *PendingLayoutPath);
		ShowScreenLine(FString::Printf(TEXT("Seed %d: load failed"), Seed), FColor::Red);
		SetMinerFrozen(false, nullptr);
		return;
	}
	CurrentSeed = Seed;
	++LayoutCount;
	if (URRFlowFieldSubsystem* FlowField = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>())
	{
		FlowField->RebuildForNewTerrain();
	}
	SetMinerFrozen(false, Grid);
	if (const ARRPlayerController* Controller = Cast<ARRPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		if (URRHUDWidget* HUD = Controller->GetHUDWidget())
		{
			HUD->ResetMinimap();
		}
	}
	ShowScreenLine(FString::Printf(TEXT("Seed %d   (rr.Level.New [seed] for a new layout)"), CurrentSeed), FColor::White);

	const URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(this);
	const FRRLayoutLoadStats* Stats = Loader ? &Loader->GetLastLoadStats() : nullptr;
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.New: layout %d ready, seed %d (requested %d): request to play %.2f s, load %.2f s over %d frames, cells file/grid match %s; spawn cell (%d, %d)."),
		LayoutCount, CurrentSeed, RequestedSeed, FPlatformTime::Seconds() - RequestSeconds, LoadSeconds, Stats ? Stats->Frames : 0,
		Stats && Stats->bCountsMatch ? TEXT("yes") : TEXT("NO"), Grid->GetSpawnCell().X, Grid->GetSpawnCell().Y);
	LogWatch();
}

int32 ARRGenTestGameMode::ClearLayoutActors()
{
	UWorld* World = GetWorld();
	if (ARRMiner* Miner = RRWorld::FindLivingMiner(World))
	{
		if (ARRDrone* Drone = Miner->GetDrone())
		{
			Drone->ReleaseDig();
		}
	}
	// Same families as a level change in the same world (URRRunTransitionSubsystem::ClearLevelActors): pool members go back idle.
	URRActorPoolSubsystem* Pool = World->GetSubsystem<URRActorPoolSubsystem>();
	TArray<AActor*> ToRemove;
	for (TActorIterator<AActor> It(World); It; ++It)
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
	for (AActor* Actor : ToRemove)
	{
		if (!IsValid(Actor) || Actor->IsActorBeingDestroyed())
		{
			continue;
		}
		if (Pool && Pool->IsPoolMember(Actor))
		{
			if (!Pool->IsIdle(Actor))
			{
				Pool->Release(Actor);
			}
		}
		else
		{
			Actor->Destroy();
		}
	}
	if (URRExplosionSubsystem* Explosions = World->GetSubsystem<URRExplosionSubsystem>())
	{
		Explosions->ClearForNextLevel();
	}
	if (URRNiagaraPoolSubsystem* FXPool = World->GetSubsystem<URRNiagaraPoolSubsystem>())
	{
		FXPool->ClearForNextLevel();
	}
	return ToRemove.Num();
}

void ARRGenTestGameMode::SetMinerFrozen(bool bFrozen, ARRArenaGrid* Grid)
{
	ARRMiner* Miner = RRWorld::FindLivingMiner(GetWorld());
	UCharacterMovementComponent* Movement = Miner ? Miner->GetCharacterMovement() : nullptr;
	if (bFrozen)
	{
		bMinerFrozen = true;
		if (Movement && Movement->MovementMode != MOVE_None)
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}
		return;
	}
	bMinerFrozen = false;
	if (!Miner)
	{
		return;
	}
	if (Grid && Grid->GetSpawnCell().X >= 0)
	{
		const UCapsuleComponent* Capsule = Miner->GetCapsuleComponent();
		const float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 100.f;
		const FIntPoint Cell = Grid->GetSpawnCell();
		const FVector Location = Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f);
		Miner->SetActorLocation(Location, /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
		if (ARRDrone* Drone = Miner->GetDrone())
		{
			Drone->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
		}
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.New: miner put on the spawn cell (%d, %d) at %s."), Cell.X, Cell.Y, *Location.ToCompactString());
	}
	if (Movement)
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
}

bool ARRGenTestGameMode::HideHUDWidgets()
{
	const ARRPlayerController* Controller = Cast<ARRPlayerController>(GetWorld()->GetFirstPlayerController());
	URRHUDWidget* HUD = Controller ? Controller->GetHUDWidget() : nullptr;
	if (!HUD)
	{
		return false;
	}
	for (const FName Name : HiddenHUDWidgetNames)
	{
		if (UWidget* Widget = HUD->FindHUDWidget(Name))
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Level test mode: HUD widget %s hidden."), *Name.ToString());
		}
		else
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Level test mode: HUD widget %s not found."), *Name.ToString());
		}
	}
	return true;
}

void ARRGenTestGameMode::LogWatch() const
{
	int32 Robots = 0;
	int32 Consoles = 0;
	int32 Chests = 0;
	int32 Shuttles = 0;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		const AActor* Actor = *It;
		if (!IsValid(Actor) || URRActorPoolSubsystem::IsPooledIdle(Actor))
		{
			continue;
		}
		Robots += Actor->IsA<ARREnemy>() ? 1 : 0;
		Chests += Actor->IsA<ARRBonusChest>() ? 1 : 0;
		Consoles += Actor->IsA<ARRConsole>() && !Actor->IsA<ARRBonusChest>() ? 1 : 0;
		Shuttles += Actor->IsA<ARRShuttle>() ? 1 : 0;
	}
	const URRRunFlowSubsystem* RunFlow = GetWorld()->GetSubsystem<URRRunFlowSubsystem>();
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Level test mode watch at %.0f s: robots %d, consoles %d, bonus chests %d, shuttles %d, run flow %s, alert %.2f, seed %d."),
		GetWorld()->GetTimeSeconds(), Robots, Consoles, Chests, Shuttles, RunFlow && RunFlow->IsRunFlowEnabled() ? TEXT("on") : TEXT("off"),
		RunFlow ? RunFlow->GetAlert() : 0.f, CurrentSeed);
}

void ARRGenTestGameMode::ShowScreenLine(const FString& Text, const FColor& Color)
{
	ScreenText = Text;
	ScreenColor = Color;
	if (SeedWidget)
	{
		SeedWidget->SetLine(ScreenText, FLinearColor(ScreenColor));
	}
}

bool ARRGenTestGameMode::CreateSeedWidget()
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	if (!Controller || !Controller->IsLocalController())
	{
		return false;
	}
	SeedWidget = CreateWidget<URRGenTestSeedWidget>(Controller, URRGenTestSeedWidget::StaticClass());
	if (!SeedWidget)
	{
		return false;
	}
	// Top centre (SetPositionInViewport would reset the anchors to the top left; the widget pads itself from the top edge).
	SeedWidget->SetAnchorsInViewport(FAnchors(0.5f, 0.f));
	SeedWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.f));
	SeedWidget->AddToPlayerScreen(5);
	SeedWidget->SetLine(ScreenText, FLinearColor(ScreenColor));
	return true;
}

ARRArenaGrid* ARRGenTestGameMode::FindMapGrid() const
{
	return RRWorld::FindArenaGrid(GetWorld());
}

#if !UE_BUILD_SHIPPING

namespace RRGenTestCommands
{
	void LevelNew(const TArray<FString>& Args, UWorld* World)
	{
		ARRGenTestGameMode* Mode = ARRGenTestGameMode::Get(World);
		if (!Mode)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Level.New: only in the generator test map (game mode ARRGenTestGameMode)."));
			return;
		}
		int32 Seed = 0;
		if (Args.Num() > 0)
		{
			if (!Args[0].IsNumeric() || FCString::Atoi(*Args[0]) <= 0)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Level.New [seed]: the seed must be a positive integer, got '%s'."), *Args[0]);
				return;
			}
			Seed = FCString::Atoi(*Args[0]);
		}
		Mode->RequestNewLayout(Seed);
	}

	static FAutoConsoleCommandWithWorldAndArgs LevelNewCommand(
		TEXT("rr.Level.New"),
		TEXT("rr.Level.New [seed]: generator test map only. Generates a new 240 x 240 layout (random seed when omitted; the search may accept a higher seed), writes it to Saved/GeneratedLevels/, rebuilds the map grid with the asynchronous loader and puts the miner on the new spawn cell. The seed is shown on screen and logged."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&LevelNew));
}

#endif
