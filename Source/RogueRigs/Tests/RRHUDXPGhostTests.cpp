// Rogue Rigs - automation test of the XP bar gain ghost (RogueRigs.HUD.XPGhost): ghost and fill values after a gain, after a level-up and after several levels at once.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestHUDWidget.h"
#include "UObject/StrongObjectPtr.h"

namespace RRHUDXPGhostTests
{
	/** Miner cell on arena_m1_fine (open floor), as in the HUD event tests. */
	const FIntPoint GhostMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Advances the bar in 1/60 s frames for Seconds. */
	void Run(URRXPStatusWidget* Widget, float Seconds)
	{
		const float Frame = 1.f / 60.f;
		for (float Elapsed = 0.f; Elapsed < Seconds; Elapsed += Frame)
		{
			Widget->AdvanceXPBar(Frame);
		}
	}

	/** XP fraction of the progression's current level. */
	float CurrentFraction(const URRProgressionComponent* Progression)
	{
		const float Needed = Progression->GetXPToNextLevel();
		return Needed > 0.f ? FMath::Clamp(Progression->GetXP() / Needed, 0.f, 1.f) : 0.f;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDXPGhostTest, "RogueRigs.HUD.XPGhost",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDXPGhostTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDXPGhostTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDXPGhost"));
	const ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(*this, TestWorld, Grid, GhostMinerCell) : nullptr;
	URRProgressionComponent* Progression = Miner ? Miner->GetProgression() : nullptr;
	URRTestXPStatusWidget* Widget = CreateWidget<URRTestXPStatusWidget>(TestWorld.Get(), URRTestXPStatusWidget::StaticClass());
	if (!TestNotNull(TEXT("progression"), Progression) || !TestNotNull(TEXT("widget"), Widget))
	{
		return false;
	}
	TStrongObjectPtr<URRTestXPStatusWidget> WidgetRef(Widget);
	Widget->SetMiner(Miner);
	const float Start = CurrentFraction(Progression);
	TestNearlyEqual(TEXT("bind: fill shows the XP at once"), Widget->GetFillFraction(), Start, 1e-4f);
	TestNearlyEqual(TEXT("bind: no ghost ahead"), Widget->GetGhostFraction(), Start, 1e-4f);
	TestTrue(TEXT("bind: settled"), Widget->IsXPBarSettled());

	// Gain inside the level: the ghost shows the new XP at once, the fill waits, then catches up.
	const int32 Level = Progression->GetLevel();
	Progression->AddXP(Progression->GetXPToNextLevel() * 0.3f);
	if (!TestEqual(TEXT("gain stays in the level"), Progression->GetLevel(), Level))
	{
		return false;
	}
	const float AfterGain = CurrentFraction(Progression);
	TestTrue(TEXT("gain moves the fraction"), AfterGain > Start + 0.1f);
	TestNearlyEqual(TEXT("gain: ghost at the new XP at once"), Widget->GetGhostFraction(), AfterGain, 1e-4f);
	TestNearlyEqual(TEXT("gain: fill unchanged at once"), Widget->GetFillFraction(), Start, 1e-4f);
	TestTrue(TEXT("gain: per-frame update armed"), Widget->IsXPBarUpdateRunning());
	Run(Widget, 0.3f);
	TestNearlyEqual(TEXT("gain: fill still waits at 0.3 s"), Widget->GetFillFraction(), Start, 1e-4f);

	// A new gain restarts the wait.
	Progression->AddXP(Progression->GetXPToNextLevel() * 0.1f);
	const float AfterSecond = CurrentFraction(Progression);
	TestNearlyEqual(TEXT("second gain: ghost at the new XP"), Widget->GetGhostFraction(), AfterSecond, 1e-4f);
	Run(Widget, 0.3f);
	TestNearlyEqual(TEXT("second gain restarts the wait"), Widget->GetFillFraction(), Start, 1e-4f);
	Run(Widget, 0.2f);
	TestTrue(TEXT("fill catches up after the wait"), Widget->GetFillFraction() > Start);
	Run(Widget, 2.f);
	TestNearlyEqual(TEXT("fill reaches the ghost"), Widget->GetFillFraction(), AfterSecond, 1e-4f);
	TestTrue(TEXT("settled after the catch-up"), Widget->IsXPBarSettled());

	// Level-up: the ghost goes to the end, the fill reaches it, the bar empties, then the remainder shows the same way.
	const float Before = Widget->GetFillFraction();
	Progression->AddXP((Progression->GetXPToNextLevel() - Progression->GetXP()) + Progression->GetXPToNextLevelAt(Level + 1) * 0.25f);
	if (!TestEqual(TEXT("one level gained"), Progression->GetLevel(), Level + 1))
	{
		return false;
	}
	const float Remainder = CurrentFraction(Progression);
	TestTrue(TEXT("level-up: pass pending"), Widget->IsLevelUpPassPending());
	TestNearlyEqual(TEXT("level-up: ghost at the end"), Widget->GetGhostFraction(), 1.f, 1e-4f);
	TestNearlyEqual(TEXT("level-up: fill unchanged at once"), Widget->GetFillFraction(), Before, 1e-4f);
	for (int32 Frame = 0; Frame < 600 && Widget->IsLevelUpPassPending(); ++Frame)
	{
		Widget->AdvanceXPBar(1.f / 60.f);
		TestTrue(TEXT("level-up: fill never above the ghost"), Widget->GetFillFraction() <= Widget->GetGhostFraction());
	}
	const bool bSawEmptied = !Widget->IsLevelUpPassPending() && Widget->GetFillFraction() == 0.f;
	TestTrue(TEXT("level-up: the bar emptied"), bSawEmptied);
	TestNearlyEqual(TEXT("level-up: ghost shows the remainder"), Widget->GetGhostFraction(), Remainder, 1e-4f);
	Run(Widget, 0.3f);
	TestNearlyEqual(TEXT("level-up: fill waits on the remainder"), Widget->GetFillFraction(), 0.f, 1e-4f);
	Run(Widget, 2.f);
	TestNearlyEqual(TEXT("level-up: fill reaches the remainder"), Widget->GetFillFraction(), Remainder, 1e-4f);
	TestTrue(TEXT("level-up: settled"), Widget->IsXPBarSettled());

	// Several levels at once: one pass to the end, then the remainder; never stuck.
	const int32 LevelBefore = Progression->GetLevel();
	float Big = Progression->GetXPToNextLevel() - Progression->GetXP();
	Big += Progression->GetXPToNextLevelAt(LevelBefore + 1) + Progression->GetXPToNextLevelAt(LevelBefore + 2) * 0.5f;
	Progression->AddXP(Big);
	TestTrue(TEXT("several levels gained"), Progression->GetLevel() >= LevelBefore + 2);
	const float MultiRemainder = CurrentFraction(Progression);
	TestNearlyEqual(TEXT("multi: ghost at the end"), Widget->GetGhostFraction(), 1.f, 1e-4f);
	Run(Widget, 5.f);
	TestFalse(TEXT("multi: pass over"), Widget->IsLevelUpPassPending());
	TestNearlyEqual(TEXT("multi: ghost on the remainder"), Widget->GetGhostFraction(), MultiRemainder, 1e-4f);
	TestNearlyEqual(TEXT("multi: fill on the remainder"), Widget->GetFillFraction(), MultiRemainder, 1e-4f);
	TestTrue(TEXT("multi: settled"), Widget->IsXPBarSettled());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
