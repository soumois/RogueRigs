// Rogue Rigs - asynchronous layout loader (task-493-01).

#include "Grid/RRLayoutLoaderSubsystem.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Engine/World.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RRLayoutData.h"
#include "Grid/RRLayoutLoaderSettings.h"
#include "HAL/PlatformTime.h"
#include "Misc/Paths.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRLayoutLoaderSubsystem)

namespace RRLayoutLoaderPrivate
{
	/** Number of ERRCellType values counted (Floor .. Spawn). */
	constexpr int32 CellTypeCount = 8;

	const TCHAR* PhaseName(ERRLayoutLoadPhase Phase)
	{
		switch (Phase)
		{
		case ERRLayoutLoadPhase::Loading:  return TEXT("parse+assets");
		case ERRLayoutLoadPhase::Building: return TEXT("build");
		case ERRLayoutLoadPhase::Done:     return TEXT("done");
		case ERRLayoutLoadPhase::Failed:   return TEXT("failed");
		default:                           return TEXT("idle");
		}
	}
}

URRLayoutLoaderSubsystem* URRLayoutLoaderSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	return World ? World->GetSubsystem<URRLayoutLoaderSubsystem>() : nullptr;
}

FString URRLayoutLoaderSubsystem::ResolveLayoutPath(const FString& LayoutPath)
{
	FString Path = LayoutPath.TrimStartAndEnd();
	const bool bBareName = !Path.Contains(TEXT("/")) && !Path.Contains(TEXT("\\")) && FPaths::GetExtension(Path).IsEmpty();
	if (bBareName)
	{
		return FPaths::ConvertRelativePathToFull(RRLayout::GetLayoutFilePath(Path));
	}
	if (FPaths::IsRelative(Path))
	{
		Path = FPaths::Combine(FPaths::ProjectDir(), Path);
	}
	return FPaths::ConvertRelativePathToFull(Path);
}

bool URRLayoutLoaderSubsystem::StartLoad(const FString& LayoutPath, FTransform GridTransform)
{
	if (IsLoading())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("RRLayoutLoader: a load of %s is running; %s not started."), *LayoutFilePath, *LayoutPath);
		return false;
	}
	const URRLayoutLoaderSettings* Settings = GetDefault<URRLayoutLoaderSettings>();
	if (Settings->GridClass.IsNull())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutLoader: URRLayoutLoaderSettings::GridClass is not set; %s not started."), *LayoutPath);
		return false;
	}
	const double CallStart = FPlatformTime::Seconds();

	Stats = FRRLayoutLoadStats();
	Layout.Reset();
	LoadedGrid = nullptr;
	AssetHandle.Reset();
	LoadPercent = 0.f;
	LastLoggedDecile = -1;
	PendingTransform = GridTransform;
	RequestedGridClass = Settings->GridClass;
	LayoutFilePath = ResolveLayoutPath(LayoutPath);
	const FString LayoutName = FPaths::GetBaseFilename(LayoutFilePath);

	// 1. File read and parse on a worker thread (no UObject touched there).
	const FString FilePath = LayoutFilePath;
	ParseTask = UE::Tasks::Launch(UE_SOURCE_LOCATION, [LayoutName, FilePath]() -> TSharedPtr<FRRLayoutData>
	{
		TSharedPtr<FRRLayoutData> Parsed = MakeShared<FRRLayoutData>();
		RRLayout::LoadLayoutFile(LayoutName, FilePath, *Parsed);
		return Parsed;
	});

	// 2. In parallel, the grid Blueprint and every asset it references, loaded asynchronously.
	TArray<FSoftObjectPath> Paths;
	CollectGridAssets(RequestedGridClass.ToSoftObjectPath(), Paths);
	for (const FSoftObjectPath& Path : Paths)
	{
		Stats.AssetsAlreadyLoaded += Path.ResolveObject() ? 1 : 0;
	}
	Stats.AssetsRequested = Paths.Num();
	AssetHandle = StreamableManager.RequestAsyncLoad(Paths, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, /*bManageActiveHandle*/ false,
		/*bStartStalled*/ false, TEXT("RRLayoutLoader"));

	Phase = ERRLayoutLoadPhase::Loading;
	StartSeconds = CallStart;
	LastTickSeconds = FPlatformTime::Seconds();
	Stats.MaxStepMs = (LastTickSeconds - CallStart) * 1000.0;
	UE_LOG(LogRogueRigs, Log, TEXT("RRLayoutLoader: loading %s (%d assets requested, %d already in memory; start call %.2f ms)."),
		*LayoutFilePath, Stats.AssetsRequested, Stats.AssetsAlreadyLoaded, Stats.MaxStepMs);
	return true;
}

