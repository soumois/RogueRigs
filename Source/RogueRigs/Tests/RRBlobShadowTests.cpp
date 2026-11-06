// Rogue Rigs - automation tests of the actor shadows (RogueRigs.BlobShadow.*, task-472-01): per-type and global opacity and
// scale, placement under the miner and a robot and follow on a move, one shadow per barrel removed with it, no light shadow.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Enemies/RRChaser.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestHarness.h"
#include "UObject/Package.h"
#include "World/RRBlobShadowComponent.h"

namespace RRBlobShadowTests
{
	using namespace RRTestHarness;

	/** Open floor of arena_m1 (rows 41-43 are floor from x 32 to 45). */
	const FIntPoint OpenCell(36, 42);

	/** Single barrel of arena_m1 (RRBarrelTests ShotBarrelCell). */
	const FIntPoint BarrelCell(43, 6);

	/** Custom primitive data float read by the shadow material (opacity). */
	float OpacityOf(const URRBlobShadowComponent* Shadow)
	{
		const TArray<float>& Data = Shadow->GetCustomPrimitiveData().Data;
		return Data.IsValidIndex(URRBlobShadowComponent::OpacityDataIndex) ? Data[URRBlobShadowComponent::OpacityDataIndex] : -1.f;
	}

	/** True when no primitive of Actor other than its shadow casts a light shadow. */
	bool CastsNoLightShadow(const AActor* Actor)
	{
		TInlineComponentArray<UPrimitiveComponent*> Primitives(Actor);
		for (const UPrimitiveComponent* Primitive : Primitives)
		{
			if (Primitive->CastShadow && Primitive->IsVisible())
			{
				return false;
			}
		}
		return true;
	}

	/** Checks Shadow lies on the floor right under a capsule of Capsule (XY on the capsule, Z = its bottom + floor offset). */
	void TestUnderCapsule(FAutomationTestBase& Test, const TCHAR* What, const URRBlobShadowComponent* Shadow, const UCapsuleComponent* Capsule)
	{
		const FVector Location = Shadow->GetComponentLocation();
		const FVector CapsuleLocation = Capsule->GetComponentLocation();
		Test.TestTrue(*FString::Printf(TEXT("%s shadow XY on the actor (%s vs %s)"), What, *Location.ToString(), *CapsuleLocation.ToString()),
			FVector::Dist2D(Location, CapsuleLocation) < 0.5);
		const double FloorZ = CapsuleLocation.Z - Capsule->GetScaledCapsuleHalfHeight();
		Test.TestNearlyEqual(*FString::Printf(TEXT("%s shadow Z just above the capsule bottom"), What), Location.Z, FloorZ + Shadow->GetFloorOffset(), 0.5);
	}
}

