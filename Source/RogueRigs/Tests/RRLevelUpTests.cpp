// Rogue Rigs - automation tests of the level-up flow (RogueRigs.LevelUp.*): heal, choice delay, chained screens, resume.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraDataSetAccessor.h"
#include "NiagaraEmitterInstance.h"
#include "NiagaraEmitterHandle.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraSystemInstanceController.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "SceneInterface.h"
#include "Settings/RRGameUserSettings.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestStatsRows.h"
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
	// PlayerBaseStats.csv Miner MaxHealth (task-420: read, never copied); the 20 % heal is the level-up flow default.
	const float Max = RRTestStats::PlayerRow(*this).MaxHealth;
	TestEqual(TEXT("max health = CSV MaxHealth"), MinerAttr(S.Miner, MaxHealth), Max, 1e-4f);

	// Wounded: 50 + 20 % of max.
	ASC->SetNumericAttributeBase(Health, 50.f);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("level 2"), S.Progression->GetLevel(), 2);
	TestEqual(TEXT("level-up heals 20 % of max: 50 + 0.2 x max"), MinerAttr(S.Miner, Health), 50.f + 0.2f * Max, 1e-4f);
	TestEqual(TEXT("one effect event"), Recorder->GetLevelUpEffects().Num(), 1);
	if (Recorder->GetLevelUpEffects().Num() == 1)
	{
		TestEqual(TEXT("effect for level 2"), Recorder->GetLevelUpEffects()[0], 2);
	}
	TestTrue(TEXT("choice delay running after the level-up"), S.LevelUp->IsChoiceDelayActive());
	TestFalse(TEXT("no choice screen yet"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("no offer yet"), Recorder->GetOfferCardCounts().Num(), 0);

	// Near full: clamped to max.
	ASC->SetNumericAttributeBase(Health, Max - 5.f);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("level 3"), S.Progression->GetLevel(), 3);
	TestEqual(TEXT("max - 5 + 20 % clamped to max"), MinerAttr(S.Miner, Health), Max, 1e-4f);
	TestEqual(TEXT("max health unchanged"), MinerAttr(S.Miner, MaxHealth), Max, 1e-4f);

	// Two levels in one call: two heals.
	ASC->SetNumericAttributeBase(Health, 40.f);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 2));
	TestEqual(TEXT("level 5"), S.Progression->GetLevel(), 5);
	TestEqual(TEXT("two heals: 40 + 2 x 20 % of max"), MinerAttr(S.Miner, Health), FMath::Min(40.f + 2.f * 0.2f * Max, Max), 1e-4f);
	TestEqual(TEXT("four effect events"), Recorder->GetLevelUpEffects().Num(), 4);
	TestEqual(TEXT("four choices owed"), S.LevelUp->GetPendingChoices(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpBurstOncePerMomentTest, "RogueRigs.LevelUp.BurstOncePerMoment",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLevelUpBurstOncePerMomentTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLevelUpSetup S;
	if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
	{
		return false;
	}
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!TestNotNull(TEXT("game user settings"), Settings))
	{
		return false;
	}
	const bool bWasShown = Settings->GetShowDamageNumbers();
	Settings->SetShowDamageNumbers(false);

	// Three levels in one call: one effect event per level, one burst, damage numbers off.
	TestEqual(TEXT("no burst at start"), S.LevelUp->GetLevelUpBurstCount(), 0);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 3));
	TestEqual(TEXT("level 4"), S.Progression->GetLevel(), 4);
	TestEqual(TEXT("three effect events"), Recorder->GetLevelUpEffects().Num(), 3);
	TestEqual(TEXT("one burst for three levels, damage numbers off"), S.LevelUp->GetLevelUpBurstCount(), 1);

	// Next level inside the same moment (20 frames, 0.33 s): no second burst.
	TickFrames(TestWorld, 20);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("one effect event more"), Recorder->GetLevelUpEffects().Num(), 4);
	TestEqual(TEXT("still one burst 0.33 s later"), S.LevelUp->GetLevelUpBurstCount(), 1);

	// Past the moment, before the 0.8 s delay ends (25 frames more, 0.75 s from the first level): a second burst.
	TickFrames(TestWorld, 25);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("a second burst after 0.75 s"), S.LevelUp->GetLevelUpBurstCount(), 2);

	Settings->SetShowDamageNumbers(bWasShown);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpPushTest, "RogueRigs.LevelUp.BurstPushesRobots",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** task-441: the level-up launches robots inside the radius away from the miner, by their resistance, with no damage; robots outside get nothing; one push for several levels. */
