// Rogue Rigs - automation tests of the level-up flow (RogueRigs.LevelUp.*): heal, choice delay, chained screens, resume.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRLevelUpTests
{
	using namespace RRTestHarness;

	/** What the setup returns. */
	struct FLevelUpSetup
	{
		/** The spawned BP_Miner. */
		ARRMiner* Miner = nullptr;
		/** Its progression. */
		URRProgressionComponent* Progression = nullptr;
		/** Its level-up flow, bound to the recorder. */
		URRLevelUpComponent* LevelUp = nullptr;
		/** Its upgrade component (seeded). */
		URRUpgradeComponent* Upgrades = nullptr;
	};

	/** Spawns the grid and BP_Miner (weapons stopped), checks the flow defaults and binds the level-up events to Recorder. */
	bool SpawnLevelUpSetup(FAutomationTestBase& Test, const FTestWorld& TestWorld, URRTestEventRecorder* Recorder, FLevelUpSetup& Out)
	{
		ARRArenaGrid* Grid = SpawnGrid(Test, TestWorld);
		Out.Miner = Grid ? SpawnMiner(Test, TestWorld, Grid, RockWestFloorCell, M1TestLoadout) : nullptr;
		if (!Out.Miner)
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
		Out.Progression = Out.Miner->GetProgression();
		Out.LevelUp = Out.Miner->GetLevelUp();
		Out.Upgrades = Out.Miner->GetUpgrades();
		if (!Test.TestNotNull(TEXT("miner has a progression"), Out.Progression)
			|| !Test.TestNotNull(TEXT("miner has a level-up flow"), Out.LevelUp)
			|| !Test.TestNotNull(TEXT("miner has an upgrade component"), Out.Upgrades))
		{
			return false;
		}
		Out.Upgrades->SetRandomSeed(20261003);
		Test.TestEqual(TEXT("choice delay 0.8 s"), Out.LevelUp->GetChoiceDelay(), 0.8f);
		Test.TestEqual(TEXT("heal fraction 20 %"), Out.LevelUp->GetHealFraction(), 0.2f);
		Test.TestFalse(TEXT("no choice at start"), Out.LevelUp->IsAwaitingChoice() || Out.LevelUp->IsChoiceDelayActive());
		Out.LevelUp->OnLevelUpEffect.AddDynamic(Recorder, &URRTestEventRecorder::HandleLevelUpEffect);
		Out.LevelUp->OnUpgradeOfferShown.AddDynamic(Recorder, &URRTestEventRecorder::HandleUpgradeOfferShown);
		Out.LevelUp->OnUpgradeOfferClosed.AddDynamic(Recorder, &URRTestEventRecorder::HandleUpgradeOfferClosed);
		return true;
	}

	/** Ticks the world Frames fixed frames. */
	void TickFrames(const FTestWorld& TestWorld, int32 Frames)
	{
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
		}
	}

	/** Current value of Attribute on the miner's ASC. */
	float MinerAttr(const ARRMiner* Miner, const FGameplayAttribute& Attribute)
	{
		const UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
		return ASC ? ASC->GetNumericAttribute(Attribute) : -1.f;
	}

	/**
	 * Raw XP that brings Progression exactly Levels levels up from its current XP, plus Extra XP carried, whatever the
	 * XPCurve.csv values and the XPGain attribute (task-028-03: the tests no longer assume a 5 x level curve).
	 */
	float LevelUpTestRawXPFor(const URRProgressionComponent* Progression, int32 Levels, float Extra = 0.f)
	{
		float Needed = Extra - Progression->GetXP();
		for (int32 Index = 0; Index < Levels; ++Index)
		{
			Needed += Progression->GetXPToNextLevelAt(Progression->GetLevel() + Index);
		}
		const UAbilitySystemComponent* ASC = Progression->GetOwner() ? Progression->GetOwner()->FindComponentByClass<UAbilitySystemComponent>() : nullptr;
		const float Gain = ASC && ASC->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetXPGainAttribute())
			? ASC->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute()) : 1.f;
		return Needed / FMath::Max(Gain, KINDA_SMALL_NUMBER);
	}

	/** Frames of the 0.8 s delay at 1/60 s: 47 frames (0.783 s) are before it, 50 frames (0.833 s) after it. */
	constexpr int32 FramesBeforeDelay = 47;
	constexpr int32 FramesAfterDelay = 50;
}

