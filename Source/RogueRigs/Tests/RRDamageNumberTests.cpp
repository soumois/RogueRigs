// Rogue Rigs - automation tests of the damage number pipeline (RogueRigs.DamageNumbers.*): merge window per target, critical style, player toggle, per-tick budget, Niagara integration through the real damage pipeline.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Algo/Count.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Damage/RRGE_Damage.h"
#include "Debug/RRTargetDummy.h"
#include "GameFramework/Actor.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Settings/RRGameUserSettings.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRDamageNumberTests
{
	using namespace RRTestHarness;

	/** Horizontal jitter bound of a number around its hit location (config Jitter 25 on each axis). */
	constexpr float MaxJitter2D = 25.f * UE_SQRT_2 + 0.01f;

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

	/** True when Number sits at HitLocation's height, within the jitter horizontally. */
	bool IsAt(const FRRDamageNumber& Number, const FVector& HitLocation)
	{
		return FMath::IsNearlyEqual(Number.Location.Z, HitLocation.Z, 0.01) && FVector::Dist2D(Number.Location, HitLocation) <= MaxJitter2D;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberMergeTest, "RogueRigs.DamageNumbers.MergeWindow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberMergeTest::RunTest(const FString& Parameters)
{
	using namespace RRDamageNumberTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberMerge"));
	URRDamageNumberSubsystem* Numbers = GetNumbers(*this, TestWorld);
	if (!Numbers)
	{
		return false;
	}
	TestEqual(TEXT("merge window 0.25 s"), Numbers->GetMergeWindow(), 0.25f);

	AActor* TargetA = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* TargetB = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* TargetC = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	FPushRecorder Recorder(Numbers);

	// Frame 0: A 10 and B 7; frame 6 (0.1 s): A 5 at a new location; frame 12 (0.2 s): A 2.
	const FVector LocationA1(100.f, 0.f, 150.f);
	const FVector LocationA2(160.f, 40.f, 150.f);
	const FVector LocationB(-300.f, 200.f, 150.f);
	Numbers->AddHit(TargetA, 10.f, false, LocationA1);
	Numbers->AddHit(TargetB, 7.f, false, LocationB);
	Recorder.TickFrames(TestWorld, 6);
	Numbers->AddHit(TargetA, 5.f, false, LocationA2);
	Recorder.TickFrames(TestWorld, 6);
	Numbers->AddHit(TargetA, 2.f, false, LocationA2);
	TestEqual(TEXT("one open entry per target (A, B)"), Numbers->GetPendingCount(), 2);

	// Frame 14 (0.233 s): window still open.
	Recorder.TickFrames(TestWorld, 2);
	TestEqual(TEXT("nothing pushed before the window closes"), Recorder.Numbers.Num(), 0);
	Recorder.TickFrames(TestWorld, 2);
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS merge: %d numbers, first at frame %d"), Recorder.Numbers.Num(), Recorder.Frames.Num() > 0 ? Recorder.Frames[0] : -1));
	if (!TestEqual(TEXT("A and B pushed once each"), Recorder.Numbers.Num(), 2))
	{
		return false;
	}
	const int32 IndexA = Recorder.FindValue(17.f);
	const int32 IndexB = Recorder.FindValue(7.f);
	if (!TestTrue(TEXT("A merged into 17, B alone 7"), IndexA != INDEX_NONE && IndexB != INDEX_NONE))
	{
		return false;
	}
	TestTrue(TEXT("window closes 0.25 s after the first hit (frame 15, +1)"), Recorder.Frames[IndexA] >= 15 && Recorder.Frames[IndexA] <= 16);
	TestEqual(TEXT("A and B pushed in the same batch"), Recorder.Frames[IndexA], Recorder.Frames[IndexB]);
	TestTrue(TEXT("A at its last hit location (within the jitter)"), IsAt(Recorder.Numbers[IndexA], LocationA2));
	TestTrue(TEXT("B at its hit location (within the jitter)"), IsAt(Recorder.Numbers[IndexB], LocationB));
	TestFalse(TEXT("no critical"), Recorder.Numbers[IndexA].bCritical || Recorder.Numbers[IndexB].bCritical);
	TestEqual(TEXT("nothing left pending"), Numbers->GetPendingCount(), 0);

	// A hit after the window closed opens a new number.
	const int32 SecondStart = Recorder.Frame;
	Numbers->AddHit(TargetA, 3.f, false, LocationA1);
	Recorder.TickFrames(TestWorld, 16);
	Numbers->AddHit(TargetA, 4.f, false, LocationA1);
	TestEqual(TEXT("a hit after the window opens a new entry"), Numbers->GetPendingCount(), 1);
	Recorder.TickFrames(TestWorld, 16);
	const int32 Index3 = Recorder.FindValue(3.f);
	const int32 Index4 = Recorder.FindValue(4.f);
	TestTrue(TEXT("3 and 4 pushed separately"), Index3 != INDEX_NONE && Index4 != INDEX_NONE && Recorder.Numbers.Num() == 4);
	if (Index3 != INDEX_NONE)
	{
		TestTrue(TEXT("second number 0.25 s after its hit"), Recorder.Frames[Index3] - SecondStart >= 15 && Recorder.Frames[Index3] - SecondStart <= 16);
	}

	// A destroyed target keeps its number where it was hit.
	const FVector LocationC(500.f, -500.f, 190.f);
	Numbers->AddHit(TargetC, 30.f, true, LocationC);
	TargetC->Destroy();
	Recorder.TickFrames(TestWorld, 16);
	const int32 IndexC = Recorder.FindValue(30.f);
	if (TestTrue(TEXT("destroyed target's number pushed"), IndexC != INDEX_NONE))
	{
		TestTrue(TEXT("destroyed target's number at its hit location"), IsAt(Recorder.Numbers[IndexC], LocationC));
	}

	// Null target and non-positive values.
	Numbers->AddHit(nullptr, 0.f, false, LocationC);
	Numbers->AddHit(TargetA, -5.f, false, LocationC);
	TestEqual(TEXT("values <= 0 ignored"), Numbers->GetPendingCount(), 0);
	Numbers->AddHit(nullptr, 1.f, false, LocationC);
	Numbers->AddHit(nullptr, 2.f, false, LocationC);
	TestEqual(TEXT("hits without a target never merge"), Numbers->GetPendingCount(), 2);
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

	// A: normal then critical within the window = one critical number; B: normal only.
	Numbers->AddHit(TargetA, 10.f, false, FVector(0.f, 0.f, 100.f));
	Numbers->AddHit(TargetB, 10.f, false, FVector(400.f, 0.f, 100.f));
	Recorder.TickFrames(TestWorld, 3);
	Numbers->AddHit(TargetA, 15.f, true, FVector(0.f, 0.f, 100.f));
	Recorder.TickFrames(TestWorld, 20);

	const int32 IndexA = Recorder.FindValue(25.f);
	const int32 IndexB = Recorder.FindValue(10.f);
	if (!TestTrue(TEXT("A 25 and B 10 pushed"), IndexA != INDEX_NONE && IndexB != INDEX_NONE && Recorder.Numbers.Num() == 2))
	{
		return false;
	}
	const FRRDamageNumber& Critical = Recorder.Numbers[IndexA];
	const FRRDamageNumber& Normal = Recorder.Numbers[IndexB];
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS style: normal size %.2f color %s, critical size %.2f color %s"),
		Normal.Size, *Normal.Color.ToString(), Critical.Size, *Critical.Color.ToString()));
	TestTrue(TEXT("A critical (one critical hit in the merge)"), Critical.bCritical);
	TestFalse(TEXT("B normal"), Normal.bCritical);
	TestTrue(TEXT("normal size positive"), Normal.Size > 0.f);
	TestEqual(TEXT("critical 1.6 x larger"), Critical.Size / Normal.Size, 1.6f, 1e-4f);
	TestTrue(TEXT("normal number white"), Normal.Color.Equals(FLinearColor::White));
	TestTrue(TEXT("critical number white-gold (1, 0.85, 0.45)"), Critical.Color.Equals(FLinearColor(1.f, 0.85f, 0.45f)));
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
	// 100 robots hit twice in the same frame: 100 entries, the first 64 (config MaxNumbersPerTick) pushed when the window closes, the rest on the next tick.
	TArray<AActor*> Targets;
	for (int32 Index = 0; Index < 100; ++Index)
	{
		Targets.Add(TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector(Index * 100.f, 0.f, 0.f)));
	}
	FPushRecorder Recorder(Numbers);
	for (int32 Index = 0; Index < Targets.Num(); ++Index)
	{
		Numbers->AddHit(Targets[Index], 1.f + Index, false, FVector(Index * 100.f, 0.f, 100.f));
		Numbers->AddHit(Targets[Index], 1000.f, false, FVector(Index * 100.f, 0.f, 100.f));
	}
	TestEqual(TEXT("one entry per robot"), Numbers->GetPendingCount(), 100);
	Recorder.TickFrames(TestWorld, 18);
	if (!TestEqual(TEXT("100 numbers pushed"), Recorder.Numbers.Num(), 100))
	{
		return false;
	}
	const int32 FirstFrame = Recorder.Frames[0];
	const int32 InFirst = Algo::CountIf(Recorder.Frames, [FirstFrame](int32 Frame) { return Frame == FirstFrame; });
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS budget: %d pushed at frame %d, %d at frame %d"), InFirst, FirstFrame, 100 - InFirst, Recorder.Frames.Last()));
	TestEqual(TEXT("64 in the first batch"), InFirst, 64);
	TestEqual(TEXT("the other 36 on the next tick"), Recorder.Frames.Last(), FirstFrame + 1);
	TestEqual(TEXT("oldest first: robot 0 merged 1001"), Recorder.Numbers[0].Value, 1001.f);
	TestEqual(TEXT("robot 99 merged 1100"), Recorder.Numbers.Last().Value, 1100.f);
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
	TestEqual(TEXT("component runs NS_DamageNumbers"), Component->GetAsset() ? Component->GetAsset()->GetName() : FString(), FString(TEXT("NS_DamageNumbers")));
	const FNiagaraVariable NewCountVariable(FNiagaraTypeDefinition::GetIntDef(), URRDamageNumberSubsystem::NewCountParameter);
	auto ReadNewCount = [Component, &NewCountVariable]()
	{
		return Component->GetOverrideParameters().GetParameterValueOrDefault<int32>(NewCountVariable, -1);
	};
	TestEqual(TEXT("NewCount user parameter exists and starts at 0"), ReadNewCount(), 0);

	// Real damage pipeline: rifle damage specs (GE + execution) on two target dummies; their OnDamageReceived feeds AddHit.
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C"));
	UClass* DummyClass = LoadTestClass<ARRTargetDummy>(*this, TEXT("/Game/RogueRigs/Blueprints/Debug/BP_TargetDummy.BP_TargetDummy_C"));
	ARRWeapon* Rifle = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, FVector(0.f, -2000.f, 100.f)) : nullptr;
	ARRTargetDummy* DummyA = DummyClass ? TestWorld.Spawn<ARRTargetDummy>(DummyClass, FVector(0.f, 0.f, 0.f)) : nullptr;
	ARRTargetDummy* DummyB = DummyClass ? TestWorld.Spawn<ARRTargetDummy>(DummyClass, FVector(1000.f, 0.f, 0.f)) : nullptr;
	if (!TestNotNull(TEXT("rifle spawned"), Rifle) || !TestNotNull(TEXT("dummy A spawned"), DummyA) || !TestNotNull(TEXT("dummy B spawned"), DummyB))
	{
		return false;
	}
	TestTrue(TEXT("rifle acquired"), Rifle->Acquire(nullptr));
	Rifle->GetAutoFireComponent()->StopFiring();
	UAbilitySystemComponent* RifleASC = Rifle->GetAbilitySystemComponent();
	auto ApplyHit = [RifleASC](ARRTargetDummy* Dummy, float CritChance)
	{
		RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), CritChance);
		const FGameplayEffectSpecHandle Spec = RifleASC->MakeOutgoingSpec(URRGE_Damage::StaticClass(), 1.f, RifleASC->MakeEffectContext());
		Dummy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	};

	FPushRecorder Recorder(Numbers);
	int32 NewCountAtPush = -1;
	const FDelegateHandle CountHandle = Numbers->OnNumbersPushed.AddLambda([&NewCountAtPush, &ReadNewCount](const TArray<FRRDamageNumber>&)
	{
		NewCountAtPush = ReadNewCount();
	});
	// Dummy A: two normal hits (10 + 10) merge; dummy B: one critical hit (10 x CritDamage 1.5).
	ApplyHit(DummyA, 0.f);
	ApplyHit(DummyA, 0.f);
	ApplyHit(DummyB, 1.f);
	TestEqual(TEXT("two open entries (one per dummy)"), Numbers->GetPendingCount(), 2);
	Recorder.TickFrames(TestWorld, 18);
	Numbers->OnNumbersPushed.Remove(CountHandle);

	if (!TestEqual(TEXT("two numbers pushed"), Recorder.Numbers.Num(), 2))
	{
		return false;
	}
	TestEqual(TEXT("NewCount = 2 on the push frame"), NewCountAtPush, 2);
	TestEqual(TEXT("NewCount back to 0 after the push frame"), ReadNewCount(), 0);
	const int32 IndexA = Recorder.FindValue(20.f);
	const int32 IndexB = Recorder.FindValue(15.f);
	if (TestTrue(TEXT("dummy A merged 20"), IndexA != INDEX_NONE) && TestTrue(TEXT("dummy B critical 15"), IndexB != INDEX_NONE))
	{
		TestFalse(TEXT("merged normal hits not critical"), Recorder.Numbers[IndexA].bCritical);
		TestTrue(TEXT("crit flagged"), Recorder.Numbers[IndexB].bCritical);
		TestTrue(TEXT("crit larger"), Recorder.Numbers[IndexB].Size > Recorder.Numbers[IndexA].Size);
	}

	// The emitter spawns from NewCount. Niagara only simulates in a process that can render: headless (-nullrhi) runs
	// stop at the parameter checks above.
	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("DAMAGENUMBERS niagara: no renderer, particle check skipped"));
		return true;
	}
	Recorder.TickFrames(TestWorld, 2);
	int32 Live = 0;
	int32 Spawned = 0;
	const bool bInstance = Numbers->GetParticleStats(Live, Spawned);
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS niagara: pushed %d, instance %d, live %d, spawned %d"), Numbers->GetTotalPushedCount(), bInstance ? 1 : 0, Live, Spawned));
	TestTrue(TEXT("system instance exists"), bInstance);
	TestTrue(TEXT("particles spawned from the push"), Spawned > 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
