// Rogue Rigs - test-only grid granularity tools: subdivide the arena grid at runtime and benchmark its per-cell systems (task-023-01).

#include "Debug/RRGridStress.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionStatsRow.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "Grid/RRArenaGrid.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformTime.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "TimerManager.h"
#include "World/RRWorld.h"

namespace RRGridStress
{
	/**
	 * Protected-member access for the stress tool only (same idiom as Tests/RRTestAccess.h): a derived struct may name a protected
	 * member, so a pointer to member applied to any grid reads or writes it. Never instantiated.
	 */
	struct FGridAccess : public ARRArenaGrid
	{
		static TArray<ERRCellType>& CellArray(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::Cells); }
		static int32& Width(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridWidth); }
		static int32& Height(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridHeight); }
		static FIntPoint& Spawn(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::SpawnCell); }
		static float& Size(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CellSize); }
		static void Rebuild(ARRArenaGrid* Grid) { (Grid->*(&FGridAccess::BuildInstances))(); }

		/** Every instanced component of the grid with its name (render HISMs then hidden collision ISMs). */
		static TArray<TPair<FString, UInstancedStaticMeshComponent*>> Components(ARRArenaGrid* Grid)
		{
			return {
				{ TEXT("Floor"), Grid->*(&FGridAccess::FloorInstances) },
				{ TEXT("Rock"), Grid->*(&FGridAccess::RockInstances) },
				{ TEXT("Ore"), Grid->*(&FGridAccess::OreInstances) },
				{ TEXT("HealOre"), Grid->*(&FGridAccess::HealOreInstances) },
				{ TEXT("SpecialOre"), Grid->*(&FGridAccess::SpecialOreInstances) },
				{ TEXT("Barrel"), Grid->*(&FGridAccess::BarrelInstances) },
				{ TEXT("RockCollision"), Grid->*(&FGridAccess::RockCollision) },
				{ TEXT("OreCollision"), Grid->*(&FGridAccess::OreCollision) },
				{ TEXT("HealOreCollision"), Grid->*(&FGridAccess::HealOreCollision) },
				{ TEXT("SpecialOreCollision"), Grid->*(&FGridAccess::SpecialOreCollision) },
				{ TEXT("BarrelCollision"), Grid->*(&FGridAccess::BarrelCollision) },
			};
		}
	};

	/** Flow-field access: lifts the once-per-frame guard so a computation can be forced and timed. Never instantiated. */
	struct FFlowAccess : public URRFlowFieldSubsystem
	{
		static uint64& ComputeFrame(URRFlowFieldSubsystem* Flow) { return Flow->*(&FFlowAccess::LastComputeFrame); }
	};

	/** Used physical memory of the process in MB. */
	double UsedPhysicalMB()
	{
		return static_cast<double>(FPlatformMemory::GetStats().UsedPhysical) / (1024.0 * 1024.0);
	}

	/** Mean and max of Samples. */
	void MeanMax(const TArray<double>& Samples, double& OutMean, double& OutMax)
	{
		OutMean = 0.0;
		OutMax = 0.0;
		for (const double Value : Samples)
		{
			OutMean += Value;
			OutMax = FMath::Max(OutMax, Value);
		}
		OutMean = Samples.IsEmpty() ? 0.0 : OutMean / Samples.Num();
	}

	TArray<FVector> DrawBlockPoints(const ARRArenaGrid* Grid, const FVector& Center, int32 Count, float MinRange, float MaxRange, int32 Seed)
	{
		TArray<FVector> Points;
		FRandomStream Stream(Seed);
		for (int32 Try = 0; Try < Count * 50 && Points.Num() < Count; ++Try)
		{
			const float Angle = Stream.FRandRange(0.f, UE_TWO_PI);
			const float Range = Stream.FRandRange(MinRange, MaxRange);
			const FVector Point = Center + FVector(FMath::Cos(Angle) * Range, FMath::Sin(Angle) * Range, 0.f);
			if (Grid->IsBlockCell(Grid->GetCellAtWorld(Point)))
			{
				Points.Add(Point);
			}
		}
		return Points;
	}

	bool SubdivideGrid(ARRArenaGrid* Grid, int32 Factor, bool bDisableNavigation)
	{
		if (!Grid || Factor < 1 || Grid->GetGridSize().X <= 0)
		{
			return false;
		}
		const double MemoryBefore = UsedPhysicalMB();
		const double StartSeconds = FPlatformTime::Seconds();

		const TArray<ERRCellType> OldCells = FGridAccess::CellArray(Grid);
		const int32 OldWidth = FGridAccess::Width(Grid);
		const int32 OldHeight = FGridAccess::Height(Grid);
		const int32 NewWidth = OldWidth * Factor;
		const int32 NewHeight = OldHeight * Factor;
		const int32 Middle = Factor / 2;

		TArray<ERRCellType> NewCells;
		NewCells.Init(ERRCellType::Floor, NewWidth * NewHeight);
		FIntPoint NewSpawn(-1, -1);
		for (int32 Y = 0; Y < NewHeight; ++Y)
		{
			for (int32 X = 0; X < NewWidth; ++X)
			{
				const ERRCellType Type = OldCells[(Y / Factor) * OldWidth + X / Factor];
				const bool bMiddle = X % Factor == Middle && Y % Factor == Middle;
				ERRCellType NewType = Type;
				if (Type == ERRCellType::Spawn)
				{
					NewType = bMiddle ? ERRCellType::Spawn : ERRCellType::Floor;
				}
				else if (Type == ERRCellType::Barrel)
				{
					NewType = bMiddle ? ERRCellType::Barrel : ERRCellType::Rock;
				}
				if (NewType == ERRCellType::Spawn)
				{
					NewSpawn = FIntPoint(X, Y);
				}
				NewCells[Y * NewWidth + X] = NewType;
			}
		}

		FGridAccess::CellArray(Grid) = MoveTemp(NewCells);
		FGridAccess::Width(Grid) = NewWidth;
		FGridAccess::Height(Grid) = NewHeight;
		FGridAccess::Spawn(Grid) = NewSpawn;
		FGridAccess::Size(Grid) = FGridAccess::Size(Grid) / Factor;
		if (bDisableNavigation)
		{
			for (const TPair<FString, UInstancedStaticMeshComponent*>& Pair : FGridAccess::Components(Grid))
			{
				if (Pair.Value)
				{
					Pair.Value->SetCanEverAffectNavigation(false);
				}
			}
		}
		FGridAccess::Rebuild(Grid);
		const double BuildMs = (FPlatformTime::Seconds() - StartSeconds) * 1000.0;

		if (UWorld* World = Grid->GetWorld())
		{
			if (URRFlowFieldSubsystem* Flow = World->GetSubsystem<URRFlowFieldSubsystem>())
			{
				Flow->SetGrid(Grid);
			}
			ForceFlowFieldCompute(World);
		}
		UE_LOG(LogRogueRigs, Display, TEXT("GridStress: subdivided by %d%s: %d x %d cells of %.2f uu, rebuild %.1f ms, used physical %.0f -> %.0f MB."),
			Factor, bDisableNavigation ? TEXT(" (no navigation)") : TEXT(""), NewWidth, NewHeight, Grid->GetCellSize(), BuildMs, MemoryBefore, UsedPhysicalMB());
		return true;
	}

	float ForceFlowFieldCompute(UWorld* World)
	{
		URRFlowFieldSubsystem* Flow = World ? World->GetSubsystem<URRFlowFieldSubsystem>() : nullptr;
		if (!Flow)
		{
			return -1.f;
		}
		const int32 CountBefore = Flow->GetComputeCount();
		Flow->MarkDirty();
		FFlowAccess::ComputeFrame(Flow) = MAX_uint64;
		Flow->UpdateField();
		return Flow->GetComputeCount() != CountBefore ? Flow->GetLastComputeMilliseconds() : -1.f;
	}

	/** Logs the grid size, the cell counts per type and the instance count of every component. */
	void LogGridInfo(ARRArenaGrid* Grid)
	{
		TMap<ERRCellType, int32> TypeCounts;
		for (const ERRCellType Type : FGridAccess::CellArray(Grid))
		{
			++TypeCounts.FindOrAdd(Type);
		}
		FString Types;
		for (const TPair<ERRCellType, int32>& Pair : TypeCounts)
		{
			Types += FString::Printf(TEXT(" %s=%d"), *UEnum::GetValueAsString(Pair.Key), Pair.Value);
		}
		FString Instances;
		int32 CollisionInstances = 0;
		for (const TPair<FString, UInstancedStaticMeshComponent*>& Pair : FGridAccess::Components(Grid))
		{
			const int32 Count = Pair.Value ? Pair.Value->GetInstanceCount() : 0;
			Instances += FString::Printf(TEXT(" %s=%d"), *Pair.Key, Count);
			if (Pair.Key.EndsWith(TEXT("Collision")) || Pair.Key == TEXT("Floor"))
			{
				CollisionInstances += Count;
			}
		}
		const FIntPoint Size = Grid->GetGridSize();
		UE_LOG(LogRogueRigs, Display, TEXT("GridBench: grid %d x %d, cell %.2f uu, cells%s"), Size.X, Size.Y, Grid->GetCellSize(), *Types);
		UE_LOG(LogRogueRigs, Display, TEXT("GridBench: instances%s, colliding instances %d, used physical %.0f MB"), *Instances,
			CollisionInstances, UsedPhysicalMB());
	}

	void RunBench(UWorld* World, int32 Iterations)
	{
		ARRArenaGrid* Grid = RRWorld::FindArenaGrid(World);
		const ARRMiner* Miner = RRWorld::FindLivingMiner(World);
		if (!Grid || !Miner)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("GridBench: needs an arena grid and a living miner."));
			return;
		}
		Iterations = FMath::Max(1, Iterations);
		const FVector Origin = Miner->GetActorLocation();
		LogGridInfo(Grid);

		// Flow field: full BFS from the miner, forced.
		TArray<double> FlowMs;
		for (int32 Index = 0; Index < Iterations; ++Index)
		{
			FlowMs.Add(ForceFlowFieldCompute(World));
		}
		double Mean = 0.0;
		double Max = 0.0;
		MeanMax(FlowMs, Mean, Max);
		UE_LOG(LogRogueRigs, Display, TEXT("GridBench: flow field BFS mean %.3f max %.3f ms (%d runs)"), Mean, Max, Iterations);

		// Spawner candidate scan (band around the view, full-grid scan); the field is already computed this frame.
		if (ARREnemySpawner* Spawner = TActorIterator<ARREnemySpawner>(World) ? *TActorIterator<ARREnemySpawner>(World) : nullptr)
		{
			TArray<double> ScanMs;
			TArray<FIntPoint> Candidates;
			for (int32 Index = 0; Index < Iterations; ++Index)
			{
				Candidates.Reset();
				const double Start = FPlatformTime::Seconds();
				Spawner->GetSpawnCandidates(Candidates);
				ScanMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
			}
			MeanMax(ScanMs, Mean, Max);
			UE_LOG(LogRogueRigs, Display, TEXT("GridBench: spawn candidate scan mean %.3f max %.3f ms, %d candidates"), Mean, Max, Candidates.Num());
		}

		// Grid line of sight: same random rays (200-2000 uu) from the miner at every cell size.
		{
			constexpr int32 RayCount = 20000;
			FRandomStream Stream(BenchSeed);
			int32 Blocked = 0;
			const double Start = FPlatformTime::Seconds();
			for (int32 Index = 0; Index < RayCount; ++Index)
			{
				const float Angle = Stream.FRandRange(0.f, UE_TWO_PI);
				const float Range = Stream.FRandRange(200.f, 2000.f);
				const FVector To = Origin + FVector(FMath::Cos(Angle) * Range, FMath::Sin(Angle) * Range, 0.f);
				Blocked += Grid->HasLineOfSight(Origin, To) ? 0 : 1;
			}
			const double TotalMs = (FPlatformTime::Seconds() - Start) * 1000.0;
			UE_LOG(LogRogueRigs, Display, TEXT("GridBench: line of sight %.3f us per ray (%d rays 200-2000 uu, %d blocked)"),
				TotalMs * 1000.0 / RayCount, RayCount, Blocked);
		}

		// Destruction breakdown: the three component operations of DestroyCell timed alone, then the grid is rebuilt from its
		// unchanged cells so the maps match the instances again.
		{
			TArray<TPair<FString, UInstancedStaticMeshComponent*>> Components = FGridAccess::Components(Grid);
			UInstancedStaticMeshComponent* RockRender = Components[1].Value;
			UInstancedStaticMeshComponent* RockCollision = Components[6].Value;
			UInstancedStaticMeshComponent* Floor = Components[0].Value;
			TArray<double> RenderMs;
			TArray<double> CollisionMs;
			TArray<double> FloorMs;
			const FTransform FloorTransform(FRotator::ZeroRotator, FVector(Grid->GetCellSize() * 0.5f, Grid->GetCellSize() * 0.5f, -100.f),
				FVector(Grid->GetCellSize() / 100.f, Grid->GetCellSize() / 100.f, 2.f));
			for (int32 Index = 0; Index < Iterations && RockRender && RockCollision && Floor && RockRender->GetInstanceCount() > Index; ++Index)
			{
				// A middle index: remove-at-swap moves the last instance, like a real destruction.
				const int32 Target = RockRender->GetInstanceCount() / 2;
				double Start = FPlatformTime::Seconds();
				RockRender->RemoveInstance(Target);
				RenderMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
				Start = FPlatformTime::Seconds();
				RockCollision->RemoveInstance(Target);
				CollisionMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
				Start = FPlatformTime::Seconds();
				Floor->AddInstance(FloorTransform, /*bWorldSpace*/ false);
				FloorMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
			}
			double RenderMean = 0.0, RenderMax = 0.0, CollisionMean = 0.0, CollisionMax = 0.0, FloorMean = 0.0, FloorMax = 0.0;
			MeanMax(RenderMs, RenderMean, RenderMax);
			MeanMax(CollisionMs, CollisionMean, CollisionMax);
			MeanMax(FloorMs, FloorMean, FloorMax);
			UE_LOG(LogRogueRigs, Display,
				TEXT("GridBench: destroy breakdown, rock HISM RemoveInstance mean %.3f max %.3f ms, rock collision ISM RemoveInstance mean %.3f max %.3f ms, floor HISM AddInstance mean %.3f max %.3f ms (%d runs)"),
				RenderMean, RenderMax, CollisionMean, CollisionMax, FloorMean, FloorMax, RenderMs.Num());
			FGridAccess::Rebuild(Grid);
			ForceFlowFieldCompute(World);
		}

		const int32 PickupsBefore = [World]() { int32 Count = 0; for (TActorIterator<ARRPickup> It(World); It; ++It) { ++Count; } return Count; }();

		// Single-cell destruction (dig): the block cells nearest to the miner, each followed by the forced flow-field rebuild it causes.
		{
			TArray<TPair<double, FIntPoint>> Blocks;
			const FIntPoint Size = Grid->GetGridSize();
			for (int32 Y = 0; Y < Size.Y; ++Y)
			{
				for (int32 X = 0; X < Size.X; ++X)
				{
					if (Grid->IsBlockCell(FIntPoint(X, Y)) && Grid->GetCellType(FIntPoint(X, Y)) != ERRCellType::Barrel)
					{
						Blocks.Emplace(FVector::DistSquared2D(Grid->GetCellCenter(X, Y), Origin), FIntPoint(X, Y));
					}
				}
			}
			Blocks.Sort([](const TPair<double, FIntPoint>& A, const TPair<double, FIntPoint>& B) { return A.Key < B.Key; });
			TArray<double> DestroyMs;
			TArray<double> RebuildMs;
			for (int32 Index = 0; Index < Iterations * 5 && Blocks.IsValidIndex(Index); ++Index)
			{
				const double Start = FPlatformTime::Seconds();
				Grid->DestroyCell(Blocks[Index].Value, ERRTerrainDamageSource::Drone);
				DestroyMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
				RebuildMs.Add(ForceFlowFieldCompute(World));
			}
			double RebuildMean = 0.0;
			double RebuildMax = 0.0;
			MeanMax(DestroyMs, Mean, Max);
			MeanMax(RebuildMs, RebuildMean, RebuildMax);
			UE_LOG(LogRogueRigs, Display, TEXT("GridBench: dig destroy one cell mean %.3f max %.3f ms, flow rebuild after it mean %.3f max %.3f ms (%d cells)"),
				Mean, Max, RebuildMean, RebuildMax, DestroyMs.Num());
		}

		// Explosion terrain side: DestroyCellsInRadius with the barrel radius at the same world points (ring 1500-5000 uu).
		{
			float Radius = 350.f;
			if (URRExplosionSubsystem* Explosions = World->GetSubsystem<URRExplosionSubsystem>())
			{
				FRRExplosionStatsRow Stats;
				if (Explosions->GetExplosionStats(TEXT("Barrel"), Stats))
				{
					Radius = Stats.Radius;
				}
			}
			const TArray<FVector> Points = DrawBlockPoints(Grid, Origin, Iterations, 1500.f, 5000.f, BenchSeed);
			TArray<double> ExplosionMs;
			int32 Destroyed = 0;
			for (const FVector& Point : Points)
			{
				const double Start = FPlatformTime::Seconds();
				Destroyed += Grid->DestroyCellsInRadius(Point, Radius, ERRTerrainDamageSource::Barrel);
				ExplosionMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
			}
			MeanMax(ExplosionMs, Mean, Max);
			const int32 PickupsAfter = [World]() { int32 Count = 0; for (TActorIterator<ARRPickup> It(World); It; ++It) { ++Count; } return Count; }();
			UE_LOG(LogRogueRigs, Display,
				TEXT("GridBench: explosion terrain (radius %.0f) mean %.3f max %.3f ms, %d explosions, %d blocks (%.1f per explosion, %.2f us per block), pickups spawned by bench %d"),
				Radius, Mean, Max, Points.Num(), Destroyed, Points.IsEmpty() ? 0.f : static_cast<float>(Destroyed) / Points.Num(),
				Destroyed > 0 ? Mean * Points.Num() * 1000.0 / Destroyed : 0.0, PickupsAfter - PickupsBefore);
		}
		UE_LOG(LogRogueRigs, Display, TEXT("GridBench: done, used physical %.0f MB"), UsedPhysicalMB());
	}

