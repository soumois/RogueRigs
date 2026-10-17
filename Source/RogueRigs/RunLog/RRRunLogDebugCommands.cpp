// Rogue Rigs - run log debug console command (compiled out of shipping): rr.RunLog.Dump logs the current level counters.

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Debug/RRCheats.h"
#include "HAL/IConsoleManager.h"
#include "RogueRigs.h"
#include "RunLog/RRRunLogSubsystem.h"

namespace RRRunLogDebug
{
	/** rr.RunLog.Dump: logs every counter of the current level (URRRunLogSubsystem::LogCurrentLevelStats). */
	FAutoConsoleCommand DumpCommand(
		TEXT("rr.RunLog.Dump"),
		TEXT("rr.RunLog.Dump: logs the run log counters of the current level (terrain, damage dealt, kills, upgrades, damage taken, XP)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			const URRRunLogSubsystem* RunLog = URRRunLogSubsystem::Get(RRCheats::FindCheatWorld());
			if (!RunLog)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.RunLog.Dump: no game instance run log subsystem."));
				return;
			}
			RunLog->LogCurrentLevelStats(TEXT("rr.RunLog.Dump"));
		}),
		ECVF_Cheat);
}

#endif // !UE_BUILD_SHIPPING