void URRLayoutLoaderSubsystem::CollectGridAssets(const FSoftObjectPath& GridClassPath, TArray<FSoftObjectPath>& OutPaths) const
{
	OutPaths.Reset();
	OutPaths.Add(GridClassPath);
	IAssetRegistry* Registry = IAssetRegistry::Get();
	if (!Registry)
	{
		return;
	}
	// Hard package dependencies of the grid Blueprint, recursively (meshes, materials, textures, pickup Blueprints...). Native
	// script packages are skipped. A cooked registry without dependency data leaves the class alone: its load still pulls them.
	TSet<FName> Visited;
	TArray<FName> Stack;
	Stack.Add(GridClassPath.GetLongPackageFName());
	Visited.Add(Stack[0]);
	TArray<FName> Dependencies;
	TArray<FAssetData> Assets;
	while (Stack.Num() > 0)
	{
		const FName Package = Stack.Pop(EAllowShrinking::No);
		Dependencies.Reset();
		Registry->GetDependencies(Package, Dependencies, UE::AssetRegistry::EDependencyCategory::Package, UE::AssetRegistry::EDependencyQuery::Hard);
		for (const FName Dependency : Dependencies)
		{
			const FString Name = Dependency.ToString();
			if (Name.StartsWith(TEXT("/Script/")) || Visited.Contains(Dependency))
			{
				continue;
			}
			Visited.Add(Dependency);
			Stack.Add(Dependency);
			Assets.Reset();
			Registry->GetAssetsByPackageName(Dependency, Assets);
			if (Assets.Num() > 0)
			{
				OutPaths.Add(Assets[0].GetSoftObjectPath());
			}
		}
	}
}

void URRLayoutLoaderSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLoading())
	{
		TickLoad(DeltaTime);
	}
}

TStatId URRLayoutLoaderSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRLayoutLoaderSubsystem, STATGROUP_Tickables);
}

void URRLayoutLoaderSubsystem::Deinitialize()
{
	if (AssetHandle.IsValid())
	{
		AssetHandle->CancelHandle();
		AssetHandle.Reset();
	}
	Phase = ERRLayoutLoadPhase::Idle;
	Super::Deinitialize();
}

void URRLayoutLoaderSubsystem::TickLoad(float DeltaSeconds)
{
	if (!IsLoading())
	{
		return;
	}
	const double TickStart = FPlatformTime::Seconds();
	const double FrameMs = (TickStart - LastTickSeconds) * 1000.0;
	LastTickSeconds = TickStart;
	++Stats.Frames;
	Stats.MaxFrameMs = FMath::Max(Stats.MaxFrameMs, FrameMs);
	const URRLayoutLoaderSettings* Settings = GetDefault<URRLayoutLoaderSettings>();

	if (Phase == ERRLayoutLoadPhase::Loading)
	{
		const bool bParsed = ParseTask.IsValid() && ParseTask.IsCompleted();
		int32 Loaded = 0;
		int32 Requested = 0;
		if (AssetHandle.IsValid())
		{
			AssetHandle->GetLoadedCount(Loaded, Requested);
		}
		const bool bAssetsDone = !AssetHandle.IsValid() || AssetHandle->HasLoadCompleted() || AssetHandle->WasCanceled();
		const float AssetFraction = bAssetsDone ? 1.f : (Requested > 0 ? static_cast<float>(Loaded) / Requested : 0.f);
		UpdatePercent(bParsed ? 1.f : 0.f, AssetFraction, 0.f);
		if (bParsed && bAssetsDone)
		{
			Layout = ParseTask.GetResult();
			ParseTask = {};
			if (!Layout.IsValid() || !Layout->IsValid())
			{
				Finish(false, Layout.IsValid() ? Layout->Error : FString(TEXT("no layout")));
				return;
			}
			if (!SpawnAndBeginBuild())
			{
				return;
			}
			Phase = ERRLayoutLoadPhase::Building;
		}
	}
	else if (Phase == ERRLayoutLoadPhase::Building)
	{
		if (!IsValid(LoadedGrid))
		{
			Finish(false, TEXT("grid destroyed during the build"));
			return;
		}
		const bool bBuilt = LoadedGrid->StepIncrementalBuild(FMath::Max(0.1f, Settings->BuildBudgetMs) / 1000.0);
		UpdatePercent(1.f, 1.f, LoadedGrid->GetIncrementalBuildProgress());
		if (bBuilt)
		{
			const double StepMs = (FPlatformTime::Seconds() - TickStart) * 1000.0;
			Stats.MaxStepMs = FMath::Max(Stats.MaxStepMs, StepMs);
			Finish(true, FString());
			return;
		}
	}

	const double StepMs = (FPlatformTime::Seconds() - TickStart) * 1000.0;
	Stats.MaxStepMs = FMath::Max(Stats.MaxStepMs, StepMs);
	const int32 Decile = FMath::FloorToInt32(LoadPercent / 10.f);
	if (Settings->bLogEachFrame || Decile != LastLoggedDecile)
	{
		LastLoggedDecile = Decile;
		UE_LOG(LogRogueRigs, Log, TEXT("RRLayoutLoader: %5.1f %% (%s), frame %.2f ms, loader %.2f ms"),
			LoadPercent, RRLayoutLoaderPrivate::PhaseName(Phase), FrameMs, StepMs);
	}
}

