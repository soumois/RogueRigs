// Rogue Rigs - barrel fuse console command for unattended package checks (debug only, compiled out of shipping): rr.Debug.FuseBarrel
// moves the player miner next to the nearest barrel and lights its fuse as a shot would, and logs the decal materials the fuse draws
// with (telegraph material and warmed set), the state a frame cannot name (task-431-01).

#include "CoreMinimal.h"
#include "Debug/RRCheats.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Grid/RRArenaGrid.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInterface.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Telegraph/RRDecalWarmUp.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "World/RRWorld.h"

#if !UE_BUILD_SHIPPING

namespace RRBarrelFuseCommands
{
	/** The barrel cell nearest to Location, not fusing yet; false when none is left. */
	bool FindNearestBarrel(const ARRArenaGrid& Grid, const FVector& Location, FIntPoint& OutCell)
	{
		double BestDistance = TNumericLimits<double>::Max();
		bool bFound = false;
		const FIntPoint Size = Grid.GetGridSize();
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				const FIntPoint Cell(X, Y);
				if (Grid.GetCellType(Cell) != ERRCellType::Barrel || Grid.IsBarrelFusing(Cell))
				{
					continue;
				}
				const double Distance = FVector::DistSquared2D(Grid.GetCellCenter(X, Y), Location);
				if (Distance < BestDistance)
				{
					BestDistance = Distance;
					OutCell = Cell;
					bFound = true;
				}
			}
		}
		return bFound;
	}

	/** Moves the miner to the first free spot Distance away from Target (keeps its place when every spot is a block). */
	void MoveMinerNear(ARRMiner& Miner, const ARRArenaGrid& Grid, const FVector& Target, float Distance)
	{
		for (int32 Turn = 0; Turn < 12; ++Turn)
		{
			const FVector Candidate = Target + FVector::XAxisVector.RotateAngleAxis(30.f * static_cast<float>(Turn), FVector::ZAxisVector) * Distance;
			if (!Grid.IsBlockCell(Grid.GetCellAtWorld(Candidate)))
			{
				Miner.SetActorLocation(FVector(Candidate.X, Candidate.Y, Miner.GetActorLocation().Z), false, nullptr, ETeleportType::TeleportPhysics);
				return;
			}
		}
	}

	/** rr.Debug.FuseBarrel [MinerDistance]: the nearest barrel lights as under a shot, with the miner MinerDistance uu from it (default 400). */
	FAutoConsoleCommandWithWorldAndArgs FuseBarrelCommand(
		TEXT("rr.Debug.FuseBarrel"),
		TEXT("rr.Debug.FuseBarrel [MinerDistance]: moves the player miner MinerDistance uu (default 400) from the nearest barrel and lights its fuse as a shot would; logs the decal materials of the fuse (visual checks of the barrel explosion)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
			ARRArenaGrid* Grid = World ? RRWorld::FindArenaGrid(World) : nullptr;
			FIntPoint Cell;
			if (!Miner || !Grid || !FindNearestBarrel(*Grid, Miner->GetActorLocation(), Cell))
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.FuseBarrel: refused (no player miner, no grid or no barrel left)."));
				return;
			}
			const float Distance = Args.Num() > 0 ? FCString::Atof(*Args[0]) : 400.f;
			MoveMinerNear(*Miner, *Grid, Grid->GetCellCenter(Cell.X, Cell.Y), Distance);
			const bool bLit = Grid->DestroyCell(Cell, ERRTerrainDamageSource::Shot);

			const URRGroundTelegraphComponent* Telegraph = Grid->GetBarrelTelegraph(Cell);
			const UMaterialInterface* DecalMaterial = Telegraph ? Telegraph->GetDecalMaterial() : nullptr;
			FString Warmed;
			for (TActorIterator<ARRDecalWarmUp> It(World); It; ++It)
			{
				for (const UMaterialInterface* Material : It->GetWarmedMaterials())
				{
					Warmed += FString::Printf(TEXT(" %s"), *GetNameSafe(Material));
				}
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.FuseBarrel: cell (%d, %d) %s, world %s, frame %llu, telegraph %s visible %d, decal material %s (base %s), warmed:%s."),
				Cell.X, Cell.Y, bLit ? TEXT("lit") : TEXT("not lit"), *World->GetName(), GFrameCounter, *GetNameSafe(Telegraph), Telegraph && Telegraph->IsVisible() ? 1 : 0,
				*GetNameSafe(DecalMaterial), DecalMaterial ? *GetNameSafe(DecalMaterial->GetMaterial()) : TEXT("none"), *Warmed);
		}),
		ECVF_Cheat);
}

#endif
