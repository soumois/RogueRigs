// Rogue Rigs - automation tests of the player damage rules (RogueRigs.Player.*): flat damage and AttackPower scaling, i-frames and blink, single death.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPlayerTests
{
	using namespace RRTestHarness;

	/** Health the tests give the miner before hitting it (miner gdd base value). */
	constexpr float TestMaxHealth = 100.f;

	/** Flat damage of one test hit (task-009-01: 10 lowers 100 to 90). */
	constexpr float HitDamage = 10.f;

	/** Spawns the grid and BP_Miner on a floor cell, with Health = MaxHealth = TestMaxHealth. Null on failure. */
	ARRMiner* SpawnTestMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld)
	{
		ARRArenaGrid* Grid = SpawnGrid(Test, TestWorld);
		if (!Grid)
		{
			return nullptr;
		}
		ARRMiner* Miner = SpawnMiner(Test, TestWorld, Grid, RockWestFloorCell);
		if (!Miner || !Test.TestNotNull(TEXT("miner health set"), Miner->GetHealthSet()))
		{
			return nullptr;
		}
		UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent();
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), TestMaxHealth);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), TestMaxHealth);
		return Miner;
	}

	/** Spawns a bare actor with an ASC and a URREnemyAttributeSet (AttackPower given): the damage source of the tests. */
	UAbilitySystemComponent* SpawnEnemySource(const FTestWorld& TestWorld, float AttackPower)
	{
		AActor* Source = TestWorld.Get()->SpawnActor<AActor>();
		URRAbilitySystemComponent* ASC = NewObject<URRAbilitySystemComponent>(Source, TEXT("AbilitySystemComponent"));
		ASC->RegisterComponent();
		ASC->AddSpawnedAttribute(NewObject<URREnemyAttributeSet>(Source, TEXT("EnemySet")));
		ASC->InitAbilityActorInfo(Source, Source);
		ASC->SetNumericAttributeBase(URREnemyAttributeSet::GetAttackPowerAttribute(), AttackPower);
		return ASC;
	}

	/** Applies a URRGE_DamageFlat spec made by Source (SetByCaller Data.Damage = Damage) to Target. */
	void ApplyFlatDamage(UAbilitySystemComponent* Source, UAbilitySystemComponent* Target, float Damage)
	{
		FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, Source->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
		Source->ApplyGameplayEffectSpecToTarget(*Spec.Data, Target);
	}

	/** Current Health of the miner. */
	float GetHealth(const ARRMiner* Miner)
	{
		return Miner->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Ticks the world until the i-frames of the last hit are over (duration + 2 frames). */
	void WaitPastIFrames(const FTestWorld& TestWorld, const ARRMiner* Miner)
	{
		const int32 Frames = FMath::CeilToInt32(Miner->GetIFramesDuration() / FrameSeconds) + 2;
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPlayerFlatDamageTest, "RogueRigs.Player.FlatDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPlayerFlatDamageTest::RunTest(const FString& Parameters)
{
	using namespace RRPlayerTests;

	FTestWorld TestWorld(TEXT("RRPlayerFlatDamage"));
	ARRMiner* Miner = SpawnTestMiner(*this, TestWorld);
	if (!Miner)
	{
		return false;
	}
	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();

	// AttackPower 1: the SetByCaller amount as is.
	UAbilitySystemComponent* Enemy = SpawnEnemySource(TestWorld, 1.f);
	ApplyFlatDamage(Enemy, MinerASC, HitDamage);
	TestEqual(TEXT("flat 10 lowers Health to 90"), GetHealth(Miner), 90.f);
	TestTrue(TEXT("hit grants State.Invulnerable"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable));
	TestEqual(TEXT("IncomingDamage consumed"), MinerASC->GetNumericAttribute(URRHealthAttributeSet::GetIncomingDamageAttribute()), 0.f);

	// AttackPower 2 doubles the flat amount.
	WaitPastIFrames(TestWorld, Miner);
	UAbilitySystemComponent* StrongEnemy = SpawnEnemySource(TestWorld, 2.f);
	ApplyFlatDamage(StrongEnemy, MinerASC, HitDamage);
	TestEqual(TEXT("AttackPower 2 deals 20"), GetHealth(Miner), 70.f);

	// A source without URREnemyAttributeSet scales by 1.
	WaitPastIFrames(TestWorld, Miner);
	ApplyFlatDamage(MinerASC, MinerASC, HitDamage);
	TestEqual(TEXT("source without AttackPower deals the flat amount"), GetHealth(Miner), 60.f);
	TestFalse(TEXT("not dead"), Miner->IsDead());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPlayerIFramesTest, "RogueRigs.Player.IFrames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPlayerIFramesTest::RunTest(const FString& Parameters)
{
	using namespace RRPlayerTests;

	FTestWorld TestWorld(TEXT("RRPlayerIFrames"));
	ARRMiner* Miner = SpawnTestMiner(*this, TestWorld);
	if (!Miner)
	{
		return false;
	}
	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	USkeletalMeshComponent* Mesh = Miner->GetMesh();
	TestEqual(TEXT("i-frames last 0.25 s"), Miner->GetIFramesDuration(), 0.25f);
	TestTrue(TEXT("mesh visible before any hit"), Mesh->IsVisible());

	UAbilitySystemComponent* Enemy = SpawnEnemySource(TestWorld, 1.f);
	ApplyFlatDamage(Enemy, MinerASC, HitDamage);
	TestEqual(TEXT("first hit applied"), GetHealth(Miner), 90.f);

	// Same frame: refused by State.Invulnerable, applied in the first hit's call stack.
	ApplyFlatDamage(Enemy, MinerASC, HitDamage);
	TestEqual(TEXT("second hit in the same frame refused"), GetHealth(Miner), 90.f);

	// Still inside the window (one frame before its end): refused; the mesh blinks meanwhile.
	const float Duration = Miner->GetIFramesDuration();
	const int32 FramesInside = FMath::FloorToInt32(Duration / FrameSeconds) - 1;
	bool bSawHidden = !Mesh->IsVisible();
	bool bSawVisible = false;
	for (int32 Frame = 0; Frame < FramesInside; ++Frame)
	{
		TestWorld.Tick();
		bSawHidden |= !Mesh->IsVisible();
		bSawVisible |= Mesh->IsVisible();
	}
	TestTrue(TEXT("still invulnerable before the end"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable));
	ApplyFlatDamage(Enemy, MinerASC, HitDamage);
	TestEqual(FString::Printf(TEXT("hit at %.3f s refused"), FramesInside * FrameSeconds), GetHealth(Miner), 90.f);
	TestTrue(TEXT("mesh hidden during the i-frames"), bSawHidden);
	TestTrue(TEXT("mesh shown again during the i-frames (blink)"), bSawVisible);

	// After the window: applied, mesh restored.
	WaitPastIFrames(TestWorld, Miner);
	TestFalse(TEXT("invulnerability over"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable));
	TestTrue(TEXT("mesh visible after the i-frames"), Mesh->IsVisible());
	ApplyFlatDamage(Enemy, MinerASC, HitDamage);
	TestEqual(TEXT("hit after the i-frames applied"), GetHealth(Miner), 80.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPlayerDeathOnceTest, "RogueRigs.Player.DeathOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPlayerDeathOnceTest::RunTest(const FString& Parameters)
{
	using namespace RRPlayerTests;

	FTestWorld TestWorld(TEXT("RRPlayerDeathOnce"));
	ARRMiner* Miner = SpawnTestMiner(*this, TestWorld);
	if (!Miner)
	{
		return false;
	}
	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	URRHealthAttributeSet* HealthSet = Miner->GetHealthSet();

	int32 DeathCount = 0;
	int32 HitCount = 0;
	AActor* DeathInstigator = nullptr;
	const FDelegateHandle DeathHandle = HealthSet->OnDeath.AddLambda([&DeathCount, &DeathInstigator](AActor* Instigator)
	{
		++DeathCount;
		DeathInstigator = Instigator;
	});
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&HitCount](float, bool, float) { ++HitCount; });

	UAbilitySystemComponent* Enemy = SpawnEnemySource(TestWorld, 1.f);

	// Lethal hit: Health 100 -> 0, death fires once with the enemy as instigator; no i-frames on a lethal hit.
	ApplyFlatDamage(Enemy, MinerASC, 150.f);
	TestEqual(TEXT("Health at 0"), GetHealth(Miner), 0.f);
	TestEqual(TEXT("death fired once"), DeathCount, 1);
	TestTrue(TEXT("death instigator is the enemy"), DeathInstigator == Enemy->GetOwnerActor());
	TestTrue(TEXT("State.Dead added"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead));
	TestTrue(TEXT("IsDead"), Miner->IsDead());
	TestFalse(TEXT("no i-frames on the lethal hit"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable));

	// Controls cut: weapons stopped, move input ignored.
	for (const ARRWeapon* Weapon : Miner->GetWeapons())
	{
		TestFalse(TEXT("weapon stopped firing on death"), Weapon->GetAutoFireComponent()->IsFiring());
	}
	Miner->ApplyMoveInput(FVector2D(1.f, 0.f));
	TestTrue(TEXT("move input ignored when dead"), Miner->GetPendingMovementInputVector().IsNearlyZero());

	// Further damage, after any i-frames time: refused, no second death.
	WaitPastIFrames(TestWorld, Miner);
	ApplyFlatDamage(Enemy, MinerASC, HitDamage);
	TestEqual(TEXT("Health still 0"), GetHealth(Miner), 0.f);
	TestEqual(TEXT("damage on a dead miner refused"), HitCount, 1);
	TestEqual(TEXT("death still fired once"), DeathCount, 1);

	HealthSet->OnDeath.Remove(DeathHandle);
	HealthSet->OnDamageReceived.Remove(HitHandle);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
