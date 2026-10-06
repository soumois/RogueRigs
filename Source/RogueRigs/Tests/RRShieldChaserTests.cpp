// Rogue Rigs - automation tests of the shield chaser (RogueRigs.ShieldChaser.*): CSV row, shield absorbed before health, single break event before the death, director mix after 1:00 and spawner resolution.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRShieldChaser.h"
#include "Enemies/RRWaveDirector.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"

namespace RRShieldChaserTests
{
	/** Spawner Blueprint whose WeightedEnemyClasses map native robot classes to their Blueprints. */
	const TCHAR* const ShieldSpawnerClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner.BP_EnemySpawner_C");

	/** Spawns a native robot of class T at Location (in the air, no grid needed: damage only); XP drop: ARRTestPickupXP. */
	template <typename T>
	T* SpawnShieldTestRobot(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, const FVector& Location)
	{
		const FTransform SpawnTransform(Location);
		T* Robot = TestWorld.Get()->SpawnActorDeferred<T>(T::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("robot spawned"), Robot))
		{
			RRTestAccess::FEnemyAccess::XPPickupClassOf(Robot) = ARRTestPickupXP::StaticClass();
			Robot->FinishSpawning(SpawnTransform);
		}
		return Robot;
	}

	/** Applies one flat hit of Damage from Source's ASC to Target (URRGE_DamageFlat, no crit). */
	void HitShieldTestRobot(ARREnemy* Source, ARREnemy* Target, float Damage)
	{
		UAbilitySystemComponent* SourceASC = Source->GetAbilitySystemComponent();
		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, SourceASC->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
		SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data, Target->GetAbilitySystemComponent());
	}

	/** Health of Robot. */
	float ShieldTestHealth(const ARREnemy* Robot)
	{
		return Robot->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShieldChaserStatsTest, "RogueRigs.ShieldChaser.StatsFromCSV",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShieldChaserStatsTest::RunTest(const FString& Parameters)
{
	FRREnemyStatsRow Shield;
	FRREnemyStatsRow Chaser;
	if (!TestTrue(TEXT("EnemyStats.csv row ShieldChaser read"), ARREnemy::LoadStatsRow(TEXT("ShieldChaser"), Shield))
		|| !TestTrue(TEXT("EnemyStats.csv row Chaser read"), ARREnemy::LoadStatsRow(TEXT("Chaser"), Chaser)))
	{
		return false;
	}
	TestEqual(TEXT("Shield 30"), Shield.Shield, 30.f);
	TestEqual(TEXT("XPValue 2"), Shield.XPValue, 2);
	TestEqual(TEXT("same MaxHealth as the chaser"), Shield.MaxHealth, Chaser.MaxHealth);
	TestEqual(TEXT("same MoveSpeed as the chaser (360)"), Shield.MoveSpeed, Chaser.MoveSpeed);
	TestEqual(TEXT("same Damage as the chaser"), Shield.Damage, Chaser.Damage);
	TestEqual(TEXT("same AttackRange as the chaser"), Shield.AttackRange, Chaser.AttackRange);
	TestEqual(TEXT("same WindUp as the chaser"), Shield.WindUp, Chaser.WindUp);
	for (const TCHAR* RowName : { TEXT("Chaser"), TEXT("Kamikaze"), TEXT("Shooter"), TEXT("Heavy") })
	{
		FRREnemyStatsRow Row;
		if (TestTrue(FString::Printf(TEXT("row %s read"), RowName), ARREnemy::LoadStatsRow(RowName, Row)))
		{
			TestEqual(FString::Printf(TEXT("%s has no shield"), RowName), Row.Shield, 0.f);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShieldChaserAbsorbTest, "RogueRigs.ShieldChaser.ShieldAbsorbsFirst",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShieldChaserAbsorbTest::RunTest(const FString& Parameters)
{
	using namespace RRShieldChaserTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRShieldChaserAbsorb"));
	ARRShieldChaser* Shielded = SpawnShieldTestRobot<ARRShieldChaser>(*this, TestWorld, FVector(0.f, 0.f, 2000.f));
	ARRChaser* Plain = SpawnShieldTestRobot<ARRChaser>(*this, TestWorld, FVector(500.f, 0.f, 2000.f));
	ARRChaser* Source = SpawnShieldTestRobot<ARRChaser>(*this, TestWorld, FVector(1000.f, 0.f, 2000.f));
	if (!Shielded || !Plain || !Source)
	{
		return false;
	}

	TArray<FString> Events;
	TArray<float> Hits;
	URRHealthAttributeSet* Set = Shielded->GetHealthSet();
	const FDelegateHandle DamageHandle = Set->OnDamageReceived.AddLambda([&Events, &Hits](float Damage, bool, float) { Events.Add(TEXT("Damage")); Hits.Add(Damage); });
	const FDelegateHandle BrokenHandle = Set->OnShieldBroken.AddLambda([&Events]() { Events.Add(TEXT("ShieldBroken")); });
	const FDelegateHandle DeathHandle = Set->OnDeath.AddLambda([&Events](AActor*) { Events.Add(TEXT("Death")); });

	TestEqual(TEXT("spawns with shield 30"), Shielded->GetShield(), 30.f);
	TestEqual(TEXT("max shield 30"), Shielded->GetMaxShield(), 30.f);
	TestEqual(TEXT("shield fraction 1"), Shielded->GetShieldFraction(), 1.f);
	TestTrue(TEXT("has shield"), Shielded->HasShield());
	TestEqual(TEXT("health 30"), ShieldTestHealth(Shielded), 30.f);
	TestEqual(TEXT("reads the ShieldChaser row (XP 2)"), Shielded->GetStats().XPValue, 2);

	// 12: all on the shield.
	HitShieldTestRobot(Source, Shielded, 12.f);
	TestEqual(TEXT("hit 12: shield 18"), Shielded->GetShield(), 18.f);
	TestEqual(TEXT("hit 12: health untouched"), ShieldTestHealth(Shielded), 30.f);
	TestEqual(TEXT("hit 12: fraction 0.6"), Shielded->GetShieldFraction(), 0.6f, 1e-5f);
	TestEqual(TEXT("hit 12: no break yet"), Shielded->GetShieldBreakCount(), 0);

	// 25: 18 on the shield, 7 on health, in the same hit; the shield breaks once.
	HitShieldTestRobot(Source, Shielded, 25.f);
	TestEqual(TEXT("hit 25: shield 0"), Shielded->GetShield(), 0.f);
	TestEqual(TEXT("hit 25: health 30 - 7"), ShieldTestHealth(Shielded), 23.f);
	TestEqual(TEXT("hit 25: fraction 0"), Shielded->GetShieldFraction(), 0.f);
	TestFalse(TEXT("hit 25: no shield left"), Shielded->HasShield());
	TestEqual(TEXT("hit 25: one break"), Shielded->GetShieldBreakCount(), 1);

	// 5: health only, no second break.
	HitShieldTestRobot(Source, Shielded, 5.f);
	TestEqual(TEXT("hit 5: health 18"), ShieldTestHealth(Shielded), 18.f);
	TestEqual(TEXT("hit 5: still one break"), Shielded->GetShieldBreakCount(), 1);

	// 18: dies.
	HitShieldTestRobot(Source, Shielded, 18.f);
	TestTrue(TEXT("hit 18: dead"), Shielded->IsDead());

	Set->OnDamageReceived.Remove(DamageHandle);
	Set->OnShieldBroken.Remove(BrokenHandle);
	Set->OnDeath.Remove(DeathHandle);
	const FString Order = FString::Join(Events, TEXT(","));
	AddInfo(FString::Printf(TEXT("events: %s"), *Order));
	TestEqual(TEXT("event order"), Order, FString(TEXT("Damage,Damage,ShieldBroken,Damage,Damage,Death")));
	TestTrue(TEXT("damage numbers get the raw hits (12, 25, 5, 18)"), Hits == TArray<float>({ 12.f, 25.f, 5.f, 18.f }));

	// A plain chaser has no shield: the hit goes straight to health.
	TestEqual(TEXT("plain chaser shield 0"), Plain->GetHealthSet()->GetShield(), 0.f);
	HitShieldTestRobot(Source, Plain, 5.f);
	TestEqual(TEXT("plain chaser health 25"), ShieldTestHealth(Plain), 25.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShieldChaserBreakAndKillTest, "RogueRigs.ShieldChaser.BreakAndKillSameHit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShieldChaserBreakAndKillTest::RunTest(const FString& Parameters)
{
	using namespace RRShieldChaserTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRShieldChaserBreakAndKill"));
	ARRShieldChaser* Shielded = SpawnShieldTestRobot<ARRShieldChaser>(*this, TestWorld, FVector(0.f, 0.f, 2000.f));
	ARRChaser* Source = SpawnShieldTestRobot<ARRChaser>(*this, TestWorld, FVector(1000.f, 0.f, 2000.f));
	if (!Shielded || !Source)
	{
		return false;
	}
	TArray<FString> Events;
	URRHealthAttributeSet* Set = Shielded->GetHealthSet();
	const FDelegateHandle BrokenHandle = Set->OnShieldBroken.AddLambda([&Events]() { Events.Add(TEXT("ShieldBroken")); });
	const FDelegateHandle DeathHandle = Set->OnDeath.AddLambda([&Events](AActor*) { Events.Add(TEXT("Death")); });

	// 60 = 30 shield + 30 health: both in one hit.
	HitShieldTestRobot(Source, Shielded, 60.f);
	Set->OnShieldBroken.Remove(BrokenHandle);
	Set->OnDeath.Remove(DeathHandle);
	TestTrue(TEXT("dead"), Shielded->IsDead());
	TestEqual(TEXT("one break"), Shielded->GetShieldBreakCount(), 1);
	TestEqual(TEXT("break before death"), FString::Join(Events, TEXT(",")), FString(TEXT("ShieldBroken,Death")));

	// 59 leaves 1 health.
	ARRShieldChaser* Survivor = SpawnShieldTestRobot<ARRShieldChaser>(*this, TestWorld, FVector(0.f, 500.f, 2000.f));
	if (Survivor)
	{
		HitShieldTestRobot(Source, Survivor, 59.f);
		TestFalse(TEXT("59 does not kill (30 shield + 29 health)"), Survivor->IsDead());
		TestEqual(TEXT("1 health left"), ShieldTestHealth(Survivor), 1.f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShieldChaserDirectorTest, "RogueRigs.ShieldChaser.DirectorMix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShieldChaserDirectorTest::RunTest(const FString& Parameters)
{
	using namespace RRShieldChaserTests;

	const FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
	const int32 ShieldType = Config.FindPackType(TEXT("ShieldChasers"));
	if (!TestTrue(TEXT("ShieldChasers pack type"), ShieldType != INDEX_NONE))
	{
		return false;
	}

	// A quarter of the pack chasers carry a shield (mean counts).
	float ShieldMean = 0.f;
	float ChaserMean = 0.f;
	for (const FRRDirectorPackMember& Member : Config.PackTypes[ShieldType].Members)
	{
		const float Mean = 0.5f * (Member.CountMin + Member.CountMax);
		if (Member.RobotClass == ARRShieldChaser::StaticClass())
		{
			ShieldMean += Mean;
		}
		if (Member.RobotClass && Member.RobotClass->IsChildOf(ARRChaser::StaticClass()))
		{
			ChaserMean += Mean;
		}
	}
	AddInfo(FString::Printf(TEXT("ShieldChasers pack: %.1f shield chasers of %.1f chasers"), ShieldMean, ChaserMean));
	TestTrue(TEXT("shield chasers are 25 % of the pack chasers"), ChaserMean > 0.f && FMath::IsNearlyEqual(ShieldMean / ChaserMean, 0.25f, 0.01f));

	// Only the phases from 1:00 draw it (FlowShields first); no other pack type holds a shield chaser.
	int32 FlowIndex = INDEX_NONE;
	int32 ShieldPhaseIndex = INDEX_NONE;
	for (int32 Index = 0; Index < Config.Phases.Num(); ++Index)
	{
		const FRRDirectorPhase& Phase = Config.Phases[Index];
		const bool bDrawsShields = Phase.PackWeights.ContainsByPredicate([](const FRRDirectorPackWeight& Weight) { return Weight.PackType == TEXT("ShieldChasers"); });
		TestEqual(FString::Printf(TEXT("phase %s draws ShieldChasers only from 60 s"), *Phase.Name.ToString()), bDrawsShields, Phase.StartTime >= 60.f);
		FlowIndex = Phase.Name == TEXT("Flow") ? Index : FlowIndex;
		ShieldPhaseIndex = bDrawsShields && ShieldPhaseIndex == INDEX_NONE ? Index : ShieldPhaseIndex;
	}
	for (int32 Type = 0; Type < Config.PackTypes.Num(); ++Type)
	{
		if (Type == ShieldType)
		{
			continue;
		}
		for (const FRRDirectorPackMember& Member : Config.PackTypes[Type].Members)
		{
			TestFalse(FString::Printf(TEXT("%s holds no shield chaser"), *Config.PackTypes[Type].Name.ToString()), Member.RobotClass == ARRShieldChaser::StaticClass());
		}
	}
	if (!TestTrue(TEXT("Flow and FlowShields phases"), FlowIndex != INDEX_NONE && ShieldPhaseIndex != INDEX_NONE))
	{
		return false;
	}

	// The shield phase continues the flow rhythm: same timer, interval picked up where Flow was, same ramp end.
	const FRRDirectorPhase& Flow = Config.Phases[FlowIndex];
	const FRRDirectorPhase& Shields = Config.Phases[ShieldPhaseIndex];
	TestEqual(TEXT("FlowShields at 60 s"), Shields.StartTime, 60.f);
	TestTrue(TEXT("keeps the wave timer"), Shields.bKeepWaveTimer);
	TestFalse(TEXT("no extra wave on start"), Shields.bWaveOnStart);
	TestEqual(TEXT("interval continues"), Shields.WaveIntervalStart, FRRWaveDirector::GetWaveInterval(Flow, Shields.StartTime - Flow.StartTime), 1e-4f);
	TestEqual(TEXT("same end interval"), Shields.WaveIntervalEnd, Flow.WaveIntervalEnd);
	TestEqual(TEXT("ramp ends with Flow's"), Shields.StartTime + Shields.WaveIntervalRampSeconds, Flow.StartTime + Flow.WaveIntervalRampSeconds, 1e-3f);
	TestEqual(TEXT("same sides"), Shields.SidesMin * 10 + Shields.SidesMax, Flow.SidesMin * 10 + Flow.SidesMax);

	// The placed spawner never maps a shield chaser to BP_Chaser: it resolves to a class of ARRShieldChaser (its Blueprint once
	// the WeightedEnemyClasses entry exists, task-028-02; the native class before).
	UClass* SpawnerClass = RRTestHarness::LoadTestClass<ARREnemySpawner>(*this, ShieldSpawnerClassPath);
	if (!SpawnerClass)
	{
		return false;
	}
	const ARREnemySpawner* Spawner = SpawnerClass->GetDefaultObject<ARREnemySpawner>();
	const TSubclassOf<ARREnemy> Resolved = Spawner->ResolveRobotClass(ARRShieldChaser::StaticClass());
	AddInfo(FString::Printf(TEXT("BP_EnemySpawner resolves ARRShieldChaser to %s"), *GetNameSafe(Resolved.Get())));
	TestTrue(TEXT("resolves to a shield chaser class"), Resolved && Resolved->IsChildOf(ARRShieldChaser::StaticClass()));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