#if !UE_BUILD_SHIPPING
	void HandleSubdivide(const TArray<FString>& Args, UWorld* World)
	{
		const int32 Factor = Args.IsEmpty() ? 1 : FCString::Atoi(*Args[0]);
		const bool bNoNavigation = Args.IsValidIndex(1) && FCString::Atoi(*Args[1]) != 0;
		if (!SubdivideGrid(RRWorld::FindArenaGrid(World), Factor, bNoNavigation))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Subdivide <Factor>: needs a built arena grid and Factor >= 1."));
		}
	}

	void HandleBench(const TArray<FString>& Args, UWorld* World)
	{
		const int32 Iterations = Args.IsValidIndex(0) ? FCString::Atoi(*Args[0]) : 20;
		const bool bQuit = Args.IsValidIndex(1) && FCString::Atoi(*Args[1]) != 0;
		const float Delay = Args.IsValidIndex(2) ? FCString::Atof(*Args[2]) : 3.f;
		if (!World)
		{
			return;
		}
		// Wait a little so the instance trees and the first frames settle before timing and reading memory.
		FTimerHandle Handle;
		TWeakObjectPtr<UWorld> WeakWorld(World);
		World->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([WeakWorld, Iterations, bQuit]()
		{
			if (UWorld* BenchWorld = WeakWorld.Get())
			{
				RunBench(BenchWorld, Iterations);
			}
			if (bQuit)
			{
				FPlatformMisc::RequestExit(false, TEXT("rr.Stress.GridBench"));
			}
		}), FMath::Max(0.01f, Delay), false);
	}

	FAutoConsoleCommandWithWorldAndArgs CmdSubdivide(TEXT("rr.Stress.Subdivide"),
		TEXT("rr.Stress.Subdivide <Factor> [NoNav]: splits every arena cell into Factor x Factor cells (same world size); NoNav 1 turns off grid navigation relevance. Test only."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleSubdivide), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdBench(TEXT("rr.Stress.GridBench"),
		TEXT("rr.Stress.GridBench [Iterations] [Quit] [DelaySeconds]: logs grid micro-benchmarks (destroys terrain), test only."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleBench), ECVF_Cheat);
#endif
}
