// Rogue Rigs - asynchronous layout loader tests (RogueRigs.LayoutLoader.*, task-493-01): the loader builds arena_m1_fine exactly as
// the synchronous grid path does, builds a layout of the C++ generator (task-492) with the file's cell counts, raises its
// percentage monotonically from 0 to 100 over several frames, fires its completion event at 100, and fails cleanly on a missing file.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Grid/RRArenaGrid.h"
#include "Grid/RRLayoutGenParams.h"
#include "Grid/RRLayoutGenerator.h"
#include "Grid/RRLayoutLoaderSubsystem.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

namespace RRLayoutLoaderTests
{
	using namespace RRTestHarness;
	using RRTestAccess::FGridAccess;

	/** What one load did, frame by frame. */
	struct FLoadRun
	{
		/** Percentage after StartLoad, then after each loader tick. */
		TArray<float> Percents;
		/** Loader ticks spent in the build phase. */
		int32 BuildTicks = 0;
		/** True when the load ended before the frame limit. */
		bool bEnded = false;
	};

	/**
	 * Starts a load of Path and ticks the loader one fixed frame at a time until it ends, pumping async loading in between (the
	 * engine loop is not running inside a test). Checks at each tick that the completion event comes exactly when the
	 * percentage reaches 100.
	 */
	FLoadRun RunLoad(FAutomationTestBase& Test, URRLayoutLoaderSubsystem* Loader, URRTestEventRecorder* Recorder, const FString& Path)
	{
		FLoadRun Run;
		if (!Test.TestTrue(TEXT("load starts"), Loader->StartLoad(Path, FTransform::Identity)))
		{
			return Run;
		}
		Run.Percents.Add(Loader->GetLoadPercent());
		for (int32 Frame = 0; Frame < 20000 && Loader->IsLoading(); ++Frame)
		{
			if (Loader->GetPhase() == ERRLayoutLoadPhase::Loading)
			{
				ProcessAsyncLoading(/*bUseTimeLimit*/ true, /*bUseFullTimeLimit*/ false, 0.005);
				FPlatformProcess::Sleep(0.001f);
			}
			const bool bBuilding = Loader->GetPhase() == ERRLayoutLoadPhase::Building;
			const int32 EventsBefore = Recorder->LayoutLoadResults.Num();
			Loader->TickLoad(FrameSeconds);
			Run.BuildTicks += bBuilding ? 1 : 0;
			Run.Percents.Add(Loader->GetLoadPercent());
			if (Recorder->LayoutLoadResults.Num() == EventsBefore && Loader->IsLoading())
			{
				if (Loader->GetLoadPercent() >= 100.f)
				{
					Test.AddError(TEXT("percentage reached 100 before the completion event"));
				}
			}
		}
		Run.bEnded = !Loader->IsLoading();
		return Run;
	}

	/** Checks the percentage of a successful load: starts below 100, never decreases, ends at exactly 100. */
	void CheckPercents(FAutomationTestBase& Test, const FLoadRun& Run)
	{
		if (!Test.TestTrue(TEXT("percentages recorded"), Run.Percents.Num() >= 2))
		{
			return;
		}
		Test.TestTrue(TEXT("percentage starts below 100"), Run.Percents[0] < 100.f);
		bool bMonotonic = true;
		for (int32 Index = 1; Index < Run.Percents.Num(); ++Index)
		{
			bMonotonic &= Run.Percents[Index] >= Run.Percents[Index - 1];
		}
		Test.TestTrue(TEXT("percentage never decreases"), bMonotonic);
		Test.TestEqual(TEXT("percentage ends at 100"), Run.Percents.Last(), 100.f);
		for (int32 Index = 0; Index + 1 < Run.Percents.Num(); ++Index)
		{
			if (Run.Percents[Index] >= 100.f)
			{
				Test.AddError(FString::Printf(TEXT("percentage at 100 at tick %d of %d, before the end"), Index, Run.Percents.Num()));
				break;
			}
		}
	}

	/** Checks the completion event: one broadcast, success, with the loader's grid. */
	void CheckCompletion(FAutomationTestBase& Test, const URRLayoutLoaderSubsystem* Loader, const URRTestEventRecorder* Recorder)
	{
		Test.TestEqual(TEXT("one completion event"), Recorder->LayoutLoadResults.Num(), 1);
		if (Recorder->LayoutLoadResults.Num() == 1)
		{
			Test.TestTrue(TEXT("completion reports success"), Recorder->LayoutLoadResults[0]);
			Test.TestTrue(TEXT("completion carries the loaded grid"), Recorder->LayoutLoadGrids[0].Get() == Loader->GetLoadedGrid());
		}
		Test.TestEqual(TEXT("phase done"), Loader->GetPhase(), ERRLayoutLoadPhase::Done);
	}

