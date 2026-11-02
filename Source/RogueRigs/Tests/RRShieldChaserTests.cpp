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
#include "Tests/RRTestStatsRows.h"

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
	// The values live in the CSV (task-420): the ShieldChaser row must be usable, each robot carries its own row. Rows are not
	// compared with each other: a hand edit of one row must not break the test.
	TestTrue(TEXT("ShieldChaser Shield > 0"), Shield.Shield > 0.f);
	TestTrue(TEXT("ShieldChaser MaxHealth > 0"), Shield.MaxHealth > 0.f);
	TestTrue(TEXT("ShieldChaser MoveSpeed > 0"), Shield.MoveSpeed > 0.f);
	TestTrue(TEXT("ShieldChaser Damage > 0"), Shield.Damage > 0.f);
	TestTrue(TEXT("ShieldChaser AttackRange > 0"), Shield.AttackRange > 0.f);
	TestTrue(TEXT("ShieldChaser XPValue > 0"), Shield.XPValue > 0);

	RRTestHarness::FTestWorld TestWorld(TEXT("RRShieldChaserStats"));
	ARRShieldChaser* Shielded = RRShieldChaserTests::SpawnShieldTestRobot<ARRShieldChaser>(*this, TestWorld, FVector(0.f, 0.f, 2000.f));
	ARRChaser* Plain = RRShieldChaserTests::SpawnShieldTestRobot<ARRChaser>(*this, TestWorld, FVector(500.f, 0.f, 2000.f));
	if (!Shielded || !Plain)
	{
		return false;
	}
	TestEqual(TEXT("shield chaser MaxShield = ShieldChaser Shield"), Shielded->GetMaxShield(), Shield.Shield);
	TestEqual(TEXT("shield chaser health = ShieldChaser MaxHealth"), RRShieldChaserTests::ShieldTestHealth(Shielded), Shield.MaxHealth);
	TestEqual(TEXT("plain chaser shield = Chaser Shield"), Plain->GetHealthSet()->GetShield(), Chaser.Shield);
	TestEqual(TEXT("plain chaser health = Chaser MaxHealth"), RRShieldChaserTests::ShieldTestHealth(Plain), Chaser.MaxHealth);
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

	// CSV row ShieldChaser: Shield S, MaxHealth H. Hits derived from them: 0.4 S (shield only), 0.6 S + H / 4 (breaks the shield
	// and reaches health in the same hit), H / 4 (health only), H / 2 (kills).
	const FRREnemyStatsRow Row = RRTestStats::EnemyRow(*this, RRTestStats::ShieldChaserRow);
	const float S = Row.Shield;
	const float H = Row.MaxHealth;
	if (!TestTrue(TEXT("CSV: ShieldChaser Shield and MaxHealth > 0"), S > 0.f && H > 0.f))
	{
		return false;
	}
	const float Hit1 = 0.4f * S;
	const float Hit2 = 0.6f * S + 0.25f * H;
	const float Hit3 = 0.25f * H;
	const float Hit4 = 0.5f * H;
	TestEqual(TEXT("spawns with the CSV shield"), Shielded->GetShield(), S);
	TestEqual(TEXT("max shield = CSV Shield"), Shielded->GetMaxShield(), S);
	TestEqual(TEXT("shield fraction 1"), Shielded->GetShieldFraction(), 1.f);
	TestTrue(TEXT("has shield"), Shielded->HasShield());
	TestEqual(TEXT("health = CSV MaxHealth"), ShieldTestHealth(Shielded), H);
	TestEqual(TEXT("reads the ShieldChaser row (XPValue)"), Shielded->GetStats().XPValue, Row.XPValue);

	// Hit 1: all on the shield.
	HitShieldTestRobot(Source, Shielded, Hit1);
	TestEqual(TEXT("hit 1: shield 0.6 S"), Shielded->GetShield(), S - Hit1, 1e-4f);
	TestEqual(TEXT("hit 1: health untouched"), ShieldTestHealth(Shielded), H);
	TestEqual(TEXT("hit 1: fraction 0.6"), Shielded->GetShieldFraction(), 0.6f, 1e-5f);
	TestEqual(TEXT("hit 1: no break yet"), Shielded->GetShieldBreakCount(), 0);

	// Hit 2: the rest of the shield, then H / 4 on health, in the same hit; the shield breaks once.
	HitShieldTestRobot(Source, Shielded, Hit2);
	TestEqual(TEXT("hit 2: shield 0"), Shielded->GetShield(), 0.f);
	TestEqual(TEXT("hit 2: health H - H / 4"), ShieldTestHealth(Shielded), H - 0.25f * H, 1e-3f);
	TestEqual(TEXT("hit 2: fraction 0"), Shielded->GetShieldFraction(), 0.f);
	TestFalse(TEXT("hit 2: no shield left"), Shielded->HasShield());
	TestEqual(TEXT("hit 2: one break"), Shielded->GetShieldBreakCount(), 1);

	// Hit 3: health only, no second break.
	HitShieldTestRobot(Source, Shielded, Hit3);
	TestEqual(TEXT("hit 3: health H / 2"), ShieldTestHealth(Shielded), 0.5f * H, 1e-3f);
	TestEqual(TEXT("hit 3: still one break"), Shielded->GetShieldBreakCount(), 1);

	// Hit 4: dies.
	HitShieldTestRobot(Source, Shielded, Hit4);
	TestTrue(TEXT("hit 4: dead"), Shielded->IsDead());

	Set->OnDamageReceived.Remove(DamageHandle);
	Set->OnShieldBroken.Remove(BrokenHandle);
	Set->OnDeath.Remove(DeathHandle);
	const FString Order = FString::Join(Events, TEXT(","));
	AddInfo(FString::Printf(TEXT("events: %s"), *Order));
	TestEqual(TEXT("event order"), Order, FString(TEXT("Damage,Damage,ShieldBroken,Damage,Damage,Death")));
	TestTrue(TEXT("damage numbers get the raw hits"), Hits == TArray<float>({ Hit1, Hit2, Hit3, Hit4 }));

	// A plain chaser carries its own row's shield; a hit of 5 (scenario value) goes to it first, then to health.
	const FRREnemyStatsRow ChaserRow = RRTestStats::EnemyRow(*this, RRTestStats::ChaserRow);
	TestEqual(TEXT("plain chaser shield = Chaser Shield"), Plain->GetHealthSet()->GetShield(), ChaserRow.Shield);
	HitShieldTestRobot(Source, Plain, 5.f);
	TestEqual(TEXT("plain chaser health = MaxHealth - (5 - Shield)"), ShieldTestHealth(Plain), ChaserRow.MaxHealth - FMath::Max(5.f - ChaserRow.Shield, 0.f));
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

	// Shield + MaxHealth (CSV row ShieldChaser): both in one hit.
	const FRREnemyStatsRow Row = RRTestStats::EnemyRow(*this, RRTestStats::ShieldChaserRow);
	HitShieldTestRobot(Source, Shielded, Row.Shield + Row.MaxHealth);
	Set->OnShieldBroken.Remove(BrokenHandle);
	Set->OnDeath.Remove(DeathHandle);
	TestTrue(TEXT("dead"), Shielded->IsDead());
	TestEqual(TEXT("one break"), Shielded->GetShieldBreakCount(), 1);
	TestEqual(TEXT("break before death"), FString::Join(Events, TEXT(",")), FString(TEXT("ShieldBroken,Death")));

	// Shield + MaxHealth - 1 leaves 1 health.
	ARRShieldChaser* Survivor = SpawnShieldTestRobot<ARRShieldChaser>(*this, TestWorld, FVector(0.f, 500.f, 2000.f));
	if (Survivor)
	{
		HitShieldTestRobot(Source, Survivor, Row.Shield + Row.MaxHealth - 1.f);
		TestFalse(TEXT("Shield + MaxHealth - 1 does not kill"), Survivor->IsDead());
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
	// ShieldChasers until 1:30, WideShieldChasers (bigger, with shooters) after.
	const FName ShieldTypeNames[] = { TEXT("ShieldChasers"), TEXT("WideShieldChasers") };
	TArray<int32> ShieldTypes;
	for (const FName ShieldTypeName : ShieldTypeNames)
	{
		const int32 ShieldType = Config.FindPackType(ShieldTypeName);
		if (!TestTrue(FString::Printf(TEXT("%s pack type"), *ShieldTypeName.ToString()), ShieldType != INDEX_NONE))
		{
			return false;
		}
		ShieldTypes.Add(ShieldType);

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
		AddInfo(FString::Printf(TEXT("%s pack: %.1f shield chasers of %.1f chasers"), *ShieldTypeName.ToString(), ShieldMean, ChaserMean));
		TestTrue(FString::Printf(TEXT("%s: shield chasers are 25 %% of the pack chasers"), *ShieldTypeName.ToString()),
			ChaserMean > 0.f && FMath::IsNearlyEqual(ShieldMean / ChaserMean, 0.25f, 0.01f));
	}

	// Only the phases from 1:00 draw them (FlowShields first); no other pack type holds a shield chaser.
	int32 FlowIndex = INDEX_NONE;
	int32 ShieldPhaseIndex = INDEX_NONE;
	for (int32 Index = 0; Index < Config.Phases.Num(); ++Index)
	{
		const FRRDirectorPhase& Phase = Config.Phases[Index];
		const bool bDrawsShields = Phase.PackWeights.ContainsByPredicate([&ShieldTypeNames](const FRRDirectorPackWeight& Weight)
			{ return Weight.PackType == ShieldTypeNames[0] || Weight.PackType == ShieldTypeNames[1]; });
		TestEqual(FString::Printf(TEXT("phase %s draws shield chaser packs only from 60 s"), *Phase.Name.ToString()), bDrawsShields, Phase.StartTime >= 60.f);
		FlowIndex = Phase.Name == TEXT("Flow") ? Index : FlowIndex;
		ShieldPhaseIndex = bDrawsShields && ShieldPhaseIndex == INDEX_NONE ? Index : ShieldPhaseIndex;
	}
	for (int32 Type = 0; Type < Config.PackTypes.Num(); ++Type)
	{
		if (ShieldTypes.Contains(Type))
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
