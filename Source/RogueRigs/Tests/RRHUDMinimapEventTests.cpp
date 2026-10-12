// Rogue Rigs - automation tests of the event-driven HUD root (RogueRigs.HUD.MinimapEvents, LocatorEvents, VignetteWarmUpOnce, task-103-01): minimap texture and view events, locator view events, one-shot vignette warm-up, no per-frame work in NativeTick.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestHUDWidget.h"
#include "UI/RRMinimap.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRHUDMinimapEventTests
{
	/** Miner cell on arena_m1_fine (open floor), as in the HUD event tests. */
	const FIntPoint HUDMinimapMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Fine grid and a miner whose weapons are stopped; null on failure. */
	ARRMiner* SpawnStoppedMiner(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, ARRArenaGrid*& OutGrid)
	{
		OutGrid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		ARRMiner* Miner = OutGrid ? RRTestHarness::SpawnMiner(Test, TestWorld, OutGrid, HUDMinimapMinerCell) : nullptr;
		if (Miner)
		{
			for (ARRWeapon* Weapon : Miner->GetWeapons())
			{
				if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
				{
					Weapon->GetAutoFireComponent()->StopFiring();
				}
			}
		}
		return Miner;
	}

	/** Ticks the world and runs the HUD's NativeTick (headless Slate never does), Frames times. */
	void TickWithHUD(const RRTestHarness::FTestWorld& TestWorld, URRHUDWidget* HUD, int32 Frames)
	{
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
			RRTestAccess::FUserWidgetAccess::NativeTickOf(HUD, RRTestHarness::FrameSeconds);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDMinimapEventsTest, "RogueRigs.HUD.MinimapEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDMinimapEventsTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDMinimapEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDMinimapEvents"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld, Grid);
	URRTestHUDWidget* HUD = Miner ? CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass()) : nullptr;
	if (!Miner || !TestNotNull(TEXT("HUD"), HUD))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(HUD);

	// Headless the widget has no player context, so NativeOnInitialized does not run: the minimap is created by the first bind
	// (in game, at initialization). Texture event once, one view event (with the miner), one seen pass.
	TestNull(TEXT("no minimap before the first bind headless"), HUD->GetMinimap());
	HUD->BindToMiner(Miner);
	const URRMinimap* Minimap = HUD->GetMinimap();
	if (!TestNotNull(TEXT("minimap created on binding"), Minimap) || !TestNotNull(TEXT("texture"), HUD->GetMinimapTexture()))
	{
		return false;
	}
	TestEqual(TEXT("texture event once"), HUD->GetMinimapTextureReadyCount(), 1);
	TestEqual(TEXT("one view event on binding"), HUD->GetMinimapViewChangedCount(), 1);
	TestEqual(TEXT("one seen pass on binding"), Minimap->GetSeenPassCount(), 1);
	TestTrue(TEXT("rock within the reveal radius seen"), Minimap->IsCellSeen(RRTestHarness::FineRockCell));

	// Idle miner: nothing for 60 frames, NativeTick included (no per-frame work).
	TickWithHUD(TestWorld, HUD, 10);
	const int32 Views = HUD->GetMinimapViewChangedCount();
	const int32 Updates = Minimap->GetUpdateCount();
	const int32 Passes = Minimap->GetSeenPassCount();
	TickWithHUD(TestWorld, HUD, 60);
	TestEqual(TEXT("idle miner: no view event"), HUD->GetMinimapViewChangedCount(), Views);
	TestEqual(TEXT("idle miner: no minimap update"), Minimap->GetUpdateCount(), Updates);
	TestEqual(TEXT("idle miner: no seen pass"), Minimap->GetSeenPassCount(), Passes);
	TestEqual(TEXT("texture event still once"), HUD->GetMinimapTextureReadyCount(), 1);

	// A move under the threshold (0.01 cell): one update, no event, no seen pass.
	const float CellSize = Grid->GetCellSize();
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(0.0, CellSize * 0.01, 0.0));
	TestEqual(TEXT("tiny move: minimap updated once"), Minimap->GetUpdateCount(), Updates + 1);
	TestEqual(TEXT("tiny move: no view event"), HUD->GetMinimapViewChangedCount(), Views);
	TestEqual(TEXT("tiny move: no seen pass"), Minimap->GetSeenPassCount(), Passes);

	// One cell: exactly one event, one seen pass revealing the far edge of the disc.
	const FIntPoint Edge(HUDMinimapMinerCell.X, HUDMinimapMinerCell.Y + 19);
	TestFalse(TEXT("cell 19 away unseen"), Minimap->IsCellSeen(Edge));
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(0.0, CellSize, 0.0));
	TestEqual(TEXT("one cell: one view event"), HUD->GetMinimapViewChangedCount(), Views + 1);
	TestEqual(TEXT("one cell: one seen pass"), Minimap->GetSeenPassCount(), Passes + 1);
	TestTrue(TEXT("one cell: cell now within 18 seen"), Minimap->IsCellSeen(Edge));
	TickWithHUD(TestWorld, HUD, 30);
	TestEqual(TEXT("stopped again: no more view event"), HUD->GetMinimapViewChangedCount(), Views + 1);

	// A seen rock destroyed: one event on the next tick for the batch, none after.
	const int32 ViewsBeforeDestroy = HUD->GetMinimapViewChangedCount();
	TestTrue(TEXT("seen rock destroyed"), Grid->DestroyCell(RRTestHarness::FineRockCell, ERRTerrainDamageSource::Drone));
	TestFalse(TEXT("no event before the coalesced flush"), HUD->GetMinimapViewChangedCount() != ViewsBeforeDestroy);
	TickWithHUD(TestWorld, HUD, 1);
	TestEqual(TEXT("destroyed seen cell: one view event"), HUD->GetMinimapViewChangedCount(), ViewsBeforeDestroy + 1);
	TickWithHUD(TestWorld, HUD, 30);
	TestEqual(TEXT("destroyed seen cell: no more"), HUD->GetMinimapViewChangedCount(), ViewsBeforeDestroy + 1);
	TestEqual(TEXT("texture event once in the whole test"), HUD->GetMinimapTextureReadyCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDLocatorEventsTest, "RogueRigs.HUD.LocatorEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDLocatorEventsTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDMinimapEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDLocatorEvents"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld, Grid);
	URRRunFlowSubsystem* RunFlow = TestWorld.Get() ? TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	URRTestHUDWidget* HUD = Miner ? CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass()) : nullptr;
	if (!Miner || !TestNotNull(TEXT("run flow"), RunFlow) || !TestNotNull(TEXT("HUD"), HUD))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(HUD);
	RunFlow->SetConfig(FRRRunFlowConfig());
	HUD->BindToRunFlow(RunFlow);
	HUD->BindToMiner(Miner);
	TickWithHUD(TestWorld, HUD, 5);

	// No target: moving sends nothing.
	const int32 NoTarget = HUD->GetLocatorViewChangedCount();
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(50.0, 0.0, 0.0));
	TickWithHUD(TestWorld, HUD, 10);
	TestEqual(TEXT("no target: no locator event on a move"), HUD->GetLocatorViewChangedCount(), NoTarget);

	// Target appears: one event (the console drop also adds a minimap marker: one minimap event).
	const int32 MinimapBefore = HUD->GetMinimapViewChangedCount();
	RunFlow->SetRandomSeed(7);
	if (!TestNotNull(TEXT("console dropped"), RunFlow->SkipToConsole()))
	{
		return false;
	}
	TestEqual(TEXT("target change: one locator event"), HUD->GetLocatorViewChangedCount(), NoTarget + 1);
	TestEqual(TEXT("console marker: one minimap event"), HUD->GetMinimapViewChangedCount(), MinimapBefore + 1);

	// Idle: nothing, NativeTick included.
	TickWithHUD(TestWorld, HUD, 10);
	const int32 Idle = HUD->GetLocatorViewChangedCount();
	TickWithHUD(TestWorld, HUD, 60);
	TestEqual(TEXT("idle: no locator event"), HUD->GetLocatorViewChangedCount(), Idle);

	// Under the threshold (0.5 uu): nothing; 50 uu: one event, then nothing while stopped.
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(0.0, 0.5, 0.0));
	TestEqual(TEXT("0.5 uu: no locator event"), HUD->GetLocatorViewChangedCount(), Idle);
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(0.0, 50.0, 0.0));
	TestEqual(TEXT("50 uu: one locator event"), HUD->GetLocatorViewChangedCount(), Idle + 1);
	TickWithHUD(TestWorld, HUD, 10);
	const int32 Settled = HUD->GetLocatorViewChangedCount();
	TickWithHUD(TestWorld, HUD, 60);
	TestEqual(TEXT("stopped: no more locator event"), HUD->GetLocatorViewChangedCount(), Settled);

	// Target gone (hack, no shuttle yet): one event, then moves send nothing.
	RunFlow->CompleteConsoleHack();
	TestEqual(TEXT("target none: one locator event"), HUD->GetLocatorViewChangedCount(), Settled + 1);
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(0.0, -50.0, 0.0));
	TestEqual(TEXT("no target again: no locator event on a move"), HUD->GetLocatorViewChangedCount(), Settled + 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDVignetteWarmUpOnceTest, "RogueRigs.HUD.VignetteWarmUpOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDVignetteWarmUpOnceTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDMinimapEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDVignetteWarmUpOnce"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld, Grid);
	URRTestHUDWidget* HUD = Miner ? CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass()) : nullptr;
	if (!Miner || !TestNotNull(TEXT("HUD"), HUD) || !TestNotNull(TEXT("HUD widget tree"), HUD->WidgetTree.Get()))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(HUD);
	TestEqual(TEXT("no vignette yet: no warm-up"), HUD->GetVignetteWarmUpCount(), 0);

	// The vignette appears with its panel (here: in the root tree), then the miner binds.
	UCanvasPanel* Canvas = HUD->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	UImage* Vignette = HUD->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("DamageVignette"));
	HUD->WidgetTree->RootWidget = Canvas;
	Canvas->AddChild(Vignette);
	URRTestHUDPanelWidget* Panel = CreateWidget<URRTestHUDPanelWidget>(HUD, URRTestHUDPanelWidget::StaticClass(), TEXT("Panel"));
	if (TestNotNull(TEXT("panel"), Panel))
	{
		Canvas->AddChild(Panel);
	}
	Vignette->SetRenderOpacity(0.f);
	HUD->BindToMiner(Miner);
	TestEqual(TEXT("warm-up once when the vignette exists"), HUD->GetVignetteWarmUpCount(), 1);
	TestNearlyEqual(TEXT("vignette at the warm floor"), Vignette->GetRenderOpacity(), HUD->GetVignetteWarmOpacity(), 1e-6f);
	if (Panel)
	{
		TestEqual(TEXT("panel handed the HUD once"), Panel->GetHUDBindCount(), 1);
		TestEqual(TEXT("panel HUD"), Panel->GetHUD(), static_cast<URRHUDWidget*>(HUD));
	}

	// Not per frame: a reset opacity stays reset across NativeTick and rebinds.
	Vignette->SetRenderOpacity(0.f);
	TickWithHUD(TestWorld, HUD, 30);
	HUD->BindToMiner(Miner);
	TestEqual(TEXT("warm-up still once"), HUD->GetVignetteWarmUpCount(), 1);
	TestEqual(TEXT("NativeTick does not touch the vignette"), Vignette->GetRenderOpacity(), 0.f);
	if (Panel)
	{
		TestEqual(TEXT("panel still handed the HUD once"), Panel->GetHUDBindCount(), 1);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
