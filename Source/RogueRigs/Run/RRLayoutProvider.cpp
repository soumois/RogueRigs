// Rogue Rigs - layout provider slot, its JSON file implementation (task-206-02) and the C++ generator provider of the run (task-496).

#include "Run/RRLayoutProvider.h"

#include "Grid/RRLayoutData.h"
#include "Grid/RRLayoutGenerator.h"
#include "Grid/RRLayoutGenParams.h"
#include "HAL/PlatformTime.h"
#include "Misc/Paths.h"
#include "RogueRigs.h"
#include "Run/RRRunSettings.h"

FRRJsonLayoutProvider::FRRJsonLayoutProvider()
	: LevelsDirectory(FPaths::GetPath(RRLayout::GetLayoutFilePath(TEXT("layout"))))
{
}

TSharedRef<FRRLayoutData> FRRJsonLayoutProvider::BuildLayout(const FRRLayoutRequest& Request) const
{
	TSharedRef<FRRLayoutData> Layout = MakeShared<FRRLayoutData>();
	if (Request.LayoutName.IsEmpty())
	{
		Layout->Error = FString::Printf(TEXT("no layout name for level %d"), Request.LevelIndex);
		return Layout;
	}
	RRLayout::LoadLayoutFile(Request.LayoutName, FPaths::Combine(LevelsDirectory, Request.LayoutName + TEXT(".json")), *Layout);
	Layout->Source = TEXT("hand json");
	return Layout;
}

FRRGeneratorLayoutProvider::FRRGeneratorLayoutProvider()
	: LevelsDirectory(FPaths::GetPath(RRLayout::GetLayoutFilePath(TEXT("layout"))))
{
	const URRRunSettings* Settings = GetDefault<URRRunSettings>();
	ParamsFiles = Settings->LevelGeneratorParams;
	BriefFiles = Settings->LevelGeneratorBriefs;
}

bool FRRGeneratorLayoutProvider::Generate(const FRRLayoutRequest& Request, FRRLayoutData& OutLayout, FString& OutError) const
{
	if (Request.bForceGeneratorFailure)
	{
		OutError = TEXT("generation skipped by rr.Layout.ForceGenFail (debug)");
		return false;
	}
	const int32 Slot = Request.LevelIndex - 1;
	const FString ParamsFile = ParamsFiles.IsValidIndex(Slot) ? ParamsFiles[Slot] : FString();
	if (ParamsFile.IsEmpty())
	{
		OutError = FString::Printf(TEXT("no generator params file for level %d (URRRunSettings::LevelGeneratorParams)"), Request.LevelIndex);
		return false;
	}
	FRRLayoutGenParams Params;
	if (!RRLayoutGen::LoadParams(FPaths::Combine(LevelsDirectory, ParamsFile), Params, OutError))
	{
		return false;
	}
	const FString BriefFile = BriefFiles.IsValidIndex(Slot) ? BriefFiles[Slot] : FString();
	FRRLayoutBrief Brief;
	if (!BriefFile.IsEmpty() && !RRLayoutGen::LoadBrief(FPaths::Combine(LevelsDirectory, BriefFile), Brief, OutError))
	{
		return false;
	}

	const double Start = FPlatformTime::Seconds();
	FRRLayoutGenerator Generator(Params);
	const FRRGeneratedLayout Generated = Generator.Generate(Request.LevelSeed, BriefFile.IsEmpty() ? nullptr : &Brief);
	const double Seconds = FPlatformTime::Seconds() - Start;
	if (Generated.Fails.Num() > 0 || Generated.Cells.Num() != Generated.Width * Generated.Height || Generated.Width <= 0)
	{
		OutError = FString::Printf(TEXT("no seed in [%d, %d] passes (%.2f s, last fails: %s)"), Request.LevelSeed,
			Request.LevelSeed + FMath::Max(0, Generated.SeedsTried - 1), Seconds, *FString::Join(Generated.Fails, TEXT(", ")));
		return false;
	}

	OutLayout = FRRLayoutData();
	OutLayout.Name = Request.LayoutName;
	OutLayout.Width = Generated.Width;
	OutLayout.Height = Generated.Height;
	OutLayout.Cells = Generated.Cells;
	const int32 SpawnIndex = OutLayout.Cells.IndexOfByKey(ERRCellType::Spawn);
	OutLayout.SpawnCell = SpawnIndex != INDEX_NONE ? FIntPoint(SpawnIndex % OutLayout.Width, SpawnIndex / OutLayout.Width) : FIntPoint(-1, -1);
	OutLayout.Source = FString::Printf(TEXT("generator, run seed %d, level seed %d, accepted seed %d"), Request.RunSeed, Request.LevelSeed, Generated.Seed);

	const FRRLayoutGenStats& S = Generated.Stats;
	UE_LOG(LogRogueRigs, Log, TEXT("RRGeneratorLayoutProvider: level %d (%s) generated: run seed %d, level seed %d, accepted seed %d after %d seed(s); params %s, brief %s; generation %.2f s (accepted seed %.2f s); %d x %d, spawn (%d, %d), rock %d, heal_ore %d, barrel %d, void %d, reachable %.3f, coverage %.3f; cells hash %08x."),
		Request.LevelIndex, *Request.LayoutName, Request.RunSeed, Request.LevelSeed, Generated.Seed, Generated.SeedsTried, *ParamsFile,
		BriefFile.IsEmpty() ? TEXT("none") : *BriefFile, Seconds, Generated.Seconds, OutLayout.Width, OutLayout.Height, OutLayout.SpawnCell.X, OutLayout.SpawnCell.Y,
		S.Counts.FindRef(TEXT("rock")), S.Counts.FindRef(TEXT("heal_ore")), S.Counts.FindRef(TEXT("barrel")), S.Counts.FindRef(TEXT("void")),
		S.Reach, S.Coverage, FRRLayoutGenerator::HashCells(OutLayout.Cells));
	if (OutLayout.SpawnCell.X < 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("RRGeneratorLayoutProvider: level %d generated layout has no spawn cell."), Request.LevelIndex);
	}
	return true;
}

TSharedRef<FRRLayoutData> FRRGeneratorLayoutProvider::BuildLayout(const FRRLayoutRequest& Request) const
{
	TSharedRef<FRRLayoutData> Layout = MakeShared<FRRLayoutData>();
	FString Error;
	if (Generate(Request, *Layout, Error))
	{
		return Layout;
	}
	// Owner decision (task-496 Q5): a level whose generation fails takes its hand layout; the run continues.
	UE_LOG(LogRogueRigs, Error, TEXT("RRGeneratorLayoutProvider: level %d generation failed (run seed %d, level seed %d): %s; falling back to the hand layout %s.json."),
		Request.LevelIndex, Request.RunSeed, Request.LevelSeed, *Error, *Request.LayoutName);
	TSharedRef<FRRLayoutData> Hand = Fallback.BuildLayout(Request);
	Hand->Source = FString::Printf(TEXT("hand json (generator fallback, level seed %d)"), Request.LevelSeed);
	return Hand;
}
