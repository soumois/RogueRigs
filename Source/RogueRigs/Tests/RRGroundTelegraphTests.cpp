// Rogue Rigs - automation tests of the ground telegraph (RogueRigs.GroundTelegraph.*, task-112-01): material parameters, hide on owner death and pool release, no collision, no decals on the miner and robots, destroyed with the owner.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RRChaser.h"
#include "GAS/RRGameplayTags.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRWeapon.h"

namespace RRGroundTelegraphTests
{
	using namespace RRTestHarness;

	/** Adds a registered ground telegraph to Owner (as an owner does at run time). */
	URRGroundTelegraphComponent* AddTelegraph(AActor* Owner)
	{
		URRGroundTelegraphComponent* Telegraph = NewObject<URRGroundTelegraphComponent>(Owner, TEXT("GroundTelegraph"));
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			Telegraph->SetupAttachment(Root);
		}
		else
		{
			Owner->SetRootComponent(Telegraph);
		}
		Owner->AddInstanceComponent(Telegraph);
		Telegraph->RegisterComponent();
		return Telegraph;
	}

	/** Scalar override of Name on Instance; false when the instance holds none. */
	bool FindScalar(const UMaterialInstanceDynamic* Instance, FName Name, float& OutValue)
	{
		for (const FScalarParameterValue& Value : Instance->ScalarParameterValues)
		{
			if (Value.ParameterInfo.Name == Name)
			{
				OutValue = Value.ParameterValue;
				return true;
			}
		}
		return false;
	}

	/** Vector override of Name on Instance; false when the instance holds none. */
	bool FindVector(const UMaterialInstanceDynamic* Instance, FName Name, FLinearColor& OutValue)
	{
		for (const FVectorParameterValue& Value : Instance->VectorParameterValues)
		{
			if (Value.ParameterInfo.Name == Name)
			{
				OutValue = Value.ParameterValue;
				return true;
			}
		}
		return false;
	}

	/** Kills Robot with a flat damage from its own ASC. */
	void Kill(ARREnemy* Robot)
	{
		UAbilitySystemComponent* ASC = Robot->GetAbilitySystemComponent();
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, ASC->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, 100000.f);
		ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data, ASC);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRGroundTelegraphParametersTest, "RogueRigs.GroundTelegraph.ParametersReachMaterial",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRGroundTelegraphParametersTest::RunTest(const FString& Parameters)
{
	using namespace RRGroundTelegraphTests;
	FTestWorld TestWorld(TEXT("RRGroundTelegraphParameters"));
	AActor* Owner = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector(100.f, 200.f, 50.f));
	if (!TestNotNull(TEXT("owner spawned"), Owner))
	{
		return false;
	}
	URRGroundTelegraphComponent* Telegraph = AddTelegraph(Owner);
	TestFalse(TEXT("hidden until Show"), Telegraph->IsShown());
	TestTrue(TEXT("projects straight down"), Telegraph->GetComponentRotation().Vector().Equals(FVector(0.f, 0.f, -1.f), 1.e-3f));

	// Values set before any material are stored.
	Telegraph->SetRadius(300.f);
	Telegraph->SetProgress(1.5f);
	Telegraph->SetMode(ERRGroundTelegraphMode::ZoneActive);
	Telegraph->SetOpacity(0.5f);
	TestEqual(TEXT("radius stored"), Telegraph->GetRadius(), 300.f);
	TestEqual(TEXT("progress clamped to 1"), Telegraph->GetProgress(), 1.f);
	TestEqual(TEXT("decal box half size = radius"), Telegraph->DecalSize.Y, 300.0);
	TestEqual(TEXT("decal box half size = radius (Z)"), Telegraph->DecalSize.Z, 300.0);
	TestTrue(TEXT("danger colour orange"), Telegraph->GetModeColor(ERRGroundTelegraphMode::Danger).Equals(FLinearColor(1.f, 0.45f, 0.05f, 1.f)));
	TestTrue(TEXT("blocked colour gold"), Telegraph->GetModeColor(ERRGroundTelegraphMode::ZoneBlocked).Equals(FLinearColor(1.f, 0.8f, 0.2f, 1.f)));

	// The material appears (here a transient decal material): the stored values reach the dynamic instance.
	UMaterial* Material = NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
	Material->MaterialDomain = MD_DeferredDecal;
	Telegraph->SetTelegraphMaterial(Material);
	UMaterialInstanceDynamic* Instance = Telegraph->GetTelegraphMaterialInstance();
	if (!TestNotNull(TEXT("dynamic material instance created"), Instance))
	{
		return false;
	}
	TestTrue(TEXT("instance parented to the material"), Instance->Parent == Material);
	TestTrue(TEXT("decal draws the instance"), Telegraph->GetDecalMaterial() == Instance);
	float Scalar = -1.f;
	TestTrue(TEXT("Radius set"), FindScalar(Instance, URRGroundTelegraphComponent::RadiusParameterName, Scalar) && Scalar == 300.f);
	TestTrue(TEXT("Progress set"), FindScalar(Instance, URRGroundTelegraphComponent::ProgressParameterName, Scalar) && Scalar == 1.f);
	TestTrue(TEXT("Opacity set"), FindScalar(Instance, URRGroundTelegraphComponent::OpacityParameterName, Scalar) && Scalar == 0.5f);
	FLinearColor Color;
	TestTrue(TEXT("ZoneColor = white-blue"), FindVector(Instance, URRGroundTelegraphComponent::ZoneColorParameterName, Color) && Color.Equals(FLinearColor(0.7f, 0.9f, 1.f, 1.f)));
	TestTrue(TEXT("ProgressColor = white"), FindVector(Instance, URRGroundTelegraphComponent::ProgressColorParameterName, Color) && Color.Equals(FLinearColor::White));

	// Later changes go straight to the instance.
	Telegraph->SetProgress(0.25f);
	Telegraph->SetMode(ERRGroundTelegraphMode::Danger);
	Telegraph->SetRadius(120.f);
	TestTrue(TEXT("Progress updated"), FindScalar(Instance, URRGroundTelegraphComponent::ProgressParameterName, Scalar) && Scalar == 0.25f);
	TestTrue(TEXT("Radius updated"), FindScalar(Instance, URRGroundTelegraphComponent::RadiusParameterName, Scalar) && Scalar == 120.f);
	TestTrue(TEXT("ZoneColor = orange"), FindVector(Instance, URRGroundTelegraphComponent::ZoneColorParameterName, Color) && Color.Equals(FLinearColor(1.f, 0.45f, 0.05f, 1.f)));
	TestTrue(TEXT("Show on a plain owner"), Telegraph->Show());
	TestTrue(TEXT("shown"), Telegraph->IsShown());
	Telegraph->Hide();
	TestFalse(TEXT("hidden"), Telegraph->IsShown());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRGroundTelegraphNoCollisionTest, "RogueRigs.GroundTelegraph.NoCollisionAndDestroyedWithOwner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRGroundTelegraphNoCollisionTest::RunTest(const FString& Parameters)
{
	using namespace RRGroundTelegraphTests;
	FTestWorld TestWorld(TEXT("RRGroundTelegraphNoCollision"));
	ARRChaser* Robot = SpawnStillChaser(*this, TestWorld, FVector::ZeroVector);
	if (!Robot)
	{
		return false;
	}
	URRGroundTelegraphComponent* Telegraph = AddTelegraph(Robot);
	// A decal is a scene component: no body, so no collision profile, no block, no overlap event.
	TestNull(TEXT("not a primitive (no collision, no overlap)"), Cast<UPrimitiveComponent>(Telegraph));
	TestFalse(TEXT("not navigation relevant"), Telegraph->CanEverAffectNavigation());
	TestFalse(TEXT("robot mesh receives no decals"), Robot->GetMesh()->bReceivesDecals);

	TWeakObjectPtr<URRGroundTelegraphComponent> WeakTelegraph(Telegraph);
	Robot->Destroy();
	TestTrue(TEXT("destroyed with its owner"), !WeakTelegraph.IsValid() || WeakTelegraph->IsBeingDestroyed());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRGroundTelegraphOwnerDeathTest, "RogueRigs.GroundTelegraph.HiddenOnOwnerDeath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRGroundTelegraphOwnerDeathTest::RunTest(const FString& Parameters)
{
	using namespace RRGroundTelegraphTests;
	FTestWorld TestWorld(TEXT("RRGroundTelegraphOwnerDeath"));
	ARRChaser* Robot = SpawnStillChaser(*this, TestWorld, FVector::ZeroVector);
	if (!Robot)
	{
		return false;
	}
	URRGroundTelegraphComponent* Telegraph = AddTelegraph(Robot);
	TestTrue(TEXT("shown on a live robot"), Telegraph->Show());
	Kill(Robot);
	TestTrue(TEXT("robot dead"), Robot->IsDead());
	TestFalse(TEXT("hidden at once on death"), Telegraph->IsShown());
	TestFalse(TEXT("Show refused on a dead owner"), Telegraph->Show());
	TestFalse(TEXT("still hidden"), Telegraph->IsShown());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRGroundTelegraphPoolTest, "RogueRigs.GroundTelegraph.HiddenOnPoolRelease",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRGroundTelegraphPoolTest::RunTest(const FString& Parameters)
{
	using namespace RRGroundTelegraphTests;
	FTestWorld TestWorld(TEXT("RRGroundTelegraphPool"));
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	if (!TestNotNull(TEXT("actor pool"), Pool))
	{
		return false;
	}
	const FTransform SpawnTransform(FVector(0.f, 0.f, 100.f));
	ARRChaser* Robot = Pool->AcquireImmediate<ARRChaser>(ARRChaser::StaticClass(), SpawnTransform, FRRPoolAcquireParams());
	if (!TestNotNull(TEXT("robot from the pool"), Robot))
	{
		return false;
	}
	URRGroundTelegraphComponent* Telegraph = AddTelegraph(Robot);
	TestTrue(TEXT("shown"), Telegraph->Show());
	TestTrue(TEXT("released"), Pool->Release(Robot));
	TestFalse(TEXT("hidden at once on pool release"), Telegraph->IsShown());
	TestFalse(TEXT("Show refused while pooled idle"), Telegraph->Show());

	ARRChaser* Reused = Pool->AcquireImmediate<ARRChaser>(ARRChaser::StaticClass(), SpawnTransform, FRRPoolAcquireParams());
	TestTrue(TEXT("same robot reused"), Reused == Robot);
	TestFalse(TEXT("still hidden on reuse until its owner shows it"), Telegraph->IsShown());
	TestTrue(TEXT("Show works on the new life"), Telegraph->Show());
	TestTrue(TEXT("shown again"), Telegraph->IsShown());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRGroundTelegraphNoDecalsTest, "RogueRigs.GroundTelegraph.MinerAndRobotsReceiveNoDecals",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRGroundTelegraphNoDecalsTest::RunTest(const FString& Parameters)
{
	using namespace RRGroundTelegraphTests;
	FTestWorld TestWorld(TEXT("RRGroundTelegraphNoDecals"));
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, FineCell(FIntPoint(40, 40))) : nullptr;
	if (!Miner)
	{
		return false;
	}
	auto CheckActor = [this](const AActor* Actor, const FString& What)
	{
		TInlineComponentArray<UPrimitiveComponent*> Primitives(Actor);
		for (const UPrimitiveComponent* Primitive : Primitives)
		{
			TestFalse(FString::Printf(TEXT("%s %s receives no decals"), *What, *Primitive->GetName()), Primitive->bReceivesDecals);
		}
	};
	CheckActor(Miner, TEXT("miner"));
	for (const ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon))
		{
			CheckActor(Weapon, Weapon->GetName());
		}
	}
	ARRChaser* Robot = SpawnStillChaser(*this, TestWorld, Miner->GetActorLocation() + FVector(400.f, 0.f, -90.f));
	if (Robot)
	{
		CheckActor(Robot, TEXT("robot"));
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
