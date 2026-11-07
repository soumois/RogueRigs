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
#include "GameFramework/WorldSettings.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRGameMode.h"
#include "Run/RRRunLostSubsystem.h"
#include "Run/RRRunSettings.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPlayerTests
{
	using namespace RRTestHarness;

	/** Health the tests set on the miner before hitting it (scenario value written on the ASC, not read from PlayerBaseStats.csv). */
	constexpr float TestMaxHealth = 100.f;

	/** Flat damage of one test hit (scenario value, task-009-01: 10 lowers 100 to 90). */
	constexpr float HitDamage = 10.f;

	/** Blueprint of the run game mode. */
	const TCHAR* const GameModeClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRGameMode.BP_RRGameMode_C");

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPlayerGameOverTest, "RogueRigs.Player.GameOver",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPlayerGameOverTest::RunTest(const FString& Parameters)
{
	using namespace RRPlayerTests;

	UClass* GameModeClass = LoadTestClass<ARRGameMode>(*this, GameModeClassPath);
	if (!GameModeClass)
	{
		return false;
	}

	FTestWorld TestWorld(TEXT("RRPlayerGameOver"));
	ARRMiner* Miner = SpawnTestMiner(*this, TestWorld);
	ARRGameMode* GameMode = TestWorld.Spawn<ARRGameMode>(GameModeClass, FVector::ZeroVector);
	if (!Miner || !TestNotNull(TEXT("game mode spawned"), GameMode))
	{
		return false;
	}

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	GameMode->OnGameOver.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleGameOver);
	// No authority game mode in the test world: register as ARRMiner::BeginPlay does in a run.
	GameMode->RegisterMiner(Miner);
	GameMode->RegisterMiner(Miner);

	TestFalse(TEXT("no game over while alive"), GameMode->IsGameOver());

	// Death sequence and run-lost recap (death-screen gdd, task-484): the bare world has no game instance (no run, no run log), so
	// the recap reads level 1 of the settings level count and this level's kills.
	UWorld* World = TestWorld.Get();
	URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>();
	URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(World);
	if (!TestNotNull(TEXT("run flow subsystem"), RunFlow) || !TestNotNull(TEXT("run-lost subsystem"), RunLost))
	{
		return false;
	}
	RunLost->OnRunLost.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleRunLost);
	const TArray<FRRRunLostRecap>& Shown = Recorder->RunLostRecaps;
	constexpr int32 HeldOre = 7;
	constexpr int32 Kills = 3;
	TestTrue(TEXT("ore carried before the death"), RunFlow->AddCarriedOre(HeldOre));
	for (int32 Kill = 0; Kill < Kills; ++Kill)
	{
		RunFlow->NotifyEnemyKilled(nullptr, nullptr);
	}
	const int32 SecuredBefore = RunFlow->GetSecuredOre();

	UAbilitySystemComponent* Enemy = SpawnEnemySource(TestWorld, 1.f);
	ApplyFlatDamage(Enemy, Miner->GetAbilitySystemComponent(), 150.f);
	TestTrue(TEXT("miner dead"), Miner->IsDead());
	TestEqual(TEXT("game over fired once on death (registered twice)"), Recorder->GetGameOverCount(), 1);
	TestTrue(TEXT("IsGameOver"), GameMode->IsGameOver());
	TestTrue(TEXT("death sequence running"), RunLost->IsDeathSequenceRunning());
	const URRRunSettings* Settings = GetDefault<URRRunSettings>();
	TestEqual(TEXT("game slowed to the death time dilation"), World->GetWorldSettings()->TimeDilation, Settings->DeathTimeDilation, 1.e-4f);
	TestEqual(TEXT("carried ore emptied by the death"), RunFlow->GetCarriedOre(), 0);
	TestEqual(TEXT("no screen at the lethal hit"), Shown.Num(), 0);

	// The screen comes after DeathSequenceRealSeconds of real time, the game having run at the dilated speed meanwhile.
	const double StartReal = World->GetRealTimeSeconds();
	const double StartGame = World->GetTimeSeconds();
	int32 ShownFrame = -1;
	for (int32 Frame = 1; Frame <= 240 && ShownFrame < 0; ++Frame)
	{
		TestWorld.Tick();
		// A second death broadcast during the sequence changes nothing.
		if (Frame == 10)
		{
			Miner->OnDeath.Broadcast(nullptr);
		}
		if (Shown.Num() > 0)
		{
			ShownFrame = Frame;
		}
	}
	const double RealSeconds = World->GetRealTimeSeconds() - StartReal;
	const double GameSeconds = World->GetTimeSeconds() - StartGame;
	AddInfo(FString::Printf(TEXT("run-lost screen at frame %d: %.3f s real, %.3f s game time"), ShownFrame, RealSeconds, GameSeconds));
	if (!TestEqual(TEXT("OnRunLost fired once"), Shown.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("game over still fired once"), Recorder->GetGameOverCount(), 1);
	TestEqual(TEXT("screen after the death sequence real time (within one frame)"), RealSeconds, double(Settings->DeathSequenceRealSeconds), double(FrameSeconds) + 1.e-3);
	TestEqual(TEXT("game time ran at the dilated speed"), GameSeconds, double(Settings->DeathSequenceRealSeconds * Settings->DeathTimeDilation), double(FrameSeconds) + 1.e-3);
	TestFalse(TEXT("death sequence over"), RunLost->IsDeathSequenceRunning());
	TestTrue(TEXT("run-lost shown"), RunLost->IsRunLostShown());
	TestEqual(TEXT("time dilation restored"), World->GetWorldSettings()->TimeDilation, 1.f, 1.e-4f);

	const FRRRunLostRecap& Recap = Shown[0];
	TestTrue(TEXT("recap: miner died"), Recap.bMinerDied);
	TestEqual(TEXT("recap: death text id"), Recap.ScreenTextId, FName(TEXT("run_lost_death")));
	TestEqual(TEXT("recap: title from screen_texts.json"), Recap.Title.ToString(), FString(TEXT("CONTRACT TERMINATED")));
	TestFalse(TEXT("recap: subtitle from screen_texts.json"), Recap.Subtitle.IsEmpty());
	TestEqual(TEXT("recap: level 1 without a run"), Recap.LevelReached, 1);
	TestEqual(TEXT("recap: level count from the settings"), Recap.LevelCount, FMath::Max(1, Settings->LevelLayouts.Num()));
	TestEqual(TEXT("recap: robots destroyed"), Recap.RobotsDestroyed, Kills);
	TestEqual(TEXT("recap: Duskite held at death"), Recap.DuskiteHeld, HeldOre + SecuredBefore);
	TestEqual(TEXT("recap: level text"), Recap.LevelText.ToString(), FString::Printf(TEXT("Level 1 of %d"), Recap.LevelCount));

	// Nothing more: no second screen, no travel (the old 3 s restart is gone).
	FWorldContext* Context = GEngine->GetWorldContextFromWorld(World);
	for (int32 Frame = 0; Frame < 300; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("still one OnRunLost"), Shown.Num(), 1);
	TestTrue(TEXT("no restart travel after 5 s"), !Context || Context->TravelURL.IsEmpty());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
