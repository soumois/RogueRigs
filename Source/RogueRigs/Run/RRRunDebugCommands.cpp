// Rogue Rigs - run debug console commands (compiled out of shipping): rr.Run.Status (logs the run state), rr.Run.NextLevel
// (successful extraction, snapshot, next level, then the current map reopened once the next layout is prepared: proves the
// carry-over and the async layout before the transition flow exists).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Containers/Ticker.h"
#include "Debug/RRCheats.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Run/RRRunSubsystem.h"

namespace RRRunDebug
{
	/** rr.Run.Status: logs level index, layout, last-level flag, pending snapshot and run ore. */
	FAutoConsoleCommand StatusCommand(
		TEXT("rr.Run.Status"),
		TEXT("rr.Run.Status: logs the run state (level index, layout, last level, pending snapshot, run secured ore)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			const URRRunSubsystem* Run = URRRunSubsystem::Get(RRCheats::FindCheatWorld());
			if (!Run)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Run.Status: no game instance run subsystem."));
				return;
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Status: run %s, level %d/%d, layout %s, last level %d, snapshot pending %d, run secured ore %d."),
				Run->IsRunInProgress() ? TEXT("in progress") : TEXT("none"), Run->GetLevelIndex(), Run->GetLevelCount(), *Run->GetCurrentLayoutName(),
				Run->IsLastLevel() ? 1 : 0, Run->HasPendingSnapshot() ? 1 : 0, Run->GetRunSecuredOre());
		}),
		ECVF_Cheat);

	/**
	 * rr.Run.NextLevel: EndRun(true) when the level is still running (carried ore secured), CaptureLevelEndSnapshot,
	 * AdvanceToNextLevel (which requests the next layout), then, once IsLayoutReady (polled on the core ticker, as the
	 * intermission Continue will), OpenLevel on the current map; its grid builds from the prepared layout.
	 */
	FAutoConsoleCommand NextLevelCommand(
		TEXT("rr.Run.NextLevel"),
		TEXT("rr.Run.NextLevel: ends the level as a successful extraction, captures the miner snapshot, advances the run and reopens the current map."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = RRCheats::FindCheatWorld();
			URRRunSubsystem* Run = URRRunSubsystem::Get(World);
			ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
			if (!Run || !Miner || Run->IsLastLevel())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Run.NextLevel: refused (no run subsystem, no miner or last level)."));
				return;
			}
			if (URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>(); RunFlow && !RunFlow->IsRunEnded())
			{
				RunFlow->EndRun(true);
			}
			if (!Run->CaptureLevelEndSnapshot(Miner) || !Run->AdvanceToNextLevel())
			{
				return;
			}
			const FString LevelName = UGameplayStatics::GetCurrentLevelName(World, true);
			const int32 Level = Run->GetLevelIndex();
			TWeakObjectPtr<UWorld> WeakWorld(World);
			TWeakObjectPtr<URRRunSubsystem> WeakRun(Run);
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakWorld, WeakRun, LevelName, Level](float)
			{
				UWorld* TickWorld = WeakWorld.Get();
				URRRunSubsystem* TickRun = WeakRun.Get();
				if (!TickWorld || !TickRun || TickRun->GetLevelIndex() != Level)
				{
					return false;
				}
				if (TickRun->IsLayoutPending(Level))
				{
					return true;
				}
				UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.NextLevel: layout of level %d %s, reopening %s."), Level,
					TickRun->IsLayoutReady(Level) ? TEXT("ready") : TEXT("failed (the grid falls back to its own layout)"), *LevelName);
				UGameplayStatics::OpenLevel(TickWorld, FName(*LevelName));
				return false;
			}));
		}),
		ECVF_Cheat);
}

#endif