bool URRLayoutLoaderSubsystem::SpawnAndBeginBuild()
{
	UWorld* World = GetWorld();
	UClass* GridClass = RequestedGridClass.Get();
	if (!World || !GridClass)
	{
		Finish(false, FString::Printf(TEXT("grid class %s did not load"), *RequestedGridClass.ToString()));
		return false;
	}
	ARRArenaGrid* Grid = World->SpawnActorDeferred<ARRArenaGrid>(GridClass, PendingTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Grid)
	{
		Finish(false, TEXT("grid spawn failed"));
		return false;
	}
	Grid->SetExternalBuild(true);
	Grid->FinishSpawning(PendingTransform);
	LoadedGrid = Grid;
	if (!Grid->BeginIncrementalBuild(*Layout))
	{
		Finish(false, TEXT("grid refused the layout"));
		return false;
	}
	Stats.BuildUnits = Grid->GetIncrementalBuildUnitCount();
	return true;
}

void URRLayoutLoaderSubsystem::UpdatePercent(float ParseFraction, float AssetFraction, float BuildFraction)
{
	const URRLayoutLoaderSettings* Settings = GetDefault<URRLayoutLoaderSettings>();
	const float Parse = FMath::Max(0.f, Settings->ParseWeight);
	const float Assets = FMath::Max(0.f, Settings->AssetWeight);
	const float Build = FMath::Max(0.f, Settings->BuildWeight);
	const float Total = Parse + Assets + Build;
	const float Fraction = Total > 0.f ? (Parse * ParseFraction + Assets * AssetFraction + Build * BuildFraction) / Total : BuildFraction;
	// 100 only once the grid is complete (Finish sets it).
	LoadPercent = FMath::Max(LoadPercent, FMath::Min(99.9f, 100.f * Fraction));
}

void URRLayoutLoaderSubsystem::ReadBackCounts()
{
	using namespace RRLayoutLoaderPrivate;
	Stats.FileCounts.Init(0, CellTypeCount);
	Stats.GridCounts.Init(0, CellTypeCount);
	if (!Layout.IsValid() || !IsValid(LoadedGrid))
	{
		Stats.bCountsMatch = false;
		return;
	}
	for (const ERRCellType Type : Layout->Cells)
	{
		const int32 Index = static_cast<int32>(Type);
		if (Index >= 0 && Index < CellTypeCount)
		{
			++Stats.FileCounts[Index];
		}
	}
	const FIntPoint Size = LoadedGrid->GetGridSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const FIntPoint Cell(X, Y);
			// An ore table deposit turns layout rock into its ore type (sys-grid-deposits): count it as the rock of the file.
			const ERRCellType Type = LoadedGrid->IsTableDepositCell(Cell) ? ERRCellType::Rock : LoadedGrid->GetCellType(Cell);
			const int32 Index = static_cast<int32>(Type);
			if (Index >= 0 && Index < CellTypeCount)
			{
				++Stats.GridCounts[Index];
			}
		}
	}
	Stats.bCountsMatch = Size == FIntPoint(Layout->Width, Layout->Height) && Stats.FileCounts == Stats.GridCounts;
}

void URRLayoutLoaderSubsystem::Finish(bool bSuccess, const FString& Reason)
{
	Stats.TotalSeconds = FPlatformTime::Seconds() - StartSeconds;
	ARRArenaGrid* Grid = nullptr;
	if (bSuccess)
	{
		ReadBackCounts();
		Phase = ERRLayoutLoadPhase::Done;
		LoadPercent = 100.f;
		Grid = LoadedGrid;
		FString Counts;
		for (int32 Index = 0; Index < Stats.FileCounts.Num(); ++Index)
		{
			Counts += FString::Printf(TEXT("%s%s %d/%d"), Counts.IsEmpty() ? TEXT("") : TEXT(", "),
				*UEnum::GetDisplayValueAsText(static_cast<ERRCellType>(Index)).ToString(), Stats.FileCounts[Index], Stats.GridCounts[Index]);
		}
		UE_LOG(LogRogueRigs, Log, TEXT("RRLayoutLoader: 100.0 %% (done), %s loaded in %.2f s over %d frames; longest frame %.2f ms, longest loader step %.2f ms; %d build units; assets %d (%d already in memory)."),
			*LayoutFilePath, Stats.TotalSeconds, Stats.Frames, Stats.MaxFrameMs, Stats.MaxStepMs, Stats.BuildUnits, Stats.AssetsRequested, Stats.AssetsAlreadyLoaded);
		UE_LOG(LogRogueRigs, Log, TEXT("RRLayoutLoader: cell counts file/grid: %s; match %s."), *Counts, Stats.bCountsMatch ? TEXT("yes") : TEXT("NO"));
	}
	else
	{
		Phase = ERRLayoutLoadPhase::Failed;
		if (IsValid(LoadedGrid))
		{
			LoadedGrid->Destroy();
		}
		LoadedGrid = nullptr;
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutLoader: load of %s failed at %.1f %%: %s"), *LayoutFilePath, LoadPercent, *Reason);
	}
	// The built grid now holds its assets through its class; the request handle is no longer needed.
	if (AssetHandle.IsValid())
	{
		AssetHandle->ReleaseHandle();
		AssetHandle.Reset();
	}
	Layout.Reset();
	OnLoadCompleted.Broadcast(Grid, bSuccess);
}
