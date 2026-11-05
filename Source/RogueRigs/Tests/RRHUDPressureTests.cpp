// Rogue Rigs - automation tests of the HUD pressure events (RogueRigs.HUD.Pressure*): damage vignette, horde banner, alert-bar icons and their position rule.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRWaveDirector.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestHUDWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRHUDPressureTests
{
	/** Miner cell on arena_m1_fine (open floor), as in the spawner tests. */
	const FIntPoint PressureMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Position tolerance on the bar (a few frames of base fill). */
	constexpr float PressureBarTolerance = 0.005f;

	/** World pieces of a pressure test. */
	struct FPressureSetup
	{
		ARRMiner* Miner = nullptr;
		URRRunFlowSubsystem* RunFlow = nullptr;
		URRTestHUDWidget* HUD = nullptr;
	};

	/** Fine grid, a miner with its weapons stopped, the run flow on Config, and a recording HUD widget (not bound yet). */
	bool MakePressureSetup(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, const FRRRunFlowConfig& Config, FPressureSetup& Out)
	{
		const ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		Out.Miner = Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Grid, PressureMinerCell) : nullptr;
		Out.RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("run flow subsystem"), Out.RunFlow))
		{
			return false;
		}
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		Out.RunFlow->SetConfig(Config);
		Out.HUD = CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass());
		return Test.TestNotNull(TEXT("test HUD created"), Out.HUD);
	}

	/** First icon of Type (and Index for a horde) in Events; null when absent. */
	const FRRAlertBarEvent* FindPressureIcon(const TArray<FRRAlertBarEvent>& Events, ERRAlertBarEventType Type, int32 Index = INDEX_NONE)
	{
		return Events.FindByPredicate([Type, Index](const FRRAlertBarEvent& Event) { return Event.Type == Type && (Index == INDEX_NONE || Event.Index == Index); });
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDPressureBarPositionTest, "RogueRigs.HUD.PressureBarPosition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDPressureBarPositionTest::RunTest(const FString& Parameters)
{
	// Base rate: 240 s fill, 60 s ahead = a quarter of the bar ahead of the fill.
	TestNearlyEqual(TEXT("60 s ahead at alert 0.1"), URRHUDWidget::EstimateBarPosition(0.1f, 60.f, 240.f), 0.35f, KINDA_SMALL_NUMBER);
	TestNearlyEqual(TEXT("clamped to the end of the bar"), URRHUDWidget::EstimateBarPosition(0.9f, 60.f, 240.f), 1.f, KINDA_SMALL_NUMBER);
	TestNearlyEqual(TEXT("an overdue event sits on the fill"), URRHUDWidget::EstimateBarPosition(0.5f, -3.f, 240.f), 0.5f, KINDA_SMALL_NUMBER);

	// Constant along the base fill (no jump); kills (faster fill) push it forward; a pause slides it back, never behind the fill.
	const float Front = 95.f;
	const float AtStart = URRHUDWidget::EstimateBarPosition(0.f, Front, 240.f);
	const float AtForty = URRHUDWidget::EstimateBarPosition(40.f / 240.f, Front - 40.f, 240.f);
	TestNearlyEqual(TEXT("same estimate along the base fill"), AtForty, AtStart, KINDA_SMALL_NUMBER);
	const float AfterKills = URRHUDWidget::EstimateBarPosition(40.f / 240.f + 0.05f, Front - 40.f, 240.f);
	TestTrue(TEXT("kills move the icon forward"), AfterKills > AtForty);
	float Previous = 1.f;
	bool bNeverBehind = true;
	bool bNeverForward = true;
	for (float Paused = 0.f; Paused <= 60.f; Paused += 5.f)
	{
		// Console pause at 0.4: run time goes on, the fill does not.
		const float Position = URRHUDWidget::EstimateBarPosition(0.4f, 30.f - Paused, 240.f);
		bNeverBehind &= Position >= 0.4f;
		bNeverForward &= Position <= Previous;
		Previous = Position;
	}
	TestTrue(TEXT("paused: never behind the fill"), bNeverBehind);
	TestTrue(TEXT("paused: slides back toward the fill"), bNeverForward);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDPressureDamageTest, "RogueRigs.HUD.PressureDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDPressureDamageTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDPressureTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDPressureDamage"));
	FPressureSetup Setup;
	if (!MakePressureSetup(*this, TestWorld, FRRRunFlowConfig(), Setup))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(Setup.HUD);
	UAbilitySystemComponent* ASC = Setup.Miner->GetAbilitySystemComponent();
	if (!TestNotNull(TEXT("miner ASC"), ASC))
	{
		return false;
	}
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 100.f);
	Setup.HUD->BindToMiner(Setup.Miner);
	TestEqual(TEXT("no damage on binding"), Setup.HUD->DamageCalls.Num(), 0);

	// Two hits: amount and fraction left; the vignette grows as the fraction falls.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 70.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 20.f);
	if (TestEqual(TEXT("two damage calls"), Setup.HUD->DamageCalls.Num(), 2))
	{
		TestNearlyEqual(TEXT("first hit 30"), Setup.HUD->DamageCalls[0].Amount, 30.f, KINDA_SMALL_NUMBER);
		TestNearlyEqual(TEXT("first fraction 0.7"), Setup.HUD->DamageCalls[0].HealthFraction, 0.7f, KINDA_SMALL_NUMBER);
		TestNearlyEqual(TEXT("second hit 50"), Setup.HUD->DamageCalls[1].Amount, 50.f, KINDA_SMALL_NUMBER);
		TestNearlyEqual(TEXT("second fraction 0.2"), Setup.HUD->DamageCalls[1].HealthFraction, 0.2f, KINDA_SMALL_NUMBER);
	}

	// A heal and a MaxHealth decrease that clamps Health are not hits.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 90.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 60.f);
	TestNearlyEqual(TEXT("MaxHealth decrease clamped Health"), ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()), 60.f, KINDA_SMALL_NUMBER);
	TestEqual(TEXT("heal and clamp are not damage"), Setup.HUD->DamageCalls.Num(), 2);

	// Lethal hit: fraction 0.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 0.f);
	if (TestEqual(TEXT("lethal hit recorded"), Setup.HUD->DamageCalls.Num(), 3))
	{
		TestNearlyEqual(TEXT("lethal fraction 0"), Setup.HUD->DamageCalls[2].HealthFraction, 0.f, KINDA_SMALL_NUMBER);
	}

	// Unbound: nothing more.
	Setup.HUD->BindToMiner(nullptr);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 50.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 10.f);
	TestEqual(TEXT("no call once unbound"), Setup.HUD->DamageCalls.Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDPressureHordeTest, "RogueRigs.HUD.PressureHorde",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDPressureHordeTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDPressureTests;
	using namespace RRTestAccess;

	// Alert fills in 20 s (console at 0.4 = 8 s, never reached); one horde, lull 1 s, banner 1.5 s, front 2 s, spread 1 s.
	FRRRunFlowConfig RunConfig;
	RunConfig.AlertFillSeconds = 20.f;
	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDPressureHorde"));
	FPressureSetup Setup;
	if (!MakePressureSetup(*this, TestWorld, RunConfig, Setup))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(Setup.HUD);
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	const FVector MinerLocation = Setup.Miner->GetActorLocation();
	const FVector2D Center(MinerLocation.X, MinerLocation.Y);
	Spawner->SetViewRectangleOverride(FBox2D(Center - FVector2D(1600.f, 1000.f), Center + FVector2D(1600.f, 1000.f)));
	Spawner->SeedRandomStream(20261005);
	FRRWaveDirectorConfig& Director = FSpawnerAccess::DirectorOf(Spawner);
	Director = FRRWaveDirectorConfig::MakeDefault();
	Director.Phases.Reset();
	Director.Hordes.SetNum(1);
	Director.Hordes[0].TriggerTime = 2.f;
	Director.Hordes[0].TriggerAlert = 0.3f;
	Director.Hordes[0].LullSeconds = 1.f;
	Director.Hordes[0].WarningSeconds = 0.5f;
	Director.Hordes[0].SpreadSeconds = 1.f;

	// Alert trigger: the horde icon sits at its trigger alert.
	Director.HordeTrigger = ERRHordeTrigger::Alert;
	Setup.HUD->BindToRunFlow(Setup.RunFlow);
	Setup.HUD->BindToSpawner(Spawner);
	const FRRAlertBarEvent* AlertHorde = FindPressureIcon(Setup.HUD->GetAlertBarEvents(), ERRAlertBarEventType::Horde, 0);
	TestTrue(TEXT("alert trigger: icon at 0.3"), AlertHorde && FMath::IsNearlyEqual(AlertHorde->Position, 0.3f, KINDA_SMALL_NUMBER));

	// Run-time trigger: console, bonus chest, horde, shuttle; the horde 2 s / 20 s ahead of the empty bar.
	Director.HordeTrigger = ERRHordeTrigger::RunTime;
	Setup.HUD->BindToSpawner(Spawner);
	const TArray<FRRAlertBarEvent> Initial = Setup.HUD->GetAlertBarEvents();
	TestEqual(TEXT("four icons"), Initial.Num(), 4);
	if (Initial.Num() == 4)
	{
		TestTrue(TEXT("order console, chest, horde, shuttle"), Initial[0].Type == ERRAlertBarEventType::Console && Initial[1].Type == ERRAlertBarEventType::BonusChest
			&& Initial[2].Type == ERRAlertBarEventType::Horde && Initial[3].Type == ERRAlertBarEventType::Extraction);
		TestNearlyEqual(TEXT("console at the threshold"), Initial[0].Position, RunConfig.ConsoleAlertThreshold, KINDA_SMALL_NUMBER);
		TestNearlyEqual(TEXT("chest at its threshold"), Initial[1].Position, RunConfig.BonusChestAlertThreshold, KINDA_SMALL_NUMBER);
		TestNearlyEqual(TEXT("horde estimate 0.1"), Initial[2].Position, 0.1f, PressureBarTolerance);
		TestEqual(TEXT("horde index 0"), Initial[2].Index, 0);
		TestNearlyEqual(TEXT("shuttle at the end"), Initial[3].Position, 1.f, KINDA_SMALL_NUMBER);
		TestTrue(TEXT("nothing done yet"), !Initial[0].bDone && !Initial[1].bDone && !Initial[2].bDone && !Initial[3].bDone);
	}
	// Run flow bind (console, shuttle), spawner bind (alert trigger), rebind (run time): one call each.
	TestEqual(TEXT("one call per changed list"), Setup.HUD->AlertEventCalls.Num(), 3);
	const int32 CallsAfterBind = Setup.HUD->AlertEventCalls.Num();

	// Run 200 frames (front at 2 s, over at 3 s): the pending icon stays ahead of the fill, then freezes done.
	Spawner->SetSpawningEnabled(true);
	bool bAheadWhilePending = true;
	float MinPending = 1.f;
	float MaxPending = 0.f;
	int32 DoneFrame = INDEX_NONE;
	float DonePosition = -1.f;
	bool bFrozen = true;
	for (int32 Frame = 1; Frame <= 200; ++Frame)
	{
		TestWorld.Tick();
		const FRRAlertBarEvent* Horde = FindPressureIcon(Setup.HUD->GetAlertBarEvents(), ERRAlertBarEventType::Horde, 0);
		if (!Horde)
		{
			continue;
		}
		if (!Horde->bDone)
		{
			bAheadWhilePending &= Horde->Position + PressureBarTolerance >= Setup.RunFlow->GetAlert();
			MinPending = FMath::Min(MinPending, Horde->Position);
			MaxPending = FMath::Max(MaxPending, Horde->Position);
		}
		else if (DoneFrame == INDEX_NONE)
		{
			DoneFrame = Frame;
			DonePosition = Horde->Position;
		}
		else
		{
			bFrozen &= Horde->Position == DonePosition;
		}
	}
	AddInfo(FString::Printf(TEXT("pending %.4f-%.4f, done at frame %d at %.4f, alert %.4f, %d icon calls"), MinPending, MaxPending, DoneFrame, DonePosition,
		Setup.RunFlow->GetAlert(), Setup.HUD->AlertEventCalls.Num()));
	TestTrue(TEXT("pending icon never behind the fill"), bAheadWhilePending);
	TestTrue(TEXT("pending icon steady along the base fill"), MaxPending - MinPending <= PressureBarTolerance);
	TestTrue(*FString::Printf(TEXT("done when the front starts (frame %d)"), DoneFrame), FMath::Abs(DoneFrame - 120) <= 1);
	TestNearlyEqual(TEXT("done icon where the fill was at the front"), DonePosition, 0.1f, PressureBarTolerance);
	TestTrue(TEXT("done icon frozen"), bFrozen);
	TestEqual(TEXT("only the done flag called it again"), Setup.HUD->AlertEventCalls.Num(), CallsAfterBind + 1);

	// Banner: once, 0.5 s before the front.
	if (TestEqual(TEXT("one banner"), Setup.HUD->HordeIncomingCalls.Num(), 1))
	{
		TestNearlyEqual(TEXT("banner 0.5 s before the front"), Setup.HUD->HordeIncomingCalls[0], 0.5f, 2.f / 60.f);
	}

	// Unbound: no more banner or icon calls from the spawner.
	Setup.HUD->BindToSpawner(nullptr);
	Setup.HUD->BindToRunFlow(nullptr);
	const int32 CallsBefore = Setup.HUD->AlertEventCalls.Num();
	TestWorld.Tick();
	TestEqual(TEXT("no call once unbound"), Setup.HUD->AlertEventCalls.Num(), CallsBefore);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDPressureKillsWaitFirstHordeTest, "RogueRigs.HUD.PressureKillsWaitFirstHorde",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDPressureKillsWaitFirstHordeTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDPressureTests;
	using namespace RRTestAccess;

	// Alert fills in 20 s (console at 8 s, never reached); run-time hordes: front 0 at 2 s spread 1 s (over at 3 s), front 1 at 6 s.
	FRRRunFlowConfig RunConfig;
	RunConfig.AlertFillSeconds = 20.f;
	TestTrue(TEXT("kills wait for the first horde by default"), RunConfig.bKillsWaitForFirstHorde);
	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDPressureKillsWaitFirstHorde"));
	FPressureSetup Setup;
	if (!MakePressureSetup(*this, TestWorld, RunConfig, Setup))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(Setup.HUD);
	URRRunFlowSubsystem* RunFlow = Setup.RunFlow;

	// No spawner yet: kills add alert as before.
	TestTrue(TEXT("no spawner: kills add alert"), RunFlow->DoKillsAddAlert());

	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	const FVector MinerLocation = Setup.Miner->GetActorLocation();
	const FVector2D Center(MinerLocation.X, MinerLocation.Y);
	Spawner->SetViewRectangleOverride(FBox2D(Center - FVector2D(1600.f, 1000.f), Center + FVector2D(1600.f, 1000.f)));
	Spawner->SeedRandomStream(20261105);
	FRRWaveDirectorConfig& Director = FSpawnerAccess::DirectorOf(Spawner);
	Director = FRRWaveDirectorConfig::MakeDefault();
	Director.Phases.Reset();
	Director.HordeTrigger = ERRHordeTrigger::RunTime;
	Director.Hordes.SetNum(2);
	Director.Hordes[0].TriggerTime = 2.f;
	Director.Hordes[0].LullSeconds = 1.f;
	Director.Hordes[0].WarningSeconds = 0.5f;
	Director.Hordes[0].SpreadSeconds = 1.f;
	Director.Hordes[1] = Director.Hordes[0];
	Director.Hordes[1].TriggerTime = 6.f;
	RunFlow->RegisterSpawner(Spawner);
	if (!TestTrue(TEXT("spawner registered with the run flow"), RunFlow->GetSpawner() == Spawner))
	{
		return false;
	}
	Setup.HUD->BindToRunFlow(RunFlow);
	Setup.HUD->BindToSpawner(Spawner);
	TestFalse(TEXT("first horde pending: kills add no alert"), RunFlow->DoKillsAddAlert());

	const auto HordePosition = [&Setup](int32 Index)
	{
		const FRRAlertBarEvent* Icon = FindPressureIcon(Setup.HUD->GetAlertBarEvents(), ERRAlertBarEventType::Horde, Index);
		return Icon ? Icon->Position : -1.f;
	};
	const float Start0 = HordePosition(0);
	const float Start1 = HordePosition(1);
	TestNearlyEqual(TEXT("horde 0 mark at 2 s / 20 s"), Start0, 0.1f, PressureBarTolerance);
	TestNearlyEqual(TEXT("horde 1 mark at 6 s / 20 s"), Start1, 0.3f, PressureBarTolerance);

	// Until the first front is over: a kill every 10 frames leaves the bar on time alone and both marks in place.
	Spawner->SetSpawningEnabled(true);
	bool bTimeAlone = true;
	float Min0 = 1.f, Max0 = 0.f, Min1 = 1.f, Max1 = 0.f;
	int32 KillsBefore = 0;
	int32 OverFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 300 && OverFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (Spawner->GetHordeState(0) == ERRHordeState::Done)
		{
			OverFrame = Frame;
			break;
		}
		if (Frame % 10 == 0)
		{
			const float AlertBefore = RunFlow->GetAlertSeconds();
			RunFlow->NotifyEnemyKilled(nullptr, nullptr);
			++KillsBefore;
			bTimeAlone &= RunFlow->GetAlertSeconds() == AlertBefore;
		}
		bTimeAlone &= FMath::IsNearlyEqual(RunFlow->GetAlertSeconds(), Frame / 60.f, 1e-3f);
		if (Spawner->GetHordeState(0) != ERRHordeState::Spawning)
		{
			Min0 = FMath::Min(Min0, HordePosition(0));
			Max0 = FMath::Max(Max0, HordePosition(0));
		}
		Min1 = FMath::Min(Min1, HordePosition(1));
		Max1 = FMath::Max(Max1, HordePosition(1));
	}
	const float AlertAtOver = RunFlow->GetAlertSeconds();
	AddInfo(FString::Printf(TEXT("before the end of front 0: %d kills, %d counted, alert %.3f s at frame %d; horde 0 mark %.4f-%.4f, horde 1 mark %.4f-%.4f"),
		KillsBefore, RunFlow->GetKillCount(), AlertAtOver, OverFrame, Min0, Max0, Min1, Max1));
	TestTrue(*FString::Printf(TEXT("first front over around 3 s (frame %d)"), OverFrame), OverFrame != INDEX_NONE && FMath::Abs(OverFrame - 180) <= 2);
	TestTrue(TEXT("kills before the end of the first front: bar on time alone"), bTimeAlone);
	TestEqual(TEXT("every kill still counted (threat)"), RunFlow->GetKillCount(), KillsBefore);
	TestTrue(TEXT("horde 0 mark still while pending"), Max0 - Min0 <= PressureBarTolerance);
	TestTrue(TEXT("horde 1 mark still until the first front is over"), Max1 - Min1 <= PressureBarTolerance);

	// After: kills add alert again and move the second mark forward.
	TestTrue(TEXT("first front over: kills add alert"), RunFlow->DoKillsAddAlert());
	const float Mark1Before = HordePosition(1);
	for (int32 Kill = 0; Kill < 4; ++Kill)
	{
		RunFlow->NotifyEnemyKilled(nullptr, nullptr);
	}
	TestNearlyEqual(TEXT("4 kills after the front add 2 s"), RunFlow->GetAlertSeconds() - AlertAtOver, 4.f * RunConfig.KillAlertSeconds, 1e-4f);
	TestWorld.Tick();
	const float Mark1After = HordePosition(1);
	AddInfo(FString::Printf(TEXT("after the front: alert %.3f s, horde 1 mark %.4f -> %.4f"), RunFlow->GetAlertSeconds(), Mark1Before, Mark1After));
	TestNearlyEqual(TEXT("horde 1 mark moved forward by the kill time"), Mark1After - Mark1Before, 2.f / RunConfig.AlertFillSeconds, PressureBarTolerance);

	// The switch off: kills add alert even before the first front.
	FRRRunFlowConfig OffConfig = RunConfig;
	OffConfig.bKillsWaitForFirstHorde = false;
	RunFlow->SetConfig(OffConfig);
	TestTrue(TEXT("switch off: kills add alert"), RunFlow->DoKillsAddAlert());

	Setup.HUD->BindToSpawner(nullptr);
	Setup.HUD->BindToRunFlow(nullptr);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
