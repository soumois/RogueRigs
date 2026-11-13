// Rogue Rigs - command-line entry point of the layout generator.

#include "Grid/RRLayoutGenCommandlet.h"

#include "Grid/RRLayoutGenerator.h"
#include "Grid/RRLayoutGenParams.h"
#include "HAL/PlatformTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
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
		UE_LOG(LogRogueRigs, Display, TEXT("RRLayoutGen carve seed %d filler: %.3f s (%.0f %% of carve), %d iterations, %d carved nothing"),
			Seed, Generator.GetFillerSeconds(), Seconds > 0.0 ? 100.0 * Generator.GetFillerSeconds() / Seconds : 0.0,
			Generator.GetFillerIterations(), Generator.GetFillerIdleIterations());
		int32 Placed = 0;
		int32 Removed = 0;
		int32 Partly = 0;
		Generator.CountPockets(Placed, Removed, Partly);
		int32 Stale = 0;
		int32 Missing = 0;
		Generator.CountGuardMismatch(Stale, Missing);
		UE_LOG(LogRogueRigs, Display, TEXT("RRLayoutGen carve seed %d pockets: %d placed, %d removed, %d partly removed; guard cells released %d, stale %d, missing %d; idle walks that lost rock to a guard %d"),
			Seed, Placed, Removed, Partly, Generator.GetGuardReleased(), Stale, Missing, Generator.GetFillerIdleGuardBlocked());
		return bSame && Narrow == 0 ? 0 : 1;
	}

	FString Name = TEXT("arena_gen");
	FParse::Value(*Params, TEXT("name="), Name);
	FString OutDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("GeneratedLevels"));
	FParse::Value(*Params, TEXT("out="), OutDir);
	OutDir = FPaths::ConvertRelativePathToFull(OutDir);
	FString BriefPath;
	FParse::Value(*Params, TEXT("brief="), BriefPath);

	bool bValidName = !Name.IsEmpty();
	for (const TCHAR Char : Name)
	{
		bValidName &= (Char >= TEXT('a') && Char <= TEXT('z')) || (Char >= TEXT('0') && Char <= TEXT('9')) || Char == TEXT('_');
	}
	if (!bValidName)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: -name must be snake_case ([a-z0-9_]+), got '%s'"), *Name);
		return 2;
	}

	FRRLayoutBrief Brief;
	const bool bHasBrief = !BriefPath.IsEmpty();
	if (bHasBrief && !RRLayoutGen::LoadBrief(BriefPath, Brief, Error))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: %s"), *Error);
		return 2;
	}

	FRRLayoutGenerator Generator(GenParams);
	const FRRGeneratedLayout Layout = Generator.Generate(Seed, bHasBrief ? &Brief : nullptr);
	if (Layout.Fails.Num() > 0)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: no seed in [%d, %d] passes (%.2f s); nothing written"),
			Seed, Seed + Layout.SeedsTried - 1, Layout.SearchSeconds);
		return 1;
	}

	const FString LayoutPath = FPaths::Combine(OutDir, Name + TEXT(".json"));
	if (!FFileHelper::SaveStringToFile(FRRLayoutGenerator::ToJson(Name, Layout), *LayoutPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: cannot write %s"), *LayoutPath);
		return 1;
	}
	if (bHasBrief)
	{
		const FString BriefOut = FPaths::Combine(OutDir, Name + TEXT(".brief.json"));
		if (!FFileHelper::SaveStringToFile(Brief.SourceText, *BriefOut, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			UE_LOG(LogRogueRigs, Error, TEXT("RRLayoutGen: cannot write %s"), *BriefOut);
			return 1;
		}
	}
	const FRRLayoutGenStats& S = Layout.Stats;
	UE_LOG(LogRogueRigs, Display, TEXT("RRLayoutGen: seed %d accepted after %d seed(s); generation %.2f s for this layout, %.2f s for the search; review: window %.3f-%.3f, longest lane %d, farthest pocket %d, repairs carve %d outcrop %d lane %d; cells hash %08x; wrote %s"),
		Layout.Seed, Layout.SeedsTried, Layout.Seconds, Layout.SearchSeconds, S.WindowMin, S.WindowMax, S.LongestLane, S.FarthestPocket,
		S.RepairCarves, S.RepairOutcrops, S.RepairLanes, FRRLayoutGenerator::HashCells(Layout.Cells), *LayoutPath);
	return 0;
}
