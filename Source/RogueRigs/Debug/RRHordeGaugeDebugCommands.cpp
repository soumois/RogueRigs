// Rogue Rigs - horde kill gauge console command for visual checks (debug only, compiled out of shipping): rr.Debug.HordeGauge
// counts kills into the gauge of the run's spawner as real kills would, and logs the gauge state (task-544).

#include "CoreMinimal.h"
#include "Debug/RRCheats.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "RogueRigs.h"

#if !UE_BUILD_SHIPPING

namespace RRHordeGaugeCommands
{
	/** rr.Debug.HordeGauge [Kills]: Kills (default 10) kills counted into the horde gauge, weighted as real kills; logs the gauge. */
	FAutoConsoleCommandWithWorldAndArgs HordeGaugeCommand(
		TEXT("rr.Debug.HordeGauge"),
		TEXT("rr.Debug.HordeGauge [Kills]: counts Kills (default 10) robot kills into the horde gauge of the run's spawner (weighted as real kills, only while it fills); logs the gauge state (visual checks of the horde gauge)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			const URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
			ARREnemySpawner* Spawner = RunFlow ? RunFlow->GetSpawner() : nullptr;
			if (!Spawner || !Spawner->UsesHordeGauge())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.HordeGauge: refused (no run spawner or no kill gauge)."));
				return;
			}
			const int32 Kills = Args.Num() > 0 ? FMath::Max(FCString::Atoi(*Args[0]), 0) : 10;
			int32 Counted = 0;
			for (int32 Index = 0; Index < Kills; ++Index)
			{
				Counted += Spawner->NotifyRobotKilled() ? 1 : 0;
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.HordeGauge: %d of %d kills counted; gauge %s, %.0f / %d (fill %.2f)."), Counted, Kills,
				*UEnum::GetValueAsString(Spawner->GetHordeGaugeState()), Spawner->GetHordeGaugeKills(), Spawner->GetHordeGaugeTarget(), Spawner->GetHordeGaugeFill());
		}),
		ECVF_Cheat);
}

#endif