using namespace RRLevelUpTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpHealTest, "RogueRigs.LevelUp.HealClampedToMax",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLevelUpHealTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLevelUpSetup S;
	if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
	{
		return false;
	}

	const FGameplayAttribute Health = URRHealthAttributeSet::GetHealthAttribute();
	const FGameplayAttribute MaxHealth = URRHealthAttributeSet::GetMaxHealthAttribute();
	UAbilitySystemComponent* ASC = S.Miner->GetAbilitySystemComponent();
	TestEqual(TEXT("max health 100"), MinerAttr(S.Miner, MaxHealth), 100.f, 1e-4f);

	// Wounded: 50 + 20 % of 100.
	ASC->SetNumericAttributeBase(Health, 50.f);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("level 2"), S.Progression->GetLevel(), 2);
	TestEqual(TEXT("level-up heals 20 % of max: 50 -> 70"), MinerAttr(S.Miner, Health), 70.f, 1e-4f);
	TestEqual(TEXT("one effect event"), Recorder->GetLevelUpEffects().Num(), 1);
	if (Recorder->GetLevelUpEffects().Num() == 1)
	{
		TestEqual(TEXT("effect for level 2"), Recorder->GetLevelUpEffects()[0], 2);
	}
	TestTrue(TEXT("choice delay running after the level-up"), S.LevelUp->IsChoiceDelayActive());
	TestFalse(TEXT("no choice screen yet"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("no offer yet"), Recorder->GetOfferCardCounts().Num(), 0);

	// Near full: clamped to max.
	ASC->SetNumericAttributeBase(Health, 95.f);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("level 3"), S.Progression->GetLevel(), 3);
	TestEqual(TEXT("95 + 20 clamped to 100"), MinerAttr(S.Miner, Health), 100.f, 1e-4f);
	TestEqual(TEXT("max health unchanged"), MinerAttr(S.Miner, MaxHealth), 100.f, 1e-4f);

	// Two levels in one call: two heals.
	ASC->SetNumericAttributeBase(Health, 40.f);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 2));
	TestEqual(TEXT("level 5"), S.Progression->GetLevel(), 5);
	TestEqual(TEXT("two heals: 40 + 2 x 20"), MinerAttr(S.Miner, Health), 80.f, 1e-4f);
	TestEqual(TEXT("four effect events"), Recorder->GetLevelUpEffects().Num(), 4);
	TestEqual(TEXT("four choices owed"), S.LevelUp->GetPendingChoices(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpDelayTest, "RogueRigs.LevelUp.PauseAfterDelay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLevelUpDelayTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLevelUpSetup S;
	if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
	{
		return false;
	}

	const float StartTime = TestWorld.Get()->GetTimeSeconds();
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestFalse(TEXT("not paused at the level-up"), S.LevelUp->IsAwaitingChoice());

	TickFrames(TestWorld, FramesBeforeDelay);
	TestTrue(TEXT("game time ran during the delay (0.78 s)"), TestWorld.Get()->GetTimeSeconds() - StartTime > 0.78f);
	TestFalse(TEXT("not paused before 0.8 s"), S.LevelUp->IsAwaitingChoice());
	TestTrue(TEXT("delay still running at 0.78 s"), S.LevelUp->IsChoiceDelayActive());
	TestEqual(TEXT("no offer before 0.8 s"), Recorder->GetOfferCardCounts().Num(), 0);
	TestFalse(TEXT("no upgrade offer built yet"), S.Upgrades->HasOffer());

	TickFrames(TestWorld, FramesAfterDelay - FramesBeforeDelay);
	TestTrue(TEXT("paused on the choice after 0.8 s"), S.LevelUp->IsAwaitingChoice());
	TestFalse(TEXT("delay over"), S.LevelUp->IsChoiceDelayActive());
	TestEqual(TEXT("one offer shown"), Recorder->GetOfferCardCounts().Num(), 1);
	if (Recorder->GetOfferCardCounts().Num() == 1)
	{
		TestEqual(TEXT("3 cards"), Recorder->GetOfferCardCounts()[0], 3);
		TestTrue(TEXT("reroll available"), Recorder->GetOfferRerollFlags()[0]);
	}
	TestEqual(TEXT("offer cards = upgrade component offer"), S.LevelUp->GetOfferCards().Num(), 3);

	// Reroll forwards to the offer builder, once.
	TestFalse(TEXT("out-of-range pick refused"), S.LevelUp->ChooseUpgrade(3));
	TestTrue(TEXT("reroll accepted"), S.LevelUp->RequestReroll());
	TestEqual(TEXT("offer shown again after the reroll"), Recorder->GetOfferCardCounts().Num(), 2);
	if (Recorder->GetOfferRerollFlags().Num() == 2)
	{
		TestFalse(TEXT("no second reroll offered"), Recorder->GetOfferRerollFlags()[1]);
	}
	TestFalse(TEXT("second reroll refused"), S.LevelUp->RequestReroll());
	TestTrue(TEXT("still awaiting the choice"), S.LevelUp->IsAwaitingChoice());

	// Paused game: further ticks do not open anything new.
	TickFrames(TestWorld, 10);
	TestEqual(TEXT("still one screen"), Recorder->GetOfferCardCounts().Num(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpChainTest, "RogueRigs.LevelUp.ChainedOffers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLevelUpChainTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLevelUpSetup S;
	if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
	{
		return false;
	}

	// Two levels at once: one delay, then two screens in sequence.
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 2, 2.f));
	TestEqual(TEXT("level 3"), S.Progression->GetLevel(), 3);
	TestEqual(TEXT("two choices owed"), S.LevelUp->GetPendingChoices(), 2);
	TickFrames(TestWorld, FramesAfterDelay);
	TestTrue(TEXT("first screen open"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("one offer shown"), Recorder->GetOfferCardCounts().Num(), 1);

	TestTrue(TEXT("first pick accepted"), S.LevelUp->ChooseUpgrade(0));
	TestEqual(TEXT("one upgrade picked"), S.Upgrades->GetPickedUpgrades().Num(), 1);
	TestTrue(TEXT("second screen open at once (still paused)"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("second offer shown"), Recorder->GetOfferCardCounts().Num(), 2);
	if (Recorder->GetOfferRerollFlags().Num() == 2)
	{
		TestTrue(TEXT("second screen has its own reroll"), Recorder->GetOfferRerollFlags()[1]);
	}
	TestEqual(TEXT("one choice left"), S.LevelUp->GetPendingChoices(), 1);
	TestEqual(TEXT("not closed between the screens"), Recorder->GetOfferClosedCount(), 0);

	TestTrue(TEXT("second pick accepted"), S.LevelUp->ChooseUpgrade(2));
	TestEqual(TEXT("two upgrades picked"), S.Upgrades->GetPickedUpgrades().Num(), 2);
	TestFalse(TEXT("no screen after the last pick"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("closed once"), Recorder->GetOfferClosedCount(), 1);
	TestEqual(TEXT("still two offers"), Recorder->GetOfferCardCounts().Num(), 2);

	// A level gained during the delay joins the running delay (no restart).
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TickFrames(TestWorld, 20);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("level 5"), S.Progression->GetLevel(), 5);
	TestEqual(TEXT("two more choices owed"), S.LevelUp->GetPendingChoices(), 2);
	TickFrames(TestWorld, FramesAfterDelay - 20);
	TestTrue(TEXT("screen opens 0.8 s after the first of the two levels"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("third offer shown"), Recorder->GetOfferCardCounts().Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpResumeTest, "RogueRigs.LevelUp.ChoiceResumes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLevelUpResumeTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLevelUpSetup S;
	if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
	{
		return false;
	}

	TestFalse(TEXT("pick refused without a screen"), S.LevelUp->ChooseUpgrade(0));
	TestFalse(TEXT("reroll refused without a screen"), S.LevelUp->RequestReroll());

	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TickFrames(TestWorld, FramesAfterDelay);
	if (!TestTrue(TEXT("screen open"), S.LevelUp->IsAwaitingChoice()))
	{
		return false;
	}
	const TArray<FRRUpgradeCard> Offered = S.LevelUp->GetOfferCards();
	TestTrue(TEXT("pick accepted"), S.LevelUp->ChooseUpgrade(1));
	TestFalse(TEXT("resumed after the only choice"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("closed once"), Recorder->GetOfferClosedCount(), 1);
	TestEqual(TEXT("no choice owed"), S.LevelUp->GetPendingChoices(), 0);
	TestFalse(TEXT("upgrade offer ended"), S.Upgrades->HasOffer());
	if (Offered.Num() == 3 && S.Upgrades->GetPickedUpgrades().Num() == 1)
	{
		TestEqual(TEXT("the picked card is card 1"), S.Upgrades->GetPickedUpgrades()[0].Stat, Offered[1].Stat);
	}
	TestFalse(TEXT("second pick refused after closing"), S.LevelUp->ChooseUpgrade(0));
	TestEqual(TEXT("still closed once"), Recorder->GetOfferClosedCount(), 1);

	// After resuming, a new level runs the whole flow again.
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestTrue(TEXT("new delay armed"), S.LevelUp->IsChoiceDelayActive());
	TickFrames(TestWorld, FramesAfterDelay);
	TestTrue(TEXT("new screen open"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("second offer shown"), Recorder->GetOfferCardCounts().Num(), 2);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
