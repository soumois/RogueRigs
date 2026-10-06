// Rogue Rigs - automation tests of the cheats (RogueRigs.Cheats.*): rr.Cheat.* console variables and the polled URRCheatSettings property route.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Debug/RRCheatSettings.h"
#include "Debug/RRCheatSubsystem.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Misc/ScopeExit.h"
#include "Objectives/RRRunFlowSubsystem.h"
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
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell, M1TestLoadout) : nullptr;
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

	// GrantXP: 2 raw XP (the level 1 need of XPCurve.csv) to the only miner (no player controller: first miner found), then back to 0.
	IConsoleVariable* GrantXP = SetCheat(*this, TEXT("rr.Cheat.GrantXP"), TEXT("1"));
	TestEqual(TEXT("GrantXP 2: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("GrantXP 2: 0 XP carried"), Progression->GetXP(), 0.f);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRCheatPropertyRouteTest, "RogueRigs.Cheats.PropertyRoute",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRCheatPropertyRouteTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;

	// The default object outlives the test: always put it back to neutral values.
	URRCheatSettings* Settings = GetMutableDefault<URRCheatSettings>();
	auto ResetSettings = [Settings]()
	{
		Settings->GrantXP = 0.f;
		Settings->bInvulnerable = false;
		Settings->ChooseUpgrade = -1;
		Settings->bReroll = false;
		Settings->AlertSpeedMultiplier = 1.f;
		Settings->bSkipToConsole = false;
		Settings->bCompleteConsoleHack = false;
		Settings->bSkipToExtraction = false;
	};
	ResetSettings();
	ON_SCOPE_EXIT { ResetSettings(); };

	FTestWorld TestWorld(TEXT("RRCheatPropertyWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell, M1TestLoadout) : nullptr;
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	if (!Miner || !TestNotNull(TEXT("run flow"), RunFlow) || !TestNotNull(TEXT("level-up flow"), Miner->GetLevelUp()))
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
	RunFlow->SetConfig(FRRRunFlowConfig());
	RunFlow->SetRandomSeed(20261003);
	URRProgressionComponent* Progression = Miner->GetProgression();
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	Miner->GetUpgrades()->SetRandomSeed(20261003);
	LevelUp->OnUpgradeOfferShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferShown);
	LevelUp->OnUpgradeOfferClosed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferClosed);

	// GrantXP: applied by the next tick, then consumed.
	Settings->GrantXP = 1.f;
	TestEqual(TEXT("nothing before the tick"), Progression->GetLevel(), 1);
	TestWorld.Tick();
	TestEqual(TEXT("GrantXP 1: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("GrantXP consumed"), Settings->GrantXP, 0.f);

	// Invulnerable: persistent, one loose count however many ticks, removed when cleared.
	UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent();
	const int32 CountBefore = ASC->GetGameplayTagCount(RRGameplayTags::State_Invulnerable);
	Settings->bInvulnerable = true;
	TestWorld.Tick();
	TestWorld.Tick();
	TestEqual(TEXT("bInvulnerable: one count"), ASC->GetGameplayTagCount(RRGameplayTags::State_Invulnerable), CountBefore + 1);
	TestTrue(TEXT("bInvulnerable stays set"), Settings->bInvulnerable);
	Settings->bInvulnerable = false;
	TestWorld.Tick();
	TestEqual(TEXT("bInvulnerable cleared: count back"), ASC->GetGameplayTagCount(RRGameplayTags::State_Invulnerable), CountBefore);

	// AlertSpeedMultiplier: 10 x for 60 frames = 10 s of alert.
	const float AlertBefore = RunFlow->GetAlertSeconds();
	Settings->AlertSpeedMultiplier = 10.f;
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("x10: 10 s of alert in 1 s"), RunFlow->GetAlertSeconds() - AlertBefore, 10.f, 0.01f);
	Settings->AlertSpeedMultiplier = 1.f;

	// The level-up screen opened during those frames: reroll, then pick.
	if (!TestTrue(TEXT("screen open"), LevelUp->IsAwaitingChoice()))
	{
		return false;
	}
	Settings->bReroll = true;
	TestWorld.Tick();
	TestFalse(TEXT("bReroll consumed"), Settings->bReroll);
	TestEqual(TEXT("bReroll: offer shown again"), Recorder->GetOfferCardCounts().Num(), 2);
	TestFalse(TEXT("reroll used"), LevelUp->CanReroll());
	const TArray<FRRUpgradeCard> Cards = LevelUp->GetOfferCards();
	Settings->ChooseUpgrade = 1;
	TestWorld.Tick();
	TestEqual(TEXT("ChooseUpgrade consumed"), Settings->ChooseUpgrade, -1);
	TestFalse(TEXT("ChooseUpgrade 1: screen closed"), LevelUp->IsAwaitingChoice());
	if (TestEqual(TEXT("one upgrade picked"), Miner->GetUpgrades()->GetPickedUpgrades().Num(), 1) && Cards.Num() == 3)
	{
		TestTrue(TEXT("card 1 picked"), Miner->GetUpgrades()->GetPickedUpgrades()[0].IsSameChoice(Cards[1]));
	}

	// Run flow cheats.
	Settings->bSkipToConsole = true;
	TestWorld.Tick();
	TestFalse(TEXT("bSkipToConsole consumed"), Settings->bSkipToConsole);
	ARRConsole* Console = RunFlow->GetConsole();
	if (!TestNotNull(TEXT("bSkipToConsole: console dropped"), Console))
	{
		return false;
	}
	TestNearlyEqual(TEXT("alert at the threshold"), RunFlow->GetAlert(), 0.4f, 1e-6f);
	TestTrue(TEXT("objective pending"), RunFlow->HasPendingObjective());

	Settings->bCompleteConsoleHack = true;
	TestWorld.Tick();
	TestFalse(TEXT("bCompleteConsoleHack consumed"), Settings->bCompleteConsoleHack);
	TestTrue(TEXT("console hacked"), Console->IsComplete());
	TestFalse(TEXT("nothing pending"), RunFlow->HasPendingObjective());

	Settings->bSkipToExtraction = true;
	TestWorld.Tick();
	TestFalse(TEXT("bSkipToExtraction consumed"), Settings->bSkipToExtraction);
	TestTrue(TEXT("extraction ready"), RunFlow->IsExtractionReady());
	TestEqual(TEXT("alert full"), RunFlow->GetAlert(), 1.f);
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRCheatLightHaloTest, "RogueRigs.Cheats.LightHalo",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRCheatLightHaloTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;

	// The default object outlives the test: the halo fields always go back to their defaults.
	URRCheatSettings* Settings = GetMutableDefault<URRCheatSettings>();
	const float DefaultIntensity = 60.f;
	TestFalse(TEXT("halo off by default"), Settings->bLightHalo);
	ON_SCOPE_EXIT
	{
		Settings->bLightHalo = false;
		Settings->LightHaloIntensity = DefaultIntensity;
		if (IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.Cheat.LightHalo")))
		{
			Variable->Set(TEXT("0"), ECVF_SetByConsole);
		}
		Settings->bLightHalo = false;
	};
	TestEqual(TEXT("default halo intensity"), Settings->LightHaloIntensity, DefaultIntensity);

	FTestWorld TestWorld(TEXT("RRCheatLightHalo"));
	const ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, FineCell(FIntPoint(40, 40))) : nullptr;
	ADirectionalLight* SunActor = TestWorld.Spawn<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 1000.f));
	ASkyLight* SkyActor = TestWorld.Spawn<ASkyLight>(ASkyLight::StaticClass(), FVector(0.f, 0.f, 1000.f));
	URRCheatSubsystem* Cheats = TestWorld.Get()->GetSubsystem<URRCheatSubsystem>();
	UDirectionalLightComponent* Sun = SunActor ? SunActor->FindComponentByClass<UDirectionalLightComponent>() : nullptr;
	USkyLightComponent* Sky = SkyActor ? SkyActor->GetLightComponent() : nullptr;
	UCameraComponent* Camera = Miner ? Miner->FindComponentByClass<UCameraComponent>() : nullptr;
	if (!Miner || !TestNotNull(TEXT("cheat subsystem"), Cheats) || !TestNotNull(TEXT("sun"), Sun) || !TestNotNull(TEXT("sky"), Sky)
		|| !TestNotNull(TEXT("miner camera"), Camera))
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
	const float SunIntensity = Sun->Intensity;
	const float SkyIntensity = Sky->Intensity;
	const bool bBiasOverride = Camera->PostProcessSettings.bOverride_AutoExposureBias != 0;
	const float Bias = Camera->PostProcessSettings.AutoExposureBias;
	TestWorld.Tick();
	TestNull(TEXT("no halo while off"), Cheats->GetLightHalo());

	// On through the console variable: a point light above the miner, sun and sky scaled, exposure bias on the camera.
	RRCheatTests::SetCheat(*this, TEXT("rr.Cheat.LightHalo"), TEXT("1"));
	TestTrue(TEXT("rr.Cheat.LightHalo writes bLightHalo"), Settings->bLightHalo);
	TestWorld.Tick();
	UPointLightComponent* Halo = Cheats->GetLightHalo();
	if (!TestNotNull(TEXT("halo light added"), Halo))
	{
		return false;
	}
	TestTrue(TEXT("halo owned by the miner"), Halo->GetOwner() == Miner && Halo->IsRegistered());
	TestNearlyEqual(TEXT("halo above the miner"), static_cast<float>(Halo->GetComponentLocation().Z - Miner->GetActorLocation().Z), Settings->LightHaloHeight, 0.5f);
	TestNearlyEqual(TEXT("halo intensity"), Halo->Intensity, Settings->LightHaloIntensity, KINDA_SMALL_NUMBER);
	TestNearlyEqual(TEXT("halo radius"), Halo->AttenuationRadius, Settings->LightHaloRadius, KINDA_SMALL_NUMBER);
	TestTrue(TEXT("halo in candelas"), Halo->IntensityUnits == ELightUnits::Candelas);
	TestNearlyEqual(TEXT("sun dimmed"), Sun->Intensity, SunIntensity * Settings->LightHaloAmbientScale, KINDA_SMALL_NUMBER);
	TestNearlyEqual(TEXT("sky dimmed"), Sky->Intensity, SkyIntensity * Settings->LightHaloAmbientScale, KINDA_SMALL_NUMBER);
	TestTrue(TEXT("camera exposure bias overridden"), Camera->PostProcessSettings.bOverride_AutoExposureBias != 0);
	TestNearlyEqual(TEXT("camera exposure bias"), Camera->PostProcessSettings.AutoExposureBias, Bias + Settings->LightHaloExposureBias, KINDA_SMALL_NUMBER);

	// Values editable while on: applied on the next frame, the dimming not compounded.
	Settings->LightHaloIntensity = 120.f;
	TestWorld.Tick();
	Halo = Cheats->GetLightHalo();
	TestTrue(TEXT("edited intensity applied"), Halo && FMath::IsNearlyEqual(Halo->Intensity, 120.f));
	TestNearlyEqual(TEXT("sun dimmed once"), Sun->Intensity, SunIntensity * Settings->LightHaloAmbientScale, KINDA_SMALL_NUMBER);
	TestEqual(TEXT("one halo light on the miner"), TInlineComponentArray<UPointLightComponent*>(Miner).Num(), 1);

	// Off through the property: everything restored.
	Settings->bLightHalo = false;
	TestWorld.Tick();
	TestNull(TEXT("halo removed"), Cheats->GetLightHalo());
	TestEqual(TEXT("no point light left on the miner"), TInlineComponentArray<UPointLightComponent*>(Miner).Num(), 0);
	TestNearlyEqual(TEXT("sun restored"), Sun->Intensity, SunIntensity, KINDA_SMALL_NUMBER);
	TestNearlyEqual(TEXT("sky restored"), Sky->Intensity, SkyIntensity, KINDA_SMALL_NUMBER);
	TestEqual(TEXT("camera bias override restored"), Camera->PostProcessSettings.bOverride_AutoExposureBias != 0, bBiasOverride);
	TestNearlyEqual(TEXT("camera bias restored"), Camera->PostProcessSettings.AutoExposureBias, Bias, KINDA_SMALL_NUMBER);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
