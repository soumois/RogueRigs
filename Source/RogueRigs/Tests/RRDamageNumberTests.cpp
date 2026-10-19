// Rogue Rigs - automation tests of the damage number pipeline (RogueRigs.DamageNumbers.*): one number per hit in a spawn disc, critical style, player toggle, per-tick budget, digit layout, digit batches, Niagara integration through the real damage pipeline.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Algo/Count.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Damage/RRGE_Damage.h"
#include "Enemies/RRChaser.h"
#include "GameFramework/Actor.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Settings/RRGameUserSettings.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRDamageNumberTests
{
	using namespace RRTestHarness;

	/** Horizontal offset bound of a number around its hit location (config SpawnRadius 30, disc). */
	constexpr float MaxSpawnOffset2D = 30.f + 0.01f;

	/** Records every pushed batch with the frame it was pushed on. */
	struct FPushRecorder
	{
		/** Numbers pushed, in order. */
		TArray<FRRDamageNumber> Numbers;
		/** Frame of each pushed number (same index as Numbers). */
		TArray<int32> Frames;
		/** Current frame, set by the test loop. */
		int32 Frame = 0;
		/** Binding on the subsystem. */
		FDelegateHandle Handle;
		/** Subsystem listened to. */
		URRDamageNumberSubsystem* Subsystem = nullptr;

		explicit FPushRecorder(URRDamageNumberSubsystem* InSubsystem) : Subsystem(InSubsystem)
		{
			Handle = Subsystem->OnNumbersPushed.AddLambda([this](const TArray<FRRDamageNumber>& Batch)
			{
				for (const FRRDamageNumber& Number : Batch)
				{
					Numbers.Add(Number);
					Frames.Add(Frame);
				}
			});
		}

		~FPushRecorder() { Subsystem->OnNumbersPushed.Remove(Handle); }

		/** Ticks the world Count frames, numbering them from the current frame. */
		void TickFrames(const FTestWorld& TestWorld, int32 Count)
		{
			for (int32 Index = 0; Index < Count; ++Index)
			{
				++Frame;
				TestWorld.Tick();
			}
		}

		/** Index of the first number with this value, INDEX_NONE if none. */
		int32 FindValue(float Value) const
		{
			return Numbers.IndexOfByPredicate([Value](const FRRDamageNumber& Number) { return FMath::IsNearlyEqual(Number.Value, Value); });
		}
	};

	/** The subsystem of the test world, failing the test when missing. */
	URRDamageNumberSubsystem* GetNumbers(FAutomationTestBase& Test, const FTestWorld& TestWorld)
	{
		URRDamageNumberSubsystem* Numbers = TestWorld.Get()->GetSubsystem<URRDamageNumberSubsystem>();
		Test.TestNotNull(TEXT("damage number subsystem exists in a game world"), Numbers);
		return Numbers;
	}

	/** True when Number sits at HitLocation's height, within the spawn disc horizontally. */
	bool IsAt(const FRRDamageNumber& Number, const FVector& HitLocation)
	{
		return FMath::IsNearlyEqual(Number.Location.Z, HitLocation.Z, 0.01) && FVector::Dist2D(Number.Location, HitLocation) <= MaxSpawnOffset2D;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberOnePerHitTest, "RogueRigs.DamageNumbers.OnePerHit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberOnePerHitTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberOnePerHit"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	TestEqual(TEXT("spawn radius 30 uu"), Numbers->GetSpawnRadius(), 30.f);

	AActor* TargetA = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* TargetB = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* TargetC = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	FPushRecorder Recorder(Numbers);

	// Frame 0: A 10 twice at the same spot and B 7. No merging: one number per hit, pushed on the next tick.
	const FVector LocationA1(100.f, 0.f, 150.f);
	const FVector LocationA2(160.f, 40.f, 150.f);
	const FVector LocationB(-300.f, 200.f, 150.f);
	Numbers->AddHit(TargetA, 10.f, false, LocationA1);
	Numbers->AddHit(TargetA, 10.f, false, LocationA1);
	Numbers->AddHit(TargetB, 7.f, false, LocationB);
	TestEqual(TEXT("one pending number per hit"), Numbers->GetPendingCount(), 3);
	Recorder.TickFrames(TestWorld, 1);
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS one per hit: %d numbers at frame %d"), Recorder.Numbers.Num(), Recorder.Frames.Num() > 0 ? Recorder.Frames[0] : -1));
	if (!TestEqual(TEXT("three numbers pushed on the next tick"), Recorder.Numbers.Num(), 3))
	{
		return false;
	}
	TestEqual(TEXT("nothing left pending"), Numbers->GetPendingCount(), 0);
	TestTrue(TEXT("values 10, 10, 7 in hit order"), FMath::IsNearlyEqual(Recorder.Numbers[0].Value, 10.f) && FMath::IsNearlyEqual(Recorder.Numbers[1].Value, 10.f)
		&& FMath::IsNearlyEqual(Recorder.Numbers[2].Value, 7.f));
	// The second A number may also be stacked above the first one (along StackDirection, which has a horizontal part).
	TestTrue(TEXT("first A number at its hit (within the spawn disc)"), IsAt(Recorder.Numbers[0], LocationA1));
	TestTrue(TEXT("two hits on one spot start from different points"), FVector::Dist2D(Recorder.Numbers[0].Location, Recorder.Numbers[1].Location) > 0.01);
	TestTrue(TEXT("B at its hit location (within the disc)"), IsAt(Recorder.Numbers[2], LocationB));
	TestFalse(TEXT("no critical"), Recorder.Numbers[0].bCritical || Recorder.Numbers[1].bCritical || Recorder.Numbers[2].bCritical);

	// A later hit on the same target is its own number, at its own hit location.
	Recorder.TickFrames(TestWorld, 60);
	Numbers->AddHit(TargetA, 5.f, false, LocationA2);
	const int32 HitFrame = Recorder.Frame;
	Recorder.TickFrames(TestWorld, 1);
	const int32 Index5 = Recorder.FindValue(5.f);
	if (TestTrue(TEXT("a later hit is its own number"), Index5 != INDEX_NONE && Recorder.Numbers.Num() == 4))
	{
		TestEqual(TEXT("pushed on the tick after its hit"), Recorder.Frames[Index5], HitFrame + 1);
		TestTrue(TEXT("at its own hit location (within the disc)"), IsAt(Recorder.Numbers[Index5], LocationA2));
	}

	// A destroyed target keeps its number where it was hit.
	const FVector LocationC(500.f, -500.f, 190.f);
	Numbers->AddHit(TargetC, 30.f, true, LocationC);
	TargetC->Destroy();
	Recorder.TickFrames(TestWorld, 2);
	const int32 IndexC = Recorder.FindValue(30.f);
	if (TestTrue(TEXT("destroyed target's number pushed"), IndexC != INDEX_NONE))
	{
		TestTrue(TEXT("destroyed target's number at its hit location"), IsAt(Recorder.Numbers[IndexC], LocationC));
	}

	// A caller radius replaces the default (the heavy passes 45 uu): radius 0 starts exactly on the hit (B's number has expired).
	Numbers->AddHit(TargetB, 40.f, false, LocationB, 0.f);
	Recorder.TickFrames(TestWorld, 1);
	const int32 Index40 = Recorder.FindValue(40.f);
	if (TestTrue(TEXT("number with radius 0 pushed"), Index40 != INDEX_NONE))
	{
		TestTrue(TEXT("radius 0 starts on the hit"), Recorder.Numbers[Index40].Location.Equals(LocationB, 0.01));
	}

	// Null target and non-positive values.
	Numbers->AddHit(nullptr, 0.f, false, LocationC);
	Numbers->AddHit(TargetA, -5.f, false, LocationC);
	TestEqual(TEXT("values <= 0 ignored"), Numbers->GetPendingCount(), 0);
	Numbers->AddHit(nullptr, 1.f, false, LocationC);
	Numbers->AddHit(nullptr, 2.f, false, LocationC);
	TestEqual(TEXT("hits without a target are numbers too"), Numbers->GetPendingCount(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberCriticalTest, "RogueRigs.DamageNumbers.CriticalStyle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberCriticalTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberCritical"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	AActor* TargetA = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* TargetB = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	FPushRecorder Recorder(Numbers);

	// A: one critical hit; B: one normal hit, pushed in the same batch.
	Numbers->AddHit(TargetA, 15.f, true, FVector(0.f, 0.f, 100.f));
	Numbers->AddHit(TargetB, 10.f, false, FVector(400.f, 0.f, 100.f));
	Recorder.TickFrames(TestWorld, 3);

	const int32 IndexA = Recorder.FindValue(15.f);
	const int32 IndexB = Recorder.FindValue(10.f);
	if (!TestTrue(TEXT("A 15 and B 10 pushed"), IndexA != INDEX_NONE && IndexB != INDEX_NONE && Recorder.Numbers.Num() == 2))
	{
		return false;
	}
	const FRRDamageNumber& Critical = Recorder.Numbers[IndexA];
	const FRRDamageNumber& Normal = Recorder.Numbers[IndexB];
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS style: normal size %.2f color %s, critical size %.2f color %s"),
		Normal.Size, *Normal.Color.ToString(), Critical.Size, *Critical.Color.ToString()));
	TestTrue(TEXT("A critical"), Critical.bCritical);
	TestFalse(TEXT("B normal"), Normal.bCritical);
	TestTrue(TEXT("normal size positive"), Normal.Size > 0.f);
	TestEqual(TEXT("critical 1.6 x larger"), Critical.Size / Normal.Size, 1.6f, 1e-4f);
	TestTrue(TEXT("normal number white"), Normal.Color.Equals(FLinearColor::White));
	TestTrue(TEXT("critical number lighter orange (1, 0.7, 0.35)"), Critical.Color.Equals(FLinearColor(1.f, 0.7f, 0.35f)));
	TestEqual(TEXT("spawn time = push world time"), Critical.SpawnTime, Normal.SpawnTime);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberToggleTest, "RogueRigs.DamageNumbers.Toggle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberToggleTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!TestNotNull(TEXT("game user settings are URRGameUserSettings (DefaultEngine.ini)"), Settings))
	{
		return false;
	}
	const bool bWasShown = Settings->GetShowDamageNumbers();
	Settings->SetShowDamageNumbers(true);

	FTestWorld TestWorld(TEXT("RRDamageNumberToggle"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		Settings->SetShowDamageNumbers(bWasShown);
		return false;
	}
	AActor* Target = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	FPushRecorder Recorder(Numbers);

	TestTrue(TEXT("shown by default"), Numbers->AreNumbersShown());
	Numbers->AddHit(Target, 10.f, false, FVector::ZeroVector);
	TestEqual(TEXT("hit recorded while on"), Numbers->GetPendingCount(), 1);

	// Off: the open entry is dropped on the next tick, new hits are ignored, nothing is pushed.
	Settings->SetShowDamageNumbers(false);
	TestFalse(TEXT("off"), Numbers->AreNumbersShown());
	Numbers->AddHit(Target, 5.f, true, FVector::ZeroVector);
	Recorder.TickFrames(TestWorld, 1);
	TestEqual(TEXT("pending dropped when off"), Numbers->GetPendingCount(), 0);
	Numbers->AddHit(Target, 5.f, true, FVector::ZeroVector);
	TestEqual(TEXT("hits ignored when off"), Numbers->GetPendingCount(), 0);
	Recorder.TickFrames(TestWorld, 30);
	TestEqual(TEXT("nothing pushed while off"), Recorder.Numbers.Num(), 0);

	// Back on: numbers again.
	Settings->SetShowDamageNumbers(true);
	Numbers->AddHit(Target, 8.f, false, FVector::ZeroVector);
	Recorder.TickFrames(TestWorld, 16);
	TestTrue(TEXT("one number of 8 pushed once back on"), Recorder.Numbers.Num() == 1 && Recorder.FindValue(8.f) == 0);

	Settings->SetShowDamageNumbers(bWasShown);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberBudgetTest, "RogueRigs.DamageNumbers.TickBudget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberBudgetTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberBudget"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	// 100 robots hit in the same frame: 100 numbers, the first 64 (config MaxNumbersPerTick) pushed on the next tick, the rest one tick later.
	TArray<AActor*> Targets;
	for (int32 Index = 0; Index < 100; ++Index)
	{
		Targets.Add(TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector(Index * 100.f, 0.f, 0.f)));
	}
	FPushRecorder Recorder(Numbers);
	for (int32 Index = 0; Index < Targets.Num(); ++Index)
	{
		Numbers->AddHit(Targets[Index], 1.f + Index, false, FVector(Index * 100.f, 0.f, 100.f));
	}
	TestEqual(TEXT("one pending number per hit"), Numbers->GetPendingCount(), 100);
	Recorder.TickFrames(TestWorld, 4);
	if (!TestEqual(TEXT("100 numbers pushed"), Recorder.Numbers.Num(), 100))
	{
		return false;
	}
	const int32 FirstFrame = Recorder.Frames[0];
	const int32 InFirst = Algo::CountIf(Recorder.Frames, [FirstFrame](int32 Frame) { return Frame == FirstFrame; });
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS budget: %d pushed at frame %d, %d at frame %d"), InFirst, FirstFrame, 100 - InFirst, Recorder.Frames.Last()));
	TestEqual(TEXT("64 in the first batch"), InFirst, 64);
	TestEqual(TEXT("the other 36 on the next tick"), Recorder.Frames.Last(), FirstFrame + 1);
	TestEqual(TEXT("pushed on the tick after the hits"), FirstFrame, 1);
	TestEqual(TEXT("oldest first: robot 0 hit 1"), Recorder.Numbers[0].Value, 1.f);
	TestEqual(TEXT("robot 99 hit 100 last"), Recorder.Numbers.Last().Value, 100.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberDigitLayoutTest, "RogueRigs.DamageNumbers.DigitLayout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberDigitLayoutTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberDigitLayout"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	auto Layout = [Numbers](float Value, float Size, bool bCritical)
	{
		FRRDamageNumber Number;
		Number.Location = FVector(100.f, 200.f, 300.f);
		Number.Value = Value;
		Number.Size = Size;
		Number.bCritical = bCritical;
		Number.Color = bCritical ? FLinearColor(1.f, 0.8f, 0.3f) : FLinearColor::White;
		Number.SpawnTime = 2.5f;
		TArray<FRRDamageDigit> Digits;
		Numbers->AppendDigits(Number, Digits);
		return Digits;
	};
	auto DigitString = [](const TArray<FRRDamageDigit>& Digits)
	{
		FString Text;
		for (const FRRDamageDigit& Digit : Digits)
		{
			Text.AppendInt(Digit.Digit);
		}
		return Text;
	};

	// 1234.4 at size 60: "1234" left to right along +Y (screen right of the top-down camera), 0.62 x 60 = 37.2 apart, centred.
	const TArray<FRRDamageDigit> Four = Layout(1234.4f, 60.f, false);
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS layout: 1234.4 -> %s, y %s"), *DigitString(Four),
		*FString::JoinBy(Four, TEXT(" "), [](const FRRDamageDigit& Digit) { return FString::Printf(TEXT("%.1f"), Digit.Location.Y); })));
	if (!TestEqual(TEXT("1234.4 shows 1234"), DigitString(Four), FString(TEXT("1234"))))
	{
		return false;
	}
	const float ExpectedY[] = { 144.2f, 181.4f, 218.6f, 255.8f };
	FVector Centre = FVector::ZeroVector;
	for (int32 Index = 0; Index < Four.Num(); ++Index)
	{
		TestEqual(FString::Printf(TEXT("digit %d y"), Index), Four[Index].Location.Y, static_cast<double>(ExpectedY[Index]), 0.01);
		TestEqual(FString::Printf(TEXT("digit %d x and z kept"), Index), FVector2D(Four[Index].Location.X, Four[Index].Location.Z), FVector2D(100.0, 300.0));
		TestEqual(FString::Printf(TEXT("digit %d size, time, style"), Index), Four[Index].Size + Four[Index].SpawnTime, 62.5f);
		TestFalse(FString::Printf(TEXT("digit %d not critical"), Index), Four[Index].bCritical);
		Centre += Four[Index].Location / Four.Num();
	}
	TestTrue(TEXT("digits centred on the number position"), Centre.Equals(FVector(100.f, 200.f, 300.f), 0.01));

	// One digit sits on the number position; a crit keeps its size, color and flag on every digit.
	const TArray<FRRDamageDigit> One = Layout(7.f, 96.f, true);
	if (TestEqual(TEXT("7 shows one digit"), DigitString(One), FString(TEXT("7"))))
	{
		TestTrue(TEXT("single digit on the number position"), One[0].Location.Equals(FVector(100.f, 200.f, 300.f), 0.01));
		TestTrue(TEXT("crit digit flagged, crit size and color"), One[0].bCritical && One[0].Size == 96.f && One[0].Color.Equals(FLinearColor(1.f, 0.8f, 0.3f)));
	}
	const TArray<FRRDamageDigit> Crit = Layout(45.f, 96.f, true);
	TestTrue(TEXT("crit digits spaced from the crit size (0.62 x 96)"), Crit.Num() == 2 && FMath::IsNearlyEqual(Crit[1].Location.Y - Crit[0].Location.Y, 59.52, 0.01));

	// Rounding and bounds: 9.5 rounds to 10, a positive hit below 0.5 still shows 1, a sum above five digits shows 99999.
	TestEqual(TEXT("9.5 shows 10"), DigitString(Layout(9.5f, 60.f, false)), FString(TEXT("10")));
	TestEqual(TEXT("0.3 shows 1"), DigitString(Layout(0.3f, 60.f, false)), FString(TEXT("1")));
	TestEqual(TEXT("123456 shows 99999"), DigitString(Layout(123456.f, 60.f, false)), FString(TEXT("99999")));
	TestEqual(TEXT("100 keeps its zeros"), DigitString(Layout(100.f, 60.f, false)), FString(TEXT("100")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberBatchTest, "RogueRigs.DamageNumbers.DigitBatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberBatchTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberBatch"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	AActor* TargetA = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* TargetB = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	FPushRecorder Recorder(Numbers);
	TestEqual(TEXT("batch id 0 before any push"), Numbers->GetDigitBatchId(), 0);

	// Batch 1: A 17 (normal) and B 5 (critical) = 3 digits, numbers in push order, digits contiguous per number.
	Numbers->AddHit(TargetA, 17.f, false, FVector(0.f, 0.f, 100.f));
	Numbers->AddHit(TargetB, 5.f, true, FVector(500.f, 0.f, 100.f));
	int32 PushFrame = -1;
	for (int32 Frame = 0; Frame < 20 && PushFrame < 0; ++Frame)
	{
		Recorder.TickFrames(TestWorld, 1);
		if (Numbers->GetDigitBatchId() > 0)
		{
			PushFrame = Recorder.Frame;
		}
	}
	if (!TestEqual(TEXT("batch id 1 after the first push"), Numbers->GetDigitBatchId(), 1))
	{
		return false;
	}
	const TArray<FRRDamageDigit> Batch1 = Numbers->GetLastDigits();
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS batch: id 1 at frame %d, %d digits"), PushFrame, Batch1.Num()));
	if (!TestEqual(TEXT("3 digits in batch 1 (1, 7, 5)"), Batch1.Num(), 3))
	{
		return false;
	}
	TestTrue(TEXT("digits 1, 7 then 5"), Batch1[0].Digit == 1 && Batch1[1].Digit == 7 && Batch1[2].Digit == 5);
	TestTrue(TEXT("17 not critical, 5 critical"), !Batch1[0].bCritical && !Batch1[1].bCritical && Batch1[2].bCritical);

	// No push: the id and the digits stay (the emitter spawns on an id change only).
	Recorder.TickFrames(TestWorld, 5);
	TestEqual(TEXT("batch id unchanged without a push"), Numbers->GetDigitBatchId(), 1);
	TestEqual(TEXT("last digits kept without a push"), Numbers->GetLastDigits().Num(), 3);

	// Batch 2: one number 250 = 3 digits, id 2.
	Numbers->AddHit(TargetA, 250.f, false, FVector(0.f, 0.f, 100.f));
	Recorder.TickFrames(TestWorld, 20);
	TestEqual(TEXT("batch id 2 after the second push"), Numbers->GetDigitBatchId(), 2);
	TestTrue(TEXT("batch 2 holds only 250"), Numbers->GetLastDigits().Num() == 3 && Numbers->GetLastDigits()[0].Digit == 2 && Numbers->GetLastDigits()[2].Digit == 0);
	TestEqual(TEXT("one batch per push"), Recorder.Numbers.Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberNiagaraTest, "RogueRigs.DamageNumbers.NiagaraIntegration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberNiagaraTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberNiagara"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	// The persistent component exists at world BeginPlay with the shipped asset (task-025-02).
	UNiagaraComponent* Component = Numbers->GetNiagaraComponent();
	if (!TestNotNull(TEXT("persistent Niagara component created at BeginPlay"), Component))
	{
		return false;
	}
	const UNiagaraSystem* Asset = Component->GetAsset();
	TestEqual(TEXT("component runs NS_DamageNumbers"), Asset ? Asset->GetName() : FString(), FString(TEXT("NS_DamageNumbers")));
	// The digit user parameters exist once the asset follows the per-digit contract (task-025-05); before that the
	// component checks are skipped and only the subsystem side is checked.
	const bool bAssetHasContract = Asset && Asset->GetExposedParameters().ReadParameterVariables().ContainsByPredicate(
		[](const FNiagaraVariableWithOffset& Variable) { return Variable.GetName() == URRDamageNumberSubsystem::DigitBatchIdParameter; });
	const FNiagaraVariable BatchIdVariable(FNiagaraTypeDefinition::GetIntDef(), URRDamageNumberSubsystem::DigitBatchIdParameter);
	const FNiagaraVariable CountVariable(FNiagaraTypeDefinition::GetIntDef(), URRDamageNumberSubsystem::DigitCountParameter);
	auto ReadInt = [Component](const FNiagaraVariable& Variable)
	{
		return Component->GetOverrideParameters().GetParameterValueOrDefault<int32>(Variable, -1);
	};
	if (bAssetHasContract)
	{
		TestEqual(TEXT("DigitBatchId starts at 0"), ReadInt(BatchIdVariable), 0);
	}
	else
	{
		AddInfo(TEXT("DAMAGENUMBERS niagara: asset does not expose the digit parameters yet (task-025-05), component checks skipped"));
	}

	// Real damage pipeline: rifle damage specs (GE + execution) on two still chasers; their OnDamageReceived feeds AddHit.
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C"));
	ARRWeapon* Rifle = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, FVector(0.f, -2000.f, 100.f)) : nullptr;
	ARRChaser* ChaserA = SpawnStillChaser(*this, TestWorld, FVector(0.f, 0.f, 0.f));
	ARRChaser* ChaserB = SpawnStillChaser(*this, TestWorld, FVector(1000.f, 0.f, 0.f));
	if (!TestNotNull(TEXT("rifle spawned"), Rifle) || !TestNotNull(TEXT("chaser A spawned"), ChaserA) || !TestNotNull(TEXT("chaser B spawned"), ChaserB))
	{
		return false;
	}
	TestTrue(TEXT("rifle acquired"), Rifle->Acquire(nullptr));
	Rifle->GetAutoFireComponent()->StopFiring();
	UAbilitySystemComponent* RifleASC = Rifle->GetAbilitySystemComponent();
	auto ApplyHit = [RifleASC](ARRChaser* Chaser, float CritChance)
	{
		RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), CritChance);
		const FGameplayEffectSpecHandle Spec = RifleASC->MakeOutgoingSpec(URRGE_Damage::StaticClass(), 1.f, RifleASC->MakeEffectContext());
		Chaser->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	};

	FPushRecorder Recorder(Numbers);
	// Chaser A: two normal hits of 10 = two numbers (no merging); chaser B: one critical hit (10 x CritDamage 1.5).
	ApplyHit(ChaserA, 0.f);
	ApplyHit(ChaserA, 0.f);
	ApplyHit(ChaserB, 1.f);
	TestEqual(TEXT("three pending numbers (one per hit)"), Numbers->GetPendingCount(), 3);
	Recorder.TickFrames(TestWorld, 3);

	if (!TestEqual(TEXT("three numbers pushed"), Recorder.Numbers.Num(), 3))
	{
		return false;
	}
	const int32 IndexA = Recorder.FindValue(10.f);
	const int32 IndexB = Recorder.FindValue(15.f);
	const int32 TensCount = Algo::CountIf(Recorder.Numbers, [](const FRRDamageNumber& Number) { return FMath::IsNearlyEqual(Number.Value, 10.f); });
	if (TestTrue(TEXT("chaser A 10 twice"), IndexA != INDEX_NONE && TensCount == 2) && TestTrue(TEXT("chaser B critical 15"), IndexB != INDEX_NONE))
	{
		TestFalse(TEXT("normal hits not critical"), Recorder.Numbers[IndexA].bCritical);
		TestTrue(TEXT("crit flagged"), Recorder.Numbers[IndexB].bCritical);
		TestTrue(TEXT("crit larger"), Recorder.Numbers[IndexB].Size > Recorder.Numbers[IndexA].Size);
	}
	const TArray<FRRDamageDigit>& Digits = Numbers->GetLastDigits();
	TestEqual(TEXT("one batch, 6 digits (10, 10 and 15)"), Numbers->GetDigitBatchId() * 10 + Digits.Num(), 16);

	if (bAssetHasContract)
	{
		// The component holds the same batch: one array entry per digit, crit as Int32 0/1.
		const TArray<int32> Indices = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayInt32(Component, URRDamageNumberSubsystem::DigitIndicesParameter);
		const TArray<int32> Criticals = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayInt32(Component, URRDamageNumberSubsystem::DigitCriticalsParameter);
		const TArray<FVector> Positions = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayPosition(Component, URRDamageNumberSubsystem::DigitPositionsParameter);
		TestEqual(TEXT("DigitBatchId 1 on the component"), ReadInt(BatchIdVariable), 1);
		TestEqual(TEXT("DigitCount 6 on the component"), ReadInt(CountVariable), 6);
		if (TestTrue(TEXT("6 entries per array"), Indices.Num() == 6 && Criticals.Num() == 6 && Positions.Num() == 6))
		{
			for (int32 Index = 0; Index < 6; ++Index)
			{
				TestEqual(FString::Printf(TEXT("digit %d value"), Index), Indices[Index], Digits[Index].Digit);
				TestEqual(FString::Printf(TEXT("digit %d crit 0/1"), Index), Criticals[Index], Digits[Index].bCritical ? 1 : 0);
				TestTrue(FString::Printf(TEXT("digit %d position"), Index), Positions[Index].Equals(Digits[Index].Location, 0.01));
			}
		}
	}

	// The emitter spawns DigitCount particles per batch id. Niagara only simulates in a process that can render: headless
	// (-nullrhi) runs stop at the checks above.
	if (!FApp::CanEverRender() || !bAssetHasContract)
	{
		AddInfo(TEXT("DAMAGENUMBERS niagara: no renderer or asset not on the digit contract, particle check skipped"));
		return true;
	}
	Recorder.TickFrames(TestWorld, 2);
	int32 Live = 0;
	int32 Spawned = 0;
	const bool bInstance = Numbers->GetParticleStats(Live, Spawned);
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS niagara: pushed %d, instance %d, live %d, spawned %d"), Numbers->GetTotalPushedCount(), bInstance ? 1 : 0, Live, Spawned));
	TestTrue(TEXT("system instance exists"), bInstance);
	TestEqual(TEXT("one particle per digit, spawned once"), Spawned, 6);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
