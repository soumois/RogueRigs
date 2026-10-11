// Rogue Rigs - automation tests of the pooled pickup scale (RogueRigs.PickupPool.*, task-095-01): a reused pickup equals a fresh one, robot drops keep the Blueprint scale.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Debug/RRStressSubsystem.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "Particles/ParticleSystemComponent.h"
#include "Pickups/RRPickup.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"

namespace RRPickupScaleTests
{
	using RRTestHarness::FTestWorld;

	const TCHAR* const XPPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupXP.BP_PickupXP_C");
	const TCHAR* const OrePickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupOre.BP_PickupOre_C");
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");

	/** Root scale of a fresh pickup of Class spawned at scale 1 (the Blueprint PickupMesh scale, 0.3 for the cubes). */
	FVector DefaultRootScale(UClass* Class)
	{
		const AActor* Defaults = Class ? Cast<AActor>(Class->GetDefaultObject()) : nullptr;
		const USceneComponent* Root = Defaults ? Defaults->GetRootComponent() : nullptr;
		return Root ? Root->GetRelativeScale3D() : FVector::OneVector;
	}

	/** Compares Actual with Fresh: actor scale and rotation, visibility, and every component's relative scale, rotation, visibility and FX state. */
	void ExpectSameAsFresh(FAutomationTestBase& Test, const FString& Name, const AActor* Actual, const AActor* Fresh)
	{
		Test.TestTrue(FString::Printf(TEXT("%s actor scale %s equals fresh %s"), *Name, *Actual->GetActorScale3D().ToString(),
			*Fresh->GetActorScale3D().ToString()), Actual->GetActorScale3D().Equals(Fresh->GetActorScale3D(), 1e-4f));
		Test.TestTrue(FString::Printf(TEXT("%s actor rotation equals fresh"), *Name), Actual->GetActorRotation().Equals(Fresh->GetActorRotation(), 0.01f));
		Test.TestEqual(FString::Printf(TEXT("%s hidden state equals fresh"), *Name), Actual->IsHidden(), Fresh->IsHidden());
		TInlineComponentArray<USceneComponent*> FreshComponents(Fresh);
		TInlineComponentArray<USceneComponent*> ActualComponents(Actual);
		Test.TestEqual(FString::Printf(TEXT("%s component count"), *Name), ActualComponents.Num(), FreshComponents.Num());
		for (const USceneComponent* FreshComponent : FreshComponents)
		{
			const USceneComponent* const* Found = ActualComponents.FindByPredicate([FreshComponent](const USceneComponent* Component)
			{
				return Component->GetFName() == FreshComponent->GetFName();
			});
			if (!Test.TestNotNull(FString::Printf(TEXT("%s has %s"), *Name, *FreshComponent->GetName()), Found))
			{
				continue;
			}
			const USceneComponent* Component = *Found;
			const FString Label = FString::Printf(TEXT("%s %s"), *Name, *Component->GetName());
			Test.TestTrue(FString::Printf(TEXT("%s relative scale %s equals fresh %s"), *Label, *Component->GetRelativeScale3D().ToString(),
				*FreshComponent->GetRelativeScale3D().ToString()), Component->GetRelativeScale3D().Equals(FreshComponent->GetRelativeScale3D(), 1e-4f));
			Test.TestTrue(FString::Printf(TEXT("%s world scale equals fresh"), *Label),
				Component->GetComponentScale().Equals(FreshComponent->GetComponentScale(), 1e-4f));
			Test.TestTrue(FString::Printf(TEXT("%s relative rotation equals fresh"), *Label),
				Component->GetRelativeRotation().Equals(FreshComponent->GetRelativeRotation(), 0.01f));
			Test.TestEqual(FString::Printf(TEXT("%s visibility equals fresh"), *Label), Component->IsVisible(), FreshComponent->IsVisible());
			if (Cast<UFXSystemComponent>(Component))
			{
				Test.TestEqual(FString::Printf(TEXT("%s FX active state equals fresh"), *Label), Component->IsActive(), FreshComponent->IsActive());
			}
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupPoolReuseScaleTest, "RogueRigs.PickupPool.ReusedPickupEqualsFresh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupPoolReuseScaleTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupScaleTests;

	// task-095-01: reused cubes came back 3.3 times bigger (root scale 1 instead of the Blueprint 0.3). For each pickup
	// Blueprint: a used pickup (shrunk, turned, magnet trail on) is released and acquired again; it must equal a fresh one.
	FTestWorld TestWorld(TEXT("RRPickupPoolReuseScale"));
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	if (!TestNotNull(TEXT("actor pool"), Pool))
	{
		return false;
	}
	const TCHAR* const Paths[] = { XPPickupClassPath, OrePickupClassPath, HealPickupClassPath };
	for (const TCHAR* Path : Paths)
	{
		UClass* Class = RRTestHarness::LoadTestClass<ARRPickup>(*this, Path);
		if (!Class)
		{
			return false;
		}
		const FString Name = Class->GetName();
		const FTransform First(FVector(0.f, 0.f, 100.f));
		const FTransform Second(FVector(300.f, 200.f, 100.f));
		ARRPickup* Used = Pool->AcquireImmediate<ARRPickup>(Class, First, FRRPoolAcquireParams());
		ARRPickup* Fresh = Pool->AcquireImmediate<ARRPickup>(Class, Second, FRRPoolAcquireParams());
		if (!TestNotNull(*FString::Printf(TEXT("%s used pickup"), *Name), Used) || !TestNotNull(*FString::Printf(TEXT("%s fresh pickup"), *Name), Fresh))
		{
			return false;
		}
		TestTrue(FString::Printf(TEXT("%s fresh pickup has the Blueprint scale"), *Name),
			Fresh->GetActorScale3D().Equals(DefaultRootScale(Class), 1e-4f));

		// A life: pops, turns to face the miner, trail on, shrinks.
		for (int32 Frame = 0; Frame < 5; ++Frame)
		{
			TestWorld.Tick();
		}
		Used->SetActorRotation(FRotator(0.f, 73.f, 0.f));
		TInlineComponentArray<UFXSystemComponent*> Effects(Used);
		for (UFXSystemComponent* Effect : Effects)
		{
			Effect->Activate(true);
		}
		Used->SetActorScale3D(Used->GetActorScale3D() * 0.2f);
		Pool->Release(Used);

		ARRPickup* Reused = Pool->AcquireImmediate<ARRPickup>(Class, Second, FRRPoolAcquireParams());
		TestTrue(FString::Printf(TEXT("%s same pickup reused"), *Name), Reused == Used);
		if (!Reused)
		{
			return false;
		}
		ExpectSameAsFresh(*this, Name, Reused, Fresh);

		// A second cycle without any mutation: the reuse must not drift either.
		Pool->Release(Reused);
		Reused = Pool->AcquireImmediate<ARRPickup>(Class, Second, FRRPoolAcquireParams());
		if (Reused)
		{
			ExpectSameAsFresh(*this, Name + TEXT(" (second reuse)"), Reused, Fresh);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupPoolShrinkDropScaleTest, "RogueRigs.PickupPool.ShrinkingRobotDropsNormalCubes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupPoolShrinkDropScaleTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupScaleTests;

	// task-095-01: a robot shrinking in its death animation drops BP_PickupXP cubes, some reused (idle in the pool), some
	// created; every cube has the Blueprint scale whatever the robot scale.
	FTestWorld TestWorld(TEXT("RRPickupPoolShrinkDrop"));
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	UClass* XPClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, XPPickupClassPath);
	if (!TestNotNull(TEXT("actor pool"), Pool) || !XPClass)
	{
		return false;
	}
	const FVector Expected = DefaultRootScale(XPClass);

	// One idle cube that lived (shrunk) before: the first cube of the burst reuses it.
	ARRPickup* Previous = Pool->AcquireImmediate<ARRPickup>(XPClass, FTransform(FVector(0.f, -900.f, 100.f)), FRRPoolAcquireParams());
	if (!TestNotNull(TEXT("previous cube"), Previous))
	{
		return false;
	}
	Previous->SetActorScale3D(Previous->GetActorScale3D() * 0.2f);
	Pool->Release(Previous);

	const float HalfHeight = GetDefault<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FTransform SpawnTransform(FVector(0.f, 0.f, HalfHeight + 2.f));
	ARRChaser* Robot = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ARRChaser::StaticClass(), SpawnTransform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!TestNotNull(TEXT("robot spawned"), Robot))
	{
		return false;
	}
	RRTestAccess::FEnemyAccess::XPPickupClassOf(Robot) = XPClass;
	Robot->FinishSpawning(SpawnTransform);
	const FVector Feet = Robot->GetActorLocation() - FVector(0.f, 0.f, HalfHeight);
	TestTrue(TEXT("robot killed"), URRStressSubsystem::KillEnemy(Robot));

	float SmallestRobotScale = 1.f;
	TArray<ARRPickup*> Cubes;
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
		if (IsValid(Robot) && !URRActorPoolSubsystem::IsPooledIdle(Robot))
		{
			SmallestRobotScale = FMath::Min(SmallestRobotScale, static_cast<float>(Robot->GetActorScale3D().X));
		}
	}
	TestTrue(FString::Printf(TEXT("robot shrank before the drop (smallest scale %.3f)"), SmallestRobotScale), SmallestRobotScale < 0.9f);
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		if (!URRActorPoolSubsystem::IsPooledIdle(*It) && FVector::Dist2D(It->GetPopStart(), Feet) < 1.f)
		{
			Cubes.Add(*It);
		}
	}
	TestTrue(FString::Printf(TEXT("robot dropped cubes (%d)"), Cubes.Num()), Cubes.Num() > 0);
	TestTrue(TEXT("the idle cube was reused"), Cubes.Contains(Previous));
	for (const ARRPickup* Cube : Cubes)
	{
		TestTrue(FString::Printf(TEXT("%s scale %s is the Blueprint scale %s"), *Cube->GetName(), *Cube->GetActorScale3D().ToString(), *Expected.ToString()),
			Cube->GetActorScale3D().Equals(Expected, 1e-4f));
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