bool FRRLevelUpPushTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLevelUpSetup S;
	if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
	{
		return false;
	}
	const FVector Feet = S.Miner->GetActorLocation() - FVector(0.f, 0.f, S.Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	ARRChaser* Inside = SpawnStillChaser(*this, TestWorld, Feet - FVector(150.f, 0.f, 0.f));
	ARRChaser* Outside = SpawnStillChaser(*this, TestWorld, Feet + FVector(0.f, 450.f, 0.f));
	if (!Inside || !Outside)
	{
		return false;
	}
	const float HealthBefore = Inside->GetHealthSet()->GetHealth();
	const FFloatProperty* PushSpeedProperty = FindFProperty<FFloatProperty>(URRLevelUpComponent::StaticClass(), TEXT("PushSpeed"));
	if (!TestNotNull(TEXT("PushSpeed property"), PushSpeedProperty))
	{
		return false;
	}
	const float Speed = PushSpeedProperty->GetPropertyValue_InContainer(S.LevelUp) * (1.f - Inside->GetKnockbackResistance());

	// Two levels at once: one launch, not two.
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 2));
	const FVector Launch = Inside->GetCharacterMovement()->PendingLaunchVelocity;
	TestTrue(TEXT("the robot inside is launched away from the miner once, by its resistance"), Launch.Equals(FVector(-Speed, 0.f, 0.f), 1.f));
	TestTrue(TEXT("the robot outside is not launched"), Outside->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestEqual(TEXT("no damage"), Inside->GetHealthSet()->GetHealth(), HealthBefore);

	// Movement on: how far the launch really carries it.
	UCharacterMovementComponent* Movement = Inside->GetCharacterMovement();
	Movement->SetComponentTickEnabled(true);
	const FVector Start = Inside->GetActorLocation();
	TickFrames(TestWorld, 40);
	const double Travel = (Start - Inside->GetActorLocation()).X;
	AddInfo(FString::Printf(TEXT("level-up push: launch %.0f uu/s, travel %.1f uu in 40 frames"), Speed, Travel));
	TestTrue(TEXT("the robot moved away from the miner"), Travel > 10.0);
	TestEqual(TEXT("still no damage"), Inside->GetHealthSet()->GetHealth(), HealthBefore);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpTextAssetTest, "RogueRigs.LevelUp.TextAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** task-441: BP_Miner plays NS_LevelUpText; under -RenderOffScreen it holds one gold sprite rising straight for about 0.7 s, with the pop. */
bool FRRLevelUpTextAssetTest::RunTest(const FString& Parameters)
{
	UNiagaraSystem* System = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_LevelUpText.NS_LevelUpText"));
	UClass* MinerClass = LoadTestClass<ARRMiner>(*this, MinerClassPath);
	if (!TestNotNull(TEXT("NS_LevelUpText"), System) || !MinerClass)
	{
		return false;
	}
	const URRLevelUpComponent* Defaults = MinerClass->GetDefaultObject<ARRMiner>()->GetLevelUp();
	const FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(URRLevelUpComponent::StaticClass(), TEXT("LevelUpTextSystem"));
	TestTrue(TEXT("BP_Miner plays NS_LevelUpText"), Defaults && Property && Property->GetObjectPropertyValue_InContainer(Defaults) == System);
	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED: FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen."));
		return true;
	}

	FTestWorld TestWorld(TEXT("RRLevelUpTextAsset"));
	UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(TestWorld.Get(), System, FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator,
		FVector::OneVector, /*bAutoDestroy*/ false, /*bAutoActivate*/ true, ENCPoolMethod::None, /*bPreCullCheck*/ false);
	if (!TestNotNull(TEXT("text component"), Component))
	{
		return false;
	}
	const auto Emitter = [Component]() -> FNiagaraEmitterInstance*
	{
		FNiagaraSystemInstanceControllerPtr Controller = Component->GetSystemInstanceController();
		FNiagaraSystemInstance* Instance = Controller.IsValid() ? Controller->GetSystemInstance_Unsafe() : nullptr;
		if (!Instance || Instance->GetEmitters().IsEmpty())
		{
			return nullptr;
		}
		Controller->WaitForConcurrentTickAndFinalize();
		return &Instance->GetEmitters()[0].Get();
	};
	int32 Frame = 0;
	while ((!Emitter() || Emitter()->GetNumParticles() == 0) && Frame < 60)
	{
		TestWorld.Tick();
		++Frame;
	}
	if (!TestTrue(TEXT("the text spawned"), Emitter() && Emitter()->GetNumParticles() == 1))
	{
		return false;
	}
	float FirstWidth = 0.f;
	float LastWidth = 0.f;
	FVector3f First = FVector3f::ZeroVector;
	FVector3f Last = FVector3f::ZeroVector;
	FLinearColor Color = FLinearColor::Black;
	int32 Frames = 0;
	float Mid = 0.f;
	for (; Frames < 60 && Emitter()->GetNumParticles() > 0; ++Frames)
	{
		const FNiagaraDataSet& Data = Emitter()->GetParticleData();
		const auto Sizes = FNiagaraDataSetAccessor<FVector2f>::CreateReader(Data, TEXT("SpriteSize"));
		const auto Positions = FNiagaraDataSetAccessor<FNiagaraPosition>::CreateReader(Data, TEXT("Position"));
		const auto Colors = FNiagaraDataSetAccessor<FLinearColor>::CreateReader(Data, TEXT("Color"));
		if (!TestTrue(TEXT("particle attributes readable"), Sizes.IsValid() && Positions.IsValid() && Colors.IsValid()))
		{
			return false;
		}
		LastWidth = Sizes[0].X;
		Last = FVector3f(Positions[0]);
		if (Frames == 0)
		{
			FirstWidth = LastWidth;
			First = Last;
			Color = Colors[0];
		}
		if (Frames == 20)
		{
			Mid = Last.Z;
		}
		TestWorld.Tick();
	}
	AddInfo(FString::Printf(TEXT("level-up text: %d frames, width %.0f to %.0f uu, position %s to %s, colour %s"), Frames, FirstWidth, LastWidth, *First.ToString(), *Last.ToString(), *Color.ToString()));
	TestTrue(TEXT("lives about 0.7 s"), Frames >= 36 && Frames <= 46);
	TestTrue(TEXT("pop: starts larger, ends at 640 uu"), FirstWidth > LastWidth * 1.1f && FMath::IsNearlyEqual(LastWidth, 640.f, 1.f));
	TestTrue(TEXT("rises straight"), Last.Z > First.Z + 40.f && FMath::IsNearlyEqual(Last.X, First.X, 0.1f) && FMath::IsNearlyEqual(Last.Y, First.Y, 0.1f));
	// A straight constant rise: the second half climbs as much as the first (a thrown arc would fall back).
	TestTrue(TEXT("no arc: still climbing at the end"), Last.Z > Mid + 20.f);
	TestTrue(TEXT("gold"), Color.Equals(FLinearColor(1.f, 0.72f, 0.15f, 1.f), 0.02f));
	Component->DestroyComponent();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLevelUpWaveAssetTest, "RogueRigs.LevelUp.WaveAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * task-441: BP_Miner plays NS_LevelUpWave and has no disc left; a level-up requests the text and the wave once. Under -RenderOffScreen the wave holds
 * one ring and 24 shards, at most 25 sprites: the ring grows to about 2 x User.WaveRadius, the shards run out on the floor plane up to about that radius, all gone within 0.55 s.
 */
bool FRRLevelUpWaveAssetTest::RunTest(const FString& Parameters)
{
	UNiagaraSystem* Wave = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_LevelUpWave.NS_LevelUpWave"));
	UNiagaraSystem* Text = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_LevelUpText.NS_LevelUpText"));
	UMaterial* Material = LoadObject<UMaterial>(nullptr, TEXT("/Game/RogueRigs/FX/M_LevelUpWave.M_LevelUpWave"));
	UClass* MinerClass = LoadTestClass<ARRMiner>(*this, MinerClassPath);
	if (!TestNotNull(TEXT("NS_LevelUpWave"), Wave) || !TestNotNull(TEXT("NS_LevelUpText"), Text) || !TestNotNull(TEXT("M_LevelUpWave"), Material) || !MinerClass)
	{
		return false;
	}
	const URRLevelUpComponent* Defaults = MinerClass->GetDefaultObject<ARRMiner>()->GetLevelUp();
	const FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(URRLevelUpComponent::StaticClass(), TEXT("LevelUpWaveSystem"));
	TestTrue(TEXT("BP_Miner plays NS_LevelUpWave"), Defaults && Property && Property->GetObjectPropertyValue_InContainer(Defaults) == Wave);
	TestNull(TEXT("no disc component left"), FindFProperty<FObjectProperty>(MinerClass, TEXT("LevelUpDisc")));
	TestNull(TEXT("no disc step left"), FindFProperty<FIntProperty>(MinerClass, TEXT("DiscStep")));
	TestNull(TEXT("no disc timer function left"), MinerClass->FindFunctionByName(TEXT("DiscStep_Tick")));

	FTestWorld TestWorld(TEXT("RRLevelUpWaveAsset"));
	{
		TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
		FLevelUpSetup S;
		if (!SpawnLevelUpSetup(*this, TestWorld, Recorder.Get(), S))
		{
			return false;
		}
		const URRNiagaraPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
		if (!TestNotNull(TEXT("Niagara pool"), Pool))
		{
			return false;
		}
		S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 2));
		TestEqual(TEXT("one text requested for two levels"), Pool->GetRequestedCount(Text), 1);
		TestEqual(TEXT("one wave requested for two levels"), Pool->GetRequestedCount(Wave), 1);
	}

	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED: FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen."));
		return true;
	}
	if (FMaterialResource* Resource = Material->GetMaterialResource(TestWorld.Get()->Scene->GetShaderPlatform()))
	{
		Resource->FinishCompilation();
		TestEqual(TEXT("M_LevelUpWave compile errors"), Resource->GetCompileErrors().Num(), 0);
		for (const FString& Error : Resource->GetCompileErrors())
		{
			AddError(Error);
		}
	}

	const FVector Center(0.f, 0.f, 100.f);
	constexpr float Radius = 300.f;
	UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(TestWorld.Get(), Wave, Center, FRotator::ZeroRotator,
		FVector::OneVector, /*bAutoDestroy*/ false, /*bAutoActivate*/ false, ENCPoolMethod::None, /*bPreCullCheck*/ false);
	if (!TestNotNull(TEXT("wave component"), Component))
	{
		return false;
	}
	Component->SetVariableFloat(URRLevelUpComponent::WaveRadiusParameter, Radius);
	Component->Activate(true);
	FNiagaraEmitterInstance* Ring = nullptr;
	FNiagaraEmitterInstance* Shards = nullptr;
	const auto Refresh = [&]()
	{
		Ring = nullptr;
		Shards = nullptr;
		FNiagaraSystemInstanceControllerPtr Controller = Component->GetSystemInstanceController();
		FNiagaraSystemInstance* Instance = Controller.IsValid() ? Controller->GetSystemInstance_Unsafe() : nullptr;
		if (Instance)
		{
			Controller->WaitForConcurrentTickAndFinalize();
			for (const FNiagaraEmitterInstanceRef& Emitter : Instance->GetEmitters())
			{
				(Emitter->GetEmitterHandle().GetName() == FName(TEXT("Core")) ? Ring : Shards) = &Emitter.Get();
			}
		}
		return (Ring ? Ring->GetNumParticles() : 0) + (Shards ? Shards->GetNumParticles() : 0);
	};
	int32 Start = 0;
	while (Refresh() == 0 && Start < 120)
	{
		TestWorld.Tick();
		++Start;
	}
	int32 Largest = 0;
	int32 Frames = 0;
	float RingSize = 0.f;
	double ShardReach = 0.0;
	double ShardHeight = 0.0;
	for (; Frames < 60; ++Frames)
	{
		const int32 Count = Refresh();
		if (Count == 0)
		{
			break;
		}
		Largest = FMath::Max(Largest, Count);
		if (Ring && Ring->GetNumParticles() > 0)
		{
			const auto Sizes = FNiagaraDataSetAccessor<FVector2f>::CreateReader(Ring->GetParticleData(), TEXT("SpriteSize"));
			RingSize = Sizes.IsValid() ? FMath::Max(RingSize, Sizes[0].X) : RingSize;
		}
		if (Shards && Shards->GetNumParticles() > 0)
		{
			const auto Positions = FNiagaraDataSetAccessor<FNiagaraPosition>::CreateReader(Shards->GetParticleData(), TEXT("Position"));
			for (int32 Index = 0; Positions.IsValid() && Index < Shards->GetNumParticles(); ++Index)
			{
				const FVector3f Position(Positions[Index]);
				ShardReach = FMath::Max(ShardReach, FMath::Sqrt(static_cast<double>(Position.X * Position.X + Position.Y * Position.Y)));
				ShardHeight = FMath::Max(ShardHeight, FMath::Abs(static_cast<double>(Position.Z)));
			}
		}
		TestWorld.Tick();
	}
	AddInfo(FString::Printf(TEXT("level-up wave: largest %d sprites, %d frames, ring up to %.0f uu, shards reach %.0f uu, height %.1f"), Largest, Frames, RingSize, ShardReach, ShardHeight));
	TestEqual(TEXT("one ring and 24 shards"), Largest, 25);
	TestTrue(TEXT("the ring grows to about the radius"), RingSize > 2.f * Radius * 0.9f && RingSize <= 2.f * Radius + 1.f);
	TestTrue(TEXT("the shards reach about the radius"), ShardReach > Radius * 0.7 && ShardReach < Radius * 1.2);
	TestTrue(TEXT("one-shot, gone within 0.55 s"), Frames > 0 && Frames <= 34);
	Component->DestroyComponent();
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

	// 30 carried ore: one paid reroll of 15, not the next one of 20 (task-315).
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	if (!TestNotNull(TEXT("run flow"), RunFlow) || !TestTrue(TEXT("30 ore granted"), RunFlow->AddCarriedOre(30)))
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

	// Reroll pays 15 ore and forwards to the offer builder; the next one costs 20.
	TestFalse(TEXT("out-of-range pick refused"), S.LevelUp->ChooseUpgrade(3));
	TestEqual(TEXT("first reroll costs 15"), S.LevelUp->GetRerollCost(), 15);
	TestTrue(TEXT("reroll accepted"), S.LevelUp->RequestReroll());
	TestEqual(TEXT("15 ore spent"), RunFlow->GetCarriedOre(), 15);
	TestEqual(TEXT("next reroll costs 20"), S.LevelUp->GetRerollCost(), 20);
	TestEqual(TEXT("offer shown again after the reroll"), Recorder->GetOfferCardCounts().Num(), 2);
	if (Recorder->GetOfferRerollFlags().Num() == 2)
	{
		TestFalse(TEXT("second reroll not affordable"), Recorder->GetOfferRerollFlags()[1]);
	}
	TestFalse(TEXT("second reroll refused (15 ore < 20)"), S.LevelUp->RequestReroll());
	TestEqual(TEXT("refused reroll spends nothing"), RunFlow->GetCarriedOre(), 15);
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

	// 35 carried ore: a reroll of 15 on the first screen; the chained level-up screen restarts the cost at 15 (task-315).
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	if (!TestNotNull(TEXT("run flow"), RunFlow) || !TestTrue(TEXT("35 ore granted"), RunFlow->AddCarriedOre(35)))
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
	TestTrue(TEXT("first screen reroll accepted"), S.LevelUp->RequestReroll());
	TestEqual(TEXT("first screen next reroll costs 20"), S.LevelUp->GetRerollCost(), 20);

	TestTrue(TEXT("first pick accepted"), S.LevelUp->ChooseUpgrade(0));
	TestEqual(TEXT("one upgrade picked"), S.Upgrades->GetPickedUpgrades().Num(), 1);
	TestTrue(TEXT("second screen open at once (still paused)"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("second offer shown"), Recorder->GetOfferCardCounts().Num(), 3);
	TestEqual(TEXT("chained level-up screen: cost back to 15"), S.LevelUp->GetRerollCost(), 15);
	if (Recorder->GetOfferRerollFlags().Num() == 3)
	{
		TestTrue(TEXT("second screen reroll affordable (20 ore >= 15)"), Recorder->GetOfferRerollFlags()[2]);
	}
	TestEqual(TEXT("one choice left"), S.LevelUp->GetPendingChoices(), 1);
	TestEqual(TEXT("not closed between the screens"), Recorder->GetOfferClosedCount(), 0);

	TestTrue(TEXT("second pick accepted"), S.LevelUp->ChooseUpgrade(2));
	TestEqual(TEXT("two upgrades picked"), S.Upgrades->GetPickedUpgrades().Num(), 2);
	TestFalse(TEXT("no screen after the last pick"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("closed once"), Recorder->GetOfferClosedCount(), 1);
	TestEqual(TEXT("still three offers (two screens, one reroll)"), Recorder->GetOfferCardCounts().Num(), 3);

	// A level gained during the delay joins the running delay (no restart).
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TickFrames(TestWorld, 20);
	S.Progression->AddXP(LevelUpTestRawXPFor(S.Progression, 1));
	TestEqual(TEXT("level 5"), S.Progression->GetLevel(), 5);
	TestEqual(TEXT("two more choices owed"), S.LevelUp->GetPendingChoices(), 2);
	TickFrames(TestWorld, FramesAfterDelay - 20);
	TestTrue(TEXT("screen opens 0.8 s after the first of the two levels"), S.LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("third screen shown"), Recorder->GetOfferCardCounts().Num(), 4);
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
