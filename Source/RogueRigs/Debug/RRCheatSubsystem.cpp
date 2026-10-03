// Rogue Rigs - polls the URRCheatSettings default object every frame (paused too) and applies the cheats it requests to its world.

#include "Debug/RRCheatSubsystem.h"

#include "Debug/RRCheats.h"
#include "Debug/RRCheatSettings.h"
#include "Engine/World.h"

namespace RRCheatSubsystemPrivate
{
	/** Log label of the property route. */
	const TCHAR* const Source = TEXT("RRCheatSettings");
}

bool URRCheatSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
#if UE_BUILD_SHIPPING
	return false;
#else
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
#endif
}

TStatId URRCheatSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRCheatSubsystem, STATGROUP_Tickables);
}

void URRCheatSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	using RRCheatSubsystemPrivate::Source;
	URRCheatSettings* Settings = GetMutableDefault<URRCheatSettings>();
	UWorld* World = GetWorld();

	if (Settings->GrantXP > 0.f)
	{
		const float Amount = Settings->GrantXP;
		Settings->GrantXP = 0.f;
		RRCheats::GrantXP(World, Amount, Source);
	}

	if (Settings->bInvulnerable != bAppliedInvulnerable)
	{
		const int32 Count = RRCheats::SetInvulnerable(World, Settings->bInvulnerable, Source);
		// No miner yet: keep trying until one is tagged.
		if (!Settings->bInvulnerable || Count > 0)
		{
			bAppliedInvulnerable = Settings->bInvulnerable;
		}
	}

	if (Settings->ChooseUpgrade >= 0)
	{
		const int32 Index = Settings->ChooseUpgrade;
		Settings->ChooseUpgrade = -1;
		RRCheats::ChooseUpgrade(World, Index, Source);
	}

	if (Settings->bReroll)
	{
		Settings->bReroll = false;
		RRCheats::Reroll(World, Source);
	}

	if (Settings->bSkipToConsole)
	{
		Settings->bSkipToConsole = false;
		RRCheats::SkipToConsole(World, Source);
	}

	if (Settings->bCompleteConsoleHack)
	{
		Settings->bCompleteConsoleHack = false;
		RRCheats::CompleteConsoleHack(World, Source);
	}

	if (Settings->bSkipToExtraction)
	{
		Settings->bSkipToExtraction = false;
		RRCheats::SkipToExtraction(World, Source);
	}
}