	/** Creates a recorder bound to the loader's completion event. */
	TStrongObjectPtr<URRTestEventRecorder> BindRecorder(URRLayoutLoaderSubsystem* Loader)
	{
		TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
		Loader->OnLoadCompleted.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleLayoutLoadCompleted);
		return Recorder;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLayoutLoaderFineLayoutTest, "RogueRigs.LayoutLoader.FineLayoutMatchesSyncBuild",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLayoutLoaderFineLayoutTest::RunTest(const FString& Parameters)
{
	using namespace RRLayoutLoaderTests;
	FTestWorld TestWorld(TEXT("RRLayoutLoaderFineWorld"));
	URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(TestWorld.Get());
	if (!TestNotNull(TEXT("layout loader subsystem"), Loader))
	{
		return false;
	}
	// Reference: the grid's own synchronous path on the same layout (sys-grid), with the game cell size.
	ARRArenaGrid* Reference = SpawnFineGrid(*this, TestWorld);
	if (!Reference)
	{
		return false;
	}

	TStrongObjectPtr<URRTestEventRecorder> Recorder = BindRecorder(Loader);
	const FLoadRun Run = RunLoad(*this, Loader, Recorder.Get(), FineLayoutName);
	TestTrue(TEXT("load ended"), Run.bEnded);
	CheckPercents(*this, Run);
	CheckCompletion(*this, Loader, Recorder.Get());
	TestTrue(TEXT("build spread over several frames"), Run.BuildTicks > 1);

	ARRArenaGrid* Loaded = Loader->GetLoadedGrid();
	if (!TestNotNull(TEXT("loaded grid"), Loaded))
	{
		return false;
	}
	const FRRLayoutLoadStats& Stats = Loader->GetLastLoadStats();
	TestTrue(TEXT("cell counts read back equal the file"), Stats.bCountsMatch);
	TestEqual(TEXT("grid size"), Loaded->GetGridSize(), Reference->GetGridSize());
	TestEqual(TEXT("same cell size"), Loaded->GetCellSize(), Reference->GetCellSize());

	// Same grid as the synchronous build: cell types, instance counts and every instance index map.
	bool bSameCells = true;
	for (int32 Y = 0; Y < Reference->GetGridSize().Y; ++Y)
	{
		for (int32 X = 0; X < Reference->GetGridSize().X; ++X)
		{
			bSameCells &= Loaded->GetCellType(FIntPoint(X, Y)) == Reference->GetCellType(FIntPoint(X, Y));
		}
	}
	TestTrue(TEXT("same cell types as the synchronous build (deposits included)"), bSameCells);
	for (const ERRCellType Type : { ERRCellType::Floor, ERRCellType::Rock, ERRCellType::Ore, ERRCellType::HealOre, ERRCellType::SpecialOre, ERRCellType::Barrel })
	{
		TestEqual(*FString::Printf(TEXT("%s instances"), *UEnum::GetValueAsString(Type)),
			FGridAccess::InstanceCount(Loaded, Type), FGridAccess::InstanceCount(Reference, Type));
	}
	TestTrue(TEXT("some rock instances"), FGridAccess::InstanceCount(Loaded, ERRCellType::Rock) > 0);
	TestEqual(TEXT("rock piece instances"), FGridAccess::PieceInstanceCount(Loaded), FGridAccess::PieceInstanceCount(Reference));
	TestTrue(TEXT("same block instance indices"), FGridAccess::InstanceIndices(Loaded) == FGridAccess::InstanceIndices(Reference));
	TestTrue(TEXT("same floor instance indices"), FGridAccess::FloorInstanceIndices(Loaded) == FGridAccess::FloorInstanceIndices(Reference));
	TestTrue(TEXT("same corner piece visuals"), FGridAccess::CornerVisuals(Loaded) == FGridAccess::CornerVisuals(Reference));
	TestEqual(TEXT("same spawn cell"), Loaded->GetSpawnCell(), Reference->GetSpawnCell());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLayoutLoaderGeneratedLayoutTest, "RogueRigs.LayoutLoader.GeneratedLayout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLayoutLoaderGeneratedLayoutTest::RunTest(const FString& Parameters)
{
	using namespace RRLayoutLoaderTests;
	// A layout of the C++ generator (task-492), written to Saved/Automation like the RRLayoutGen commandlet writes it.
	FRRLayoutGenParams Params;
	FString Error;
	if (!TestTrue(*FString::Printf(TEXT("generator params load (%s)"), *Error), RRLayoutGen::LoadParams(RRLayoutGen::GetDefaultParamsPath(), Params, Error)))
	{
		return false;
	}
	FRRLayoutGenerator Generator(Params);
	const FRRGeneratedLayout Generated = Generator.Generate(1000, nullptr);
	if (!TestEqual(TEXT("generator found a passing seed"), Generated.Fails.Num(), 0))
	{
		return false;
	}
	const FString Path = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("RRLayoutLoader"), TEXT("loader_gen.json")));
	if (!TestTrue(TEXT("layout written"), FFileHelper::SaveStringToFile(FRRLayoutGenerator::ToJson(TEXT("loader_gen"), Generated), *Path,
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)))
	{
		return false;
	}

