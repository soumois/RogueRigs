// Rogue Rigs - automation tests of the HUD objective locator (RogueRigs.HUD.ObjectiveLocator): target console until hacked, then the shuttle; viewport projection; edge point and arrow angle.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Camera/CameraComponent.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestHUDWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDObjectiveLocatorTest, "RogueRigs.HUD.ObjectiveLocator",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDObjectiveLocatorTest::RunTest(const FString& Parameters)
{
	// Edge math on a 1920 x 1080 viewport, margin 50.
	const FVector2D Viewport(1920.0, 1080.0);
	FVector2D Edge;
	float Angle = 0.f;
	TestTrue(TEXT("center is on screen"), URRHUDWidget::ComputeEdgeLocator(FVector2D(960.0, 540.0), Viewport, 50.f, Edge, Angle));
	TestTrue(TEXT("a corner pixel is on screen"), URRHUDWidget::ComputeEdgeLocator(FVector2D(1900.0, 20.0), Viewport, 50.f, Edge, Angle));
	TestTrue(TEXT("on screen: arrow at the target"), Edge.Equals(FVector2D(1900.0, 20.0), 1e-3));
	TestFalse(TEXT("far right is off screen"), URRHUDWidget::ComputeEdgeLocator(FVector2D(3000.0, 540.0), Viewport, 50.f, Edge, Angle));
	TestTrue(TEXT("right: edge (1870, 540)"), Edge.Equals(FVector2D(1870.0, 540.0), 1e-3));
	TestNearlyEqual(TEXT("right: angle 0"), Angle, 0.f, 1e-3f);
	URRHUDWidget::ComputeEdgeLocator(FVector2D(960.0, -500.0), Viewport, 50.f, Edge, Angle);
	TestTrue(TEXT("up: edge (960, 50)"), Edge.Equals(FVector2D(960.0, 50.0), 1e-3));
	TestNearlyEqual(TEXT("up: angle -90"), Angle, -90.f, 1e-3f);
	URRHUDWidget::ComputeEdgeLocator(FVector2D(-1000.0, -1000.0), Viewport, 50.f, Edge, Angle);
	AddInfo(FString::Printf(TEXT("up-left target: edge (%.1f, %.1f), angle %.2f"), Edge.X, Edge.Y, Angle));
	TestTrue(TEXT("up-left: edge on the top inset line (y 50, x 336.4)"), Edge.Equals(FVector2D(960.0 - 1960.0 * 490.0 / 1540.0, 50.0), 0.1));
	TestNearlyEqual(TEXT("up-left: angle -141.8"), Angle, static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(-1540.0, -1960.0))), 1e-2f);

	// World: fine grid, miner (its camera gives the view), run flow, test HUD bound to it.
	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDObjectiveLocator"));
	const ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(*this, TestWorld, Grid, RRTestHarness::FineCell(FIntPoint(40, 40))) : nullptr;
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	if (!Miner || !TestNotNull(TEXT("run flow"), RunFlow))
	{
		return false;
	}
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}
	RunFlow->SetRandomSeed(20261006);
	TStrongObjectPtr<URRTestHUDWidget> HUD(CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass()));
	if (!TestNotNull(TEXT("test HUD"), HUD.Get()))
	{
		return false;
	}
	HUD->BindToRunFlow(RunFlow);
	ERRLocatorTarget Target = ERRLocatorTarget::Console;
	FVector Location;
	TestFalse(TEXT("no target before the console"), HUD->GetObjectiveTarget(Target, Location));
	TestEqual(TEXT("target None"), Target, ERRLocatorTarget::None);

	ARRConsole* Console = RunFlow->SkipToConsole();
	if (!TestNotNull(TEXT("console dropped"), Console))
	{
		return false;
	}
	TestTrue(TEXT("console is the target"), HUD->GetObjectiveTarget(Target, Location) && Target == ERRLocatorTarget::Console);
	TestTrue(TEXT("at the console location"), Location.Equals(Console->GetActorLocation()));
	FRRObjectiveLocator Locator;
	TestFalse(TEXT("no owning player: no projection"), HUD->GetObjectiveLocator(Locator));
	TestEqual(TEXT("locator target still filled"), Locator.Target, ERRLocatorTarget::Console);

	// Projection with the miner camera: the miner is at the screen center area, the console (36-60 cells away) off screen.
	const UCameraComponent* Camera = Miner->FindComponentByClass<UCameraComponent>();
	if (TestNotNull(TEXT("miner camera"), Camera))
	{
		FMinimalViewInfo View;
		const_cast<UCameraComponent*>(Camera)->GetCameraView(0.f, View);
		FVector2D MinerScreen;
		TestTrue(TEXT("miner in front of the camera"), URRHUDWidget::ProjectToViewport(View, Viewport, Miner->GetActorLocation(), MinerScreen));
		TestTrue(TEXT("miner on screen"), URRHUDWidget::ComputeEdgeLocator(MinerScreen, Viewport, 50.f, Edge, Angle));
		FVector2D ConsoleScreen;
		URRHUDWidget::ProjectToViewport(View, Viewport, Console->GetActorLocation(), ConsoleScreen);
		const bool bConsoleOnScreen = URRHUDWidget::ComputeEdgeLocator(ConsoleScreen, Viewport, 50.f, Edge, Angle);
		AddInfo(FString::Printf(TEXT("miner at (%.0f, %.0f); console %.0f uu away at (%.0f, %.0f): edge (%.0f, %.0f), angle %.1f"),
			MinerScreen.X, MinerScreen.Y, FVector::Dist2D(Miner->GetActorLocation(), Console->GetActorLocation()), ConsoleScreen.X, ConsoleScreen.Y, Edge.X, Edge.Y, Angle));
		TestFalse(TEXT("console off screen"), bConsoleOnScreen);
		const bool bOnInset = FMath::IsNearlyEqual(Edge.X, 50.0, 0.5) || FMath::IsNearlyEqual(Edge.X, 1870.0, 0.5)
			|| FMath::IsNearlyEqual(Edge.Y, 50.0, 0.5) || FMath::IsNearlyEqual(Edge.Y, 1030.0, 0.5);
		TestTrue(TEXT("edge point on the inset border"), bOnInset);
		// A point straight right of the camera in camera space lands right of the center.
		FVector2D Right;
		URRHUDWidget::ProjectToViewport(View, Viewport, View.Location + View.Rotation.RotateVector(FVector(1000.0, 300.0, 0.0)), Right);
		TestTrue(TEXT("camera-right projects right of center, on the middle row"), Right.X > 960.0 && FMath::IsNearlyEqual(Right.Y, 540.0, 0.5));
		FVector2D Behind;
		TestFalse(TEXT("a point behind the camera does not project"), URRHUDWidget::ProjectToViewport(View, Viewport, View.Location - View.Rotation.Vector() * 500.0 + View.Rotation.RotateVector(FVector(0.0, 200.0, 0.0)), Behind));
		TestTrue(TEXT("behind and right: pushed off screen to the right"), Behind.X > 1920.0);
	}

	// Hacked: no target until the shuttle lands, then the shuttle.
	TestTrue(TEXT("hack completed"), RunFlow->CompleteConsoleHack());
	TestFalse(TEXT("console hacked: not a target"), HUD->GetObjectiveTarget(Target, Location) && Target == ERRLocatorTarget::Console);
	TestTrue(TEXT("skip to extraction"), RunFlow->SkipToExtraction());
	ARRShuttle* Shuttle = RunFlow->GetShuttle();
	if (TestNotNull(TEXT("shuttle landed"), Shuttle))
	{
		TestTrue(TEXT("shuttle is the target"), HUD->GetObjectiveTarget(Target, Location) && Target == ERRLocatorTarget::Shuttle);
		TestTrue(TEXT("at the shuttle location"), Location.Equals(Shuttle->GetActorLocation()));
	}
	HUD->BindToRunFlow(nullptr);
	TestFalse(TEXT("unbound: no target"), HUD->GetObjectiveTarget(Target, Location));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
