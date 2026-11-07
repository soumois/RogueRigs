// Rogue Rigs - debug command of the asynchronous layout loader (task-493-01, debug only, compiled out of shipping):
// rr.Level.LoadLayout <layout path or name> [keep] [stay] loads a layout with URRLayoutLoaderSubsystem in the current world.

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Containers/Ticker.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RRLayoutLoaderSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "RogueRigs.h"

#if !UE_BUILD_SHIPPING

namespace RRLayoutLoaderDebug
{
	/** Puts the player's pawn on the spawn cell of Grid (floor top + capsule half height). */
	void MovePlayerToSpawn(UWorld* World, ARRArenaGrid* Grid)
	{
		APlayerController* Controller = World ? World->GetFirstPlayerController() : nullptr;
		APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
		if (!Pawn || !Grid || Grid->GetSpawnCell().X < 0)
		{
			return;
		}
		const ACharacter* Character = Cast<ACharacter>(Pawn);
		const float HalfHeight = Character && Character->GetCapsuleComponent() ? Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 100.f;
		const FIntPoint Cell = Grid->GetSpawnCell();
		const FVector Location = Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f);
		Pawn->SetActorLocation(Location, /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.LoadLayout: player moved to the spawn cell (%d, %d) of %s."), Cell.X, Cell.Y, *Grid->GetLayoutName());
	}

	void LoadLayout(const TArray<FString>& Args, UWorld* World)
	{
		URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(World);
		if (!Loader || Args.Num() < 1)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Level.LoadLayout <layout path or name> [keep]: no game world or no path."));
			return;
		}
		bool bKeep = false;
		bool bStay = false;
		for (int32 Index = 1; Index < Args.Num(); ++Index)
		{
			bKeep |= Args[Index].Equals(TEXT("keep"), ESearchCase::IgnoreCase);
			bStay |= Args[Index].Equals(TEXT("stay"), ESearchCase::IgnoreCase);
		}
		if (const APlayerController* Controller = World->GetFirstPlayerController(); Controller && Controller->GetPawn())
		{
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Level.LoadLayout: player at %s before the load."), *Controller->GetPawn()->GetActorLocation().ToCompactString());
		}

		// The grid placed in the map gives the transform; unless "keep", it is hidden and stops colliding once the new grid is built.
		FTransform Transform = FTransform::Identity;
		TWeakObjectPtr<ARRArenaGrid> MapGrid;
		for (TActorIterator<ARRArenaGrid> It(World); It; ++It)
		{
			if (!It->IsExternalBuild())
			{
				Transform = It->GetActorTransform();
				MapGrid = *It;
				break;
			}
		}
		if (!Loader->StartLoad(Args[0], Transform))
		{
			return;
		}

		// Watch the load and put the player on the new spawn cell at the end.
		TWeakObjectPtr<UWorld> WeakWorld = World;
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakWorld, MapGrid, bKeep, bStay](float)
		{
			UWorld* TickWorld = WeakWorld.Get();
			URRLayoutLoaderSubsystem* TickLoader = URRLayoutLoaderSubsystem::Get(TickWorld);
			if (!TickLoader || TickLoader->IsLoading())
			{
				return TickLoader != nullptr;
			}
			if (TickLoader->GetPhase() != ERRLayoutLoadPhase::Done)
			{
				return false;
			}
			if (ARRArenaGrid* Old = MapGrid.Get(); Old && !bKeep)
			{
				Old->SetActorHiddenInGame(true);
				Old->SetActorEnableCollision(false);
			}
			if (!bStay)
			{
				MovePlayerToSpawn(TickWorld, TickLoader->GetLoadedGrid());
			}
			return false;
		}));
	}

	static FAutoConsoleCommandWithWorldAndArgs LoadLayoutCommand(
		TEXT("rr.Level.LoadLayout"),
		TEXT("rr.Level.LoadLayout <layout path or name> [keep] [stay]: loads the layout asynchronously (URRLayoutLoaderSubsystem) at the transform of the map grid, which is hidden at the end unless 'keep'; the player is moved to the new spawn cell at the end unless 'stay'. Path: absolute, relative to the project folder, or a name in Content/RogueRigs/Data/Levels."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&LoadLayout));
}

#endif