	FTestWorld TestWorld(TEXT("RRLayoutLoaderGeneratedWorld"));
	URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(TestWorld.Get());
	if (!TestNotNull(TEXT("layout loader subsystem"), Loader))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder = BindRecorder(Loader);
	const FLoadRun Run = RunLoad(*this, Loader, Recorder.Get(), Path);
	TestTrue(TEXT("load ended"), Run.bEnded);
	CheckPercents(*this, Run);
	CheckCompletion(*this, Loader, Recorder.Get());
	TestTrue(TEXT("build spread over several frames"), Run.BuildTicks > 1);

	const ARRArenaGrid* Loaded = Loader->GetLoadedGrid();
	if (!TestNotNull(TEXT("loaded grid"), Loaded))
	{
		return false;
	}
	const FRRLayoutLoadStats& Stats = Loader->GetLastLoadStats();
	TestTrue(TEXT("cell counts read back equal the file"), Stats.bCountsMatch);
	TestEqual(TEXT("grid size"), Loaded->GetGridSize(), FIntPoint(Generated.Width, Generated.Height));
	// The file counts are the generator's cells.
	TArray<int32> Expected;
	Expected.Init(0, Stats.FileCounts.Num());
	for (const ERRCellType Type : Generated.Cells)
	{
		if (Expected.IsValidIndex(static_cast<int32>(Type)))
		{
			++Expected[static_cast<int32>(Type)];
		}
	}
	TestTrue(TEXT("file counts equal the generated cells"), Expected == Stats.FileCounts);
	TestTrue(TEXT("the layout has barrels and heal ore"), Stats.GridCounts[static_cast<int32>(ERRCellType::Barrel)] > 0
		&& Stats.GridCounts[static_cast<int32>(ERRCellType::HealOre)] > 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLayoutLoaderMissingFileTest, "RogueRigs.LayoutLoader.MissingFileFails",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLayoutLoaderMissingFileTest::RunTest(const FString& Parameters)
{
	using namespace RRLayoutLoaderTests;
	AddExpectedError(TEXT("RRLayoutLoader: load of"), EAutomationExpectedErrorFlags::Contains, 1);
	FTestWorld TestWorld(TEXT("RRLayoutLoaderMissingWorld"));
	URRLayoutLoaderSubsystem* Loader = URRLayoutLoaderSubsystem::Get(TestWorld.Get());
	if (!TestNotNull(TEXT("layout loader subsystem"), Loader))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder = BindRecorder(Loader);
	const FLoadRun Run = RunLoad(*this, Loader, Recorder.Get(), TEXT("Saved/Automation/RRLayoutLoader/no_such_layout.json"));
	TestTrue(TEXT("load ended"), Run.bEnded);
	TestEqual(TEXT("phase failed"), Loader->GetPhase(), ERRLayoutLoadPhase::Failed);
	TestEqual(TEXT("one completion event"), Recorder->LayoutLoadResults.Num(), 1);
	if (Recorder->LayoutLoadResults.Num() == 1)
	{
		TestFalse(TEXT("completion reports failure"), Recorder->LayoutLoadResults[0]);
	}
	TestNull(TEXT("no grid left"), Loader->GetLoadedGrid());
	TestTrue(TEXT("percentage below 100"), Loader->GetLoadPercent() < 100.f);
	return true;
}

#endif