using namespace RRBlobShadowTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBlobShadowSettingsTest, "RogueRigs.BlobShadow.OpacityAndScaleSettings",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBlobShadowSettingsTest::RunTest(const FString& Parameters)
{
	URRBlobShadowSettings* Settings = GetMutableDefault<URRBlobShadowSettings>();
	const float SavedOpacity = Settings->GlobalOpacityMultiplier;
	const float SavedScale = Settings->GlobalScaleMultiplier;
	Settings->GlobalOpacityMultiplier = 1.f;
	Settings->GlobalScaleMultiplier = 1.f;

	URRBlobShadowComponent* Shadow = NewObject<URRBlobShadowComponent>(GetTransientPackage());
	TestNotNull(TEXT("shadow created"), Shadow);
	if (Shadow)
	{
		// Start values (owner 2026-11-06 Q3): opacity 0.6, diameter 1.2 x the footprint.
		TestNearlyEqual(TEXT("default opacity 0.6"), Shadow->GetShadowOpacity(), 0.6f, 1e-4f);
		TestNearlyEqual(TEXT("default scale 1.2"), Shadow->GetShadowScale(), 1.2f, 1e-4f);
		Shadow->SetFootprintDiameter(50.f);
		Shadow->RefreshFromSettings();
		TestNearlyEqual(TEXT("disc scale X = 50 x 1.2 / 100"), Shadow->GetRelativeScale3D().X, 0.6, 1e-4);
		TestNearlyEqual(TEXT("disc scale Y = 50 x 1.2 / 100"), Shadow->GetRelativeScale3D().Y, 0.6, 1e-4);
		TestNearlyEqual(TEXT("opacity data = 0.6"), OpacityOf(Shadow), 0.6f, 1e-4f);

		// Per-type values.
		Shadow->SetShadowScale(2.f);
		Shadow->SetShadowOpacity(0.4f);
		TestNearlyEqual(TEXT("per-type scale applies"), Shadow->GetRelativeScale3D().X, 1.0, 1e-4);
		TestNearlyEqual(TEXT("per-type opacity applies"), OpacityOf(Shadow), 0.4f, 1e-4f);

		// Global multipliers on top.
		Settings->GlobalScaleMultiplier = 1.5f;
		Settings->GlobalOpacityMultiplier = 2.f;
		Shadow->RefreshFromSettings();
		TestNearlyEqual(TEXT("global scale multiplies"), Shadow->GetRelativeScale3D().X, 1.5, 1e-4);
		TestNearlyEqual(TEXT("global opacity multiplies"), OpacityOf(Shadow), 0.8f, 1e-4f);
		Shadow->SetShadowOpacity(0.9f);
		TestNearlyEqual(TEXT("opacity clamped to 1"), OpacityOf(Shadow), 1.f, 1e-4f);

		// Elliptic footprint (fitted actors).
		Settings->GlobalScaleMultiplier = 1.f;
		Shadow->SetShadowScale(1.f);
		Shadow->SetFootprintSize(FVector2D(130.f, 70.f));
		TestNearlyEqual(TEXT("footprint X"), Shadow->GetRelativeScale3D().X, 1.3, 1e-4);
		TestNearlyEqual(TEXT("footprint Y"), Shadow->GetRelativeScale3D().Y, 0.7, 1e-4);
		TestFalse(TEXT("the shadow casts no light shadow"), Shadow->CastShadow);
	}

	Settings->GlobalOpacityMultiplier = SavedOpacity;
	Settings->GlobalScaleMultiplier = SavedScale;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBlobShadowFollowTest, "RogueRigs.BlobShadow.UnderMinerAndRobotAndFollows",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBlobShadowFollowTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRBlobShadowFollowWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenCell) : nullptr;
	if (!Miner)
	{
		return false;
	}
	TestWorld.Tick();

	URRBlobShadowComponent* MinerShadow = Miner->FindComponentByClass<URRBlobShadowComponent>();
	if (!TestNotNull(TEXT("miner has a shadow"), MinerShadow))
	{
		return false;
	}
	const UCapsuleComponent* MinerCapsule = Miner->GetCapsuleComponent();
	TestUnderCapsule(*this, TEXT("miner"), MinerShadow, MinerCapsule);
	TestTrue(TEXT("miner shadow visible"), MinerShadow->IsVisible());
	TestNearlyEqual(TEXT("miner footprint = capsule diameter"), MinerShadow->GetFootprintDiameter(), MinerCapsule->GetUnscaledCapsuleRadius() * 2.f, 0.01f);
	TestTrue(TEXT("miner casts no light shadow"), CastsNoLightShadow(Miner));

	// Follows a move (dash, knockback and walk all move the actor).
	Miner->SetActorLocation(Miner->GetActorLocation() + FVector(150.f, -80.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	TestUnderCapsule(*this, TEXT("moved miner"), MinerShadow, MinerCapsule);

	const FVector Floor = Grid->GetCellCenter(OpenCell.X - 2, OpenCell.Y);
	ARRChaser* Chaser = SpawnStillChaser(*this, TestWorld, Floor);
	if (!Chaser)
	{
		return false;
	}
	URRBlobShadowComponent* RobotShadow = Chaser->GetBlobShadow();
	if (!TestNotNull(TEXT("robot has a shadow"), RobotShadow))
	{
		return false;
	}
	TestUnderCapsule(*this, TEXT("robot"), RobotShadow, Chaser->GetCapsuleComponent());
	TestTrue(TEXT("robot casts no light shadow"), CastsNoLightShadow(Chaser));
	Chaser->SetActorLocation(Chaser->GetActorLocation() + FVector(-60.f, 40.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	TestUnderCapsule(*this, TEXT("moved robot"), RobotShadow, Chaser->GetCapsuleComponent());
	TestNearlyEqual(TEXT("robot shadow opacity data = its opacity"), OpacityOf(RobotShadow), RobotShadow->GetEffectiveOpacity(), 1e-4f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBlobShadowBarrelTest, "RogueRigs.BlobShadow.OnePerBarrelRemovedWithIt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBlobShadowBarrelTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRBlobShadowBarrelWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid || !TestEqual(TEXT("barrel cell"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel))
	{
		return false;
	}

	int32 BarrelCount = 0;
	for (int32 Y = 0; Y < 200; ++Y)
	{
		for (int32 X = 0; X < 200; ++X)
		{
			BarrelCount += Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::Barrel ? 1 : 0;
		}
	}
	TestEqual(TEXT("one shadow per barrel"), Grid->GetBarrelShadowCount(), BarrelCount);

	URRBlobShadowComponent* Shadow = Grid->GetBarrelShadow(BarrelCell);
	if (!TestNotNull(TEXT("barrel shadow"), Shadow))
	{
		return false;
	}
	const FVector Center = Grid->GetCellCenter(BarrelCell.X, BarrelCell.Y);
	TestTrue(TEXT("barrel shadow centred on its cell"), FVector::Dist2D(Shadow->GetComponentLocation(), Center) < 0.5);
	TestNearlyEqual(TEXT("barrel shadow on the floor top"), Shadow->GetComponentLocation().Z, Center.Z + Shadow->GetFloorOffset(), 0.5);
	TestTrue(TEXT("barrel shadow registered and visible"), Shadow->IsRegistered() && Shadow->IsVisible());
	TestTrue(TEXT("barrel footprint at most one cell"), Shadow->GetFootprintDiameter() > 0.f && Shadow->GetFootprintDiameter() <= Grid->GetCellSize() + 0.01f);

	// Barrel render chunks are named after their template (BarrelInstances_<x>_<y>).
	TInlineComponentArray<UHierarchicalInstancedStaticMeshComponent*> Renders(Grid);
	int32 BarrelRenders = 0;
	for (const UHierarchicalInstancedStaticMeshComponent* Render : Renders)
	{
		if (Render->GetName().StartsWith(TEXT("BarrelInstances_")))
		{
			++BarrelRenders;
			TestFalse(*FString::Printf(TEXT("%s casts no light shadow"), *Render->GetName()), Render->CastShadow);
		}
	}
	TestTrue(TEXT("barrel render chunks found"), BarrelRenders > 0);

	TestTrue(TEXT("barrel removed"), Grid->DestroyCell(BarrelCell, ERRTerrainDamageSource::Extractor));
	TestNull(TEXT("its shadow is gone"), Grid->GetBarrelShadow(BarrelCell));
	TestEqual(TEXT("one shadow less"), Grid->GetBarrelShadowCount(), BarrelCount - 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
