// Rogue Rigs - command-line entry point of the layout generator.

#include "Grid/RRLayoutGenCommandlet.h"

#include "Grid/RRLayoutGenerator.h"
#include "Grid/RRLayoutGenParams.h"
#include "HAL/PlatformTime.h"
#include "RogueRigs.h"

URRLayoutGenCommandlet::URRLayoutGenCommandlet()
{
	IsClient = false;
	IsServer = false;
	IsEditor = true;
	LogToConsole = true;
}

int32 URRLayoutGenCommandlet::Main(const FString& Params)
{
	FString ParamsPath = RRLayoutGen::GetDefaultParamsPath();
	FParse::Value(*Params, TEXT("params="), ParamsPath);
	int32 Seed = 1;
	FParse::Value(*Params, TEXT("seed="), Seed);
	const bool bCarveOnly = FParse::Param(*Params, TEXT("carveonly"));

	FRRLayoutGenParams GenParams;
	FString Error;
	if (!RRLayoutGen::LoadParams(ParamsPath, GenParams, Error))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: %s"), *Error);
		return 2;
	}

	if (bCarveOnly)
	{
		// Carving slice check: carves the seed twice and reports determinism and the passage rule.
		FRRLayoutGenerator Generator(GenParams);
		const double Start = FPlatformTime::Seconds();
		Generator.Carve(Seed);
		const double Seconds = FPlatformTime::Seconds() - Start;
		const uint32 FirstHash = Generator.HashGrid();
		const int32 Narrow = Generator.CountNarrowCells();
		int32 Counts[8] = {};
		for (const ERRCellType Type : Generator.GetCells())
		{
			++Counts[static_cast<int32>(Type)];
		}
		FRRLayoutGenerator Second(GenParams);
		Second.Carve(Seed);
		const bool bSame = Second.GetCells() == Generator.GetCells();
		UE_LOG(LogRogueRigs, Display, TEXT("RRLayoutGen carve seed %d: %.3f s, hash %08x, same on rerun %s, narrow cells %d, void %d rock %d floor %d"),
			Seed, Seconds, FirstHash, bSame ? TEXT("yes") : TEXT("no"), Narrow,
			Counts[static_cast<int32>(ERRCellType::Void)], Counts[static_cast<int32>(ERRCellType::Rock)],
			Counts[static_cast<int32>(ERRCellType::Floor)] + Counts[static_cast<int32>(ERRCellType::Spawn)]);
		return bSame && Narrow == 0 ? 0 : 1;
	}

	UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: only -carveonly is available in this build"));
	return 2;
}
