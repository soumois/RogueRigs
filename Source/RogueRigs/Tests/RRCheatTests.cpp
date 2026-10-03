// Rogue Rigs - automation test of the cheat console variables (RogueRigs.Cheats.*): rr.Cheat.GrantXP, Reroll, ChooseUpgrade, Invulnerable.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "GAS/RRGameplayTags.h"
#include "HAL/IConsoleManager.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRCheatTests
{
	/** Sets the console variable Name to Value as the console would; fails the test when it does not exist. */
	IConsoleVariable* SetCheat(FAutomationTestBase& Test, const TCHAR* Name, const TCHAR* Value)
	{
		IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(Name);
		if (Test.TestNotNull(FString::Printf(TEXT("%s exists"), Name), Variable))
		{
			Variable->Set(Value, ECVF_SetByConsole);
		}
		return Variable;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRCheatConsoleVariablesTest, "RogueRigs.Cheats.ConsoleVariables",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRCheatConsoleVariablesTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRCheatTests;

	FTestWorld TestWorld(TEXT("RRCheatWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	if (!Miner || !TestNotNull(TEXT("progression"), Miner->GetProgression()) || !TestNotNull(TEXT("level-up flow"), Miner->GetLevelUp()))
	{
		return false;
	}
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}
	URRProgressionComponent* Progression = Miner->GetProgression();
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	Miner->GetUpgrades()->SetRandomSeed(20261003);
	LevelUp->OnUpgradeOfferShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferShown);
	LevelUp->OnUpgradeOfferClosed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferClosed);

	// GrantXP: 5 raw XP to the only miner (no player controller: first miner found), then back to 0.
	IConsoleVariable* GrantXP = SetCheat(*this, TEXT("rr.Cheat.GrantXP"), TEXT("5"));
	TestEqual(TEXT("GrantXP 5: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("GrantXP 5: 0 XP carried"), Progression->GetXP(), 0.f);
	if (GrantXP)
	{
		TestEqual(TEXT("GrantXP back to 0"), GrantXP->GetFloat(), 0.f);
	}
	SetCheat(*this, TEXT("rr.Cheat.GrantXP"), TEXT("3"));
	TestEqual(TEXT("GrantXP 3 again: 3 XP"), Progression->GetXP(), 3.f);
	TestTrue(TEXT("choice delay running"), LevelUp->IsChoiceDelayActive());

	// Reroll and ChooseUpgrade refused without a screen, then drive the screen.
	IConsoleVariable* Choose = SetCheat(*this, TEXT("rr.Cheat.ChooseUpgrade"), TEXT("0"));
	TestEqual(TEXT("pick refused without a screen"), Miner->GetUpgrades()->GetPickedUpgrades().Num(), 0);
	if (Choose)
	{
		TestEqual(TEXT("ChooseUpgrade back to -1"), Choose->GetInt(), -1);
	}
	for (int32 Frame = 0; Frame < 50; ++Frame)
	{
		TestWorld.Tick();
	}
	if (!TestTrue(TEXT("screen open after the delay"), LevelUp->IsAwaitingChoice()))
	{
		return false;
	}
	IConsoleVariable* Reroll = SetCheat(*this, TEXT("rr.Cheat.Reroll"), TEXT("1"));
	TestEqual(TEXT("Reroll 1: offer shown again"), Recorder->GetOfferCardCounts().Num(), 2);
	TestFalse(TEXT("Reroll used"), LevelUp->CanReroll());
	if (Reroll)
	{
		TestEqual(TEXT("Reroll back to 0"), Reroll->GetInt(), 0);
	}
	SetCheat(*this, TEXT("rr.Cheat.ChooseUpgrade"), TEXT("3"));
	TestTrue(TEXT("ChooseUpgrade 3 refused: screen still open"), LevelUp->IsAwaitingChoice());
	const TArray<FRRUpgradeCard> Cards = LevelUp->GetOfferCards();
	SetCheat(*this, TEXT("rr.Cheat.ChooseUpgrade"), TEXT("2"));
	TestFalse(TEXT("ChooseUpgrade 2: screen closed"), LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("closed once"), Recorder->GetOfferClosedCount(), 1);
	if (TestEqual(TEXT("one upgrade picked"), Miner->GetUpgrades()->GetPickedUpgrades().Num(), 1) && Cards.Num() == 3)
	{
		TestTrue(TEXT("card 2 picked"), Miner->GetUpgrades()->GetPickedUpgrades()[0].IsSameChoice(Cards[2]));
	}

	// Invulnerable: one loose count however often it is set to 1, removed by 0.
	UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent();
	const int32 CountBefore = ASC->GetGameplayTagCount(RRGameplayTags::State_Invulnerable);
	SetCheat(*this, TEXT("rr.Cheat.Invulnerable"), TEXT("1"));
	SetCheat(*this, TEXT("rr.Cheat.Invulnerable"), TEXT("1"));
	TestTrue(TEXT("Invulnerable 1: tag on the miner"), ASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable));
	TestEqual(TEXT("Invulnerable 1 twice: one count"), ASC->GetGameplayTagCount(RRGameplayTags::State_Invulnerable), CountBefore + 1);
	SetCheat(*this, TEXT("rr.Cheat.Invulnerable"), TEXT("0"));
	TestEqual(TEXT("Invulnerable 0: count back"), ASC->GetGameplayTagCount(RRGameplayTags::State_Invulnerable), CountBefore);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
