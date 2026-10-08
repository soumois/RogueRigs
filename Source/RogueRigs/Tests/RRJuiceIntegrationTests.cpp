// Rogue Rigs - automation tests of the juice with the final Blueprints (RogueRigs.Integration.Juice*) and of the damage-number stacking over displayed numbers (RogueRigs.DamageNumbers.StackOverDisplayed, DisplayFromAsset): task-027-03.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Misc/App.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Debug/RRStressSubsystem.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RRKamikaze.h"
#include "Engine/World.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectIterator.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRJuiceTests
{
	/** Final Blueprints of the juice (task-027-02). */
	const TCHAR* const JuiceChaserPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");
	const TCHAR* const JuiceKamikazePath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze.BP_Kamikaze_C");
	const TCHAR* const JuicePickupXPPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupXP.BP_PickupXP_C");
	const TCHAR* const JuicePickupHealPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");
	/** Collect sparkle BP_PickupXP spawns with SpawnSystemAtLocation since task-054-01 (bp-pickup-xp). */
	const TCHAR* const JuiceSparkleSystemPath = TEXT("/Game/RogueRigs/FX/NS_PickupSparkle.NS_PickupSparkle");
	const TCHAR* const JuiceHealRingPath = TEXT("/Game/RogueRigs/Blueprints/FX/BP_HealRing.BP_HealRing_C");

	/** BP_Kamikaze CoreLight peak (bp-kamikaze: intensity = flare progress x 10000 since task-028-02). */
	constexpr float JuiceCoreLightPeak = 10000.f;

	/** Open floor cell of the legacy arena_m1 layout (pickup motion and kamikaze fuse tests). */
	const FIntPoint JuiceMinerCell(40, 40);

	/** Spawns BP_Miner on JuiceMinerCell of the legacy grid, possessed by a player controller (player 0 pawn), weapons stopped. */
	ARRMiner* SpawnJuiceMiner(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, ARRArenaGrid*& OutGrid)
	{
		OutGrid = RRTestHarness::SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = OutGrid ? RRTestHarness::SpawnMiner(Test, TestWorld, OutGrid, JuiceMinerCell) : nullptr;
		if (!Miner)
		{
			return nullptr;
		}
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		APlayerController* Controller = TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation());
		if (!Test.TestNotNull(TEXT("player controller spawned"), Controller))
		{
			return nullptr;
		}
		Controller->Possess(Miner);
		return Miner;
	}

	/** Spawns a robot of Class with its feet at FloorLocation. */
	ARREnemy* SpawnJuiceRobot(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, UClass* Class, const FVector& FloorLocation)
	{
		const ARREnemy* Default = Class ? Cast<ARREnemy>(Class->GetDefaultObject()) : nullptr;
		if (!Test.TestNotNull(TEXT("robot class"), Default))
		{
			return nullptr;
		}
		const FVector Location = FloorLocation + FVector(0.f, 0.f, Default->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.f);
		ARREnemy* Robot = TestWorld.Spawn<ARREnemy>(Class, Location);
		Test.TestNotNull(TEXT("robot spawned"), Robot);
		return Robot;
	}

	/** Component of Actor named Name (Blueprint SCS components keep their variable name), or null. */
	template <typename T>
	T* FindJuiceComponent(const AActor* Actor, const TCHAR* Name)
	{
		TArray<T*> Components;
		if (Actor)
		{
			Actor->GetComponents(Components);
		}
		for (T* Component : Components)
		{
			if (Component && Component->GetName() == Name)
			{
				return Component;
			}
		}
		return nullptr;
	}

	/** True when Component renders in game: visible flag set and not Hidden In Game. */
	bool IsShownInGame(const USceneComponent* Component)
	{
		return Component && Component->IsVisible() && !Component->bHiddenInGame;
	}

	/** Records the actors of one class spawned in a world (spawn time, location), from a world actor-spawned handler. */
	struct FJuiceSpawnLog
	{
		struct FEntry
		{
			TWeakObjectPtr<AActor> Actor;
			double Time = 0.0;
			FVector Location = FVector::ZeroVector;
		};
		TArray<FEntry> Entries;
		FDelegateHandle Handle;
		UWorld* World = nullptr;

		FJuiceSpawnLog(UWorld* InWorld, UClass* Class) : World(InWorld)
		{
			Handle = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda([this, Class](AActor* Actor)
			{
				if (Class && Actor && Actor->IsA(Class))
				{
					Entries.Add({ Actor, World->GetTimeSeconds(), Actor->GetActorLocation() });
				}
			}));
		}

		~FJuiceSpawnLog()
		{
			World->RemoveOnActorSpawnedHandler(Handle);
		}
	};

	/** True when the TrailFX Niagara component of an XP pickup is active (task-054-01: replaces the Trail static mesh). */
	bool IsJuiceTrailActive(const AActor* Cube)
	{
		const UNiagaraComponent* TrailFX = FindJuiceComponent<UNiagaraComponent>(Cube, TEXT("TrailFX"));
		return TrailFX && TrailFX->IsActive();
	}

	/** Records the Niagara components of one system created in a world (first-seen location), polled after each tick. */
	struct FJuiceNiagaraLog
	{
		struct FEntry
		{
			TWeakObjectPtr<UNiagaraComponent> Component;
			FVector Location = FVector::ZeroVector;
		};
		TArray<FEntry> Entries;
		TSet<const UNiagaraComponent*> Known;

		/** Registers the components of System already in World so only later spawns are recorded. */
		FJuiceNiagaraLog(const UWorld* World, const UNiagaraSystem* System)
		{
			Poll(World, System, false);
		}

		void Poll(const UWorld* World, const UNiagaraSystem* System, bool bRecord = true)
		{
			for (TObjectIterator<UNiagaraComponent> It; It; ++It)
			{
				UNiagaraComponent* Component = *It;
				if (Component->GetWorld() != World || Component->GetAsset() != System || Known.Contains(Component))
				{
					continue;
				}
				Known.Add(Component);
				if (bRecord)
				{
					Entries.Add({ Component, Component->GetComponentLocation() });
				}
			}
		}
	};

	/** Screen footprint of a pushed number (DigitAdvance 0.62 x size per digit, size tall), as URRDamageNumberSubsystem computes it. */
	FVector2D JuiceFootprint(const FRRDamageNumber& Number)
	{
		const int32 Digits = FString::FromInt(FMath::Max(FMath::RoundToInt(Number.Value), 1)).Len();
		return FVector2D(0.62f * Number.Size * Digits, Number.Size);
	}

	/** True when two numbers overlap on screen (right = +Y, up = StackDirection of the defaults), with a 0.5 uu tolerance. */
	bool JuiceOverlap(const FVector& LocationA, const FVector2D& SizeA, const FVector& LocationB, const FVector2D& SizeB)
	{
		const FVector Right(0.f, 1.f, 0.f);
		const FVector Up = FVector(0.766f, 0.f, 0.643f).GetSafeNormal();
		const FVector Delta = LocationB - LocationA;
		return FMath::Abs(FVector::DotProduct(Delta, Right)) < 0.5 * (SizeA.X + SizeB.X) - 0.5
			&& FMath::Abs(FVector::DotProduct(Delta, Up)) < 0.5 * (SizeA.Y + SizeB.Y) - 0.5;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberDisplayFromAssetTest, "RogueRigs.DamageNumbers.DisplayFromAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberDisplayFromAssetTest::RunTest(const FString& Parameters)
{
	RRTestHarness::FTestWorld TestWorld(TEXT("RRDamageNumberDisplay"));
	const URRDamageNumberSubsystem* Numbers = TestWorld.Get()->GetSubsystem<URRDamageNumberSubsystem>();
	if (!TestNotNull(TEXT("damage number subsystem"), Numbers) || !TestNotNull(TEXT("NS_DamageNumbers component"), Numbers->GetNiagaraComponent()))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("DAMAGENUMBERS display: lifetime %.3f s, rise %s, from asset %d"), Numbers->GetDisplayLifetime(),
		*Numbers->GetRiseVelocity().ToString(), Numbers->IsDisplayReadFromAsset() ? 1 : 0));
	// NS_DamageNumbers keeps no direct Lifetime constant (only a stale random-range Max of 1.75 s) and rises without a velocity
	// constant: the config values stand for it (bp-damage-numbers: lifetime 0.7 s, rise 85.7 uu/s) until the asset exposes them.
	TestFalse(TEXT("stale Lifetime Min / Max of NS_DamageNumbers ignored"), Numbers->IsDisplayReadFromAsset() && Numbers->GetDisplayLifetime() > 1.f);
	TestEqual(TEXT("NS_DamageNumbers lifetime 0.7 s (bp-damage-numbers)"), Numbers->GetDisplayLifetime(), 0.7f, 0.01f);
	TestEqual(TEXT("rise 85.7 uu/s up (bp-damage-numbers)"), static_cast<float>(Numbers->GetRiseVelocity().Z), 85.7f, 0.1f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberStackOverDisplayedTest, "RogueRigs.DamageNumbers.StackOverDisplayed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberStackOverDisplayedTest::RunTest(const FString& Parameters)
{
	using namespace RRJuiceTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRDamageNumberStackOverDisplayed"));
	URRDamageNumberSubsystem* Numbers = TestWorld.Get()->GetSubsystem<URRDamageNumberSubsystem>();
	if (!TestNotNull(TEXT("damage number subsystem"), Numbers))
	{
		return false;
	}
	struct FPushed
	{
		FRRDamageNumber Number;
		double Time = 0.0;
	};
	TArray<FPushed> Pushed;
	UWorld* World = TestWorld.Get();
	const FDelegateHandle Handle = Numbers->OnNumbersPushed.AddLambda([&Pushed, World](const TArray<FRRDamageNumber>& Batch)
	{
		for (const FRRDamageNumber& Number : Batch)
		{
			Pushed.Add({ Number, World->GetTimeSeconds() });
		}
	});
	AActor* RobotA = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	AActor* RobotB = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	const FVector Hit(0.f, 0.f, 150.f);
	const FVector Rise = Numbers->GetRiseVelocity();
	const double Lifetime = Numbers->GetDisplayLifetime();

	// Sustained fire on one robot, as in screenshot Juice_06: a hit every 0.25 s (15 frames) for 1 s, so each new number
	// opens while the previous ones are still rising on screen. A second robot 5 uu away is hit in between: with the
	// +-25 uu random jitter, B stays within the 55.8 uu half-width shared with a 2-digit A number, so it always overlaps
	// their column and must stack (at 20 uu, about 1.4 % of jitter draws put it beside the column: not stacked, task-050-01).
	for (int32 Frame = 0; Frame < 90; ++Frame)
	{
		if (Frame % 15 == 0 && Frame < 75)
		{
			Numbers->AddHit(RobotA, 12.f, false, Hit);
		}
		if (Frame == 22)
		{
			Numbers->AddHit(RobotB, 8.f, false, Hit + FVector(0.f, 5.f, 0.f));
		}
		TestWorld.Tick();
	}
	if (!TestEqual(TEXT("six numbers pushed (five on A, one on B)"), Pushed.Num(), 6))
	{
		Numbers->OnNumbersPushed.Remove(Handle);
		return false;
	}
	TestTrue(TEXT("displayed numbers tracked"), Numbers->GetDisplayedCount() > 0);

	// At each push, the new number must not overlap any number still on screen (risen since its own push).
	int32 Overlaps = 0;
	int32 Stacked = 0;
	for (int32 New = 1; New < Pushed.Num(); ++New)
	{
		const FPushed& Fresh = Pushed[New];
		for (int32 Old = 0; Old < New; ++Old)
		{
			const double Age = Fresh.Time - Pushed[Old].Time;
			if (Age >= Lifetime - 1e-3)
			{
				continue;
			}
			const FVector OldNow = Pushed[Old].Number.Location + Rise * Age;
			const bool bOverlap = JuiceOverlap(OldNow, JuiceFootprint(Pushed[Old].Number), Fresh.Number.Location, JuiceFootprint(Fresh.Number));
			AddInfo(FString::Printf(TEXT("DAMAGENUMBERS displayed: #%d (%.0f at %.3f s) vs #%d (age %.3f s): dz %.1f%s"), New, Fresh.Number.Value, Fresh.Time,
				Old, Age, Fresh.Number.Location.Z - OldNow.Z, bOverlap ? TEXT(" OVERLAP") : TEXT("")));
			Overlaps += bOverlap ? 1 : 0;
		}
		Stacked += Fresh.Number.Location.Z > Hit.Z + 1.0 ? 1 : 0;
	}
	TestEqual(TEXT("no new number overlaps a number still rising on screen"), Overlaps, 0);
	TestTrue(TEXT("numbers opened over displayed ones were stacked"), Stacked >= 4);

	// Once every number has expired, a new hit at the same place is not stacked.
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("expired numbers dropped"), Numbers->GetDisplayedCount(), 0);
	Numbers->AddHit(RobotA, 5.f, false, Hit);
	for (int32 Frame = 0; Frame < 16; ++Frame)
	{
		TestWorld.Tick();
	}
	Numbers->OnNumbersPushed.Remove(Handle);
	TestTrue(TEXT("a number after the display lifetime is not stacked"), Pushed.Num() == 7 && FMath::IsNearlyEqual(Pushed.Last().Number.Location.Z, Hit.Z, 0.01));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRJuiceKillStreamTest, "RogueRigs.Integration.JuiceKillDropsStreamingCubes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRJuiceKillStreamTest::RunTest(const FString& Parameters)
{
	using namespace RRJuiceTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRJuiceKillStream"));
	UClass* ChaserClass = RRTestHarness::LoadTestClass<ARREnemy>(*this, JuiceChaserPath);
	UClass* CubeClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, JuicePickupXPPath);
	const UNiagaraSystem* SparkleSystem = LoadObject<UNiagaraSystem>(nullptr, JuiceSparkleSystemPath);
	TestNotNull(TEXT("NS_PickupSparkle loads"), SparkleSystem);
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = ChaserClass && CubeClass && SparkleSystem ? SpawnJuiceMiner(*this, TestWorld, Grid) : nullptr;
	if (!Miner)
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(JuiceMinerCell.X, JuiceMinerCell.Y);
	ARREnemy* Chaser = SpawnJuiceRobot(*this, TestWorld, ChaserClass, MinerFloor + FVector(60.f, 0.f, 0.f));
	if (!Chaser)
	{
		return false;
	}
	TestWorld.Tick();

	FJuiceNiagaraLog Sparkles(TestWorld.Get(), SparkleSystem);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollectedAt.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollectedAt);
	const TArray<FVector>& CollectedAt = Recorder->GetCollectedPickupLocations();
	// Total XP (needs of the levels passed + XP inside the level): one chaser can complete a level (task-031-03 curve: level 1 needs 1).
	const URRProgressionComponent* JuiceProgression = Miner->FindComponentByClass<URRProgressionComponent>();
	auto JuiceTotalXP = [JuiceProgression]()
	{
		float Total = JuiceProgression->GetXP();
		for (int32 Level = 1; Level < JuiceProgression->GetLevel(); ++Level)
		{
			Total += JuiceProgression->GetXPToNextLevelAt(Level);
		}
		return Total;
	};
	const float XPBefore = JuiceTotalXP();
	TestTrue(TEXT("chaser killed"), URRStressSubsystem::KillEnemy(Chaser));

	// The cubes drop at the end of the death shrink.
	TArray<TWeakObjectPtr<ARRPickup>> Cubes;
	for (int32 Wait = 0; Wait < 120 && !TActorIterator<ARRPickup>(TestWorld.Get()); ++Wait)
	{
		TestWorld.Tick();
	}
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		if (It->IsA(CubeClass))
		{
			Cubes.Add(*It);
		}
	}
	if (!TestEqual(TEXT("BP_Chaser drops 3 BP_PickupXP cubes"), Cubes.Num(), 3))
	{
		return false;
	}
	for (const TWeakObjectPtr<ARRPickup>& Cube : Cubes)
	{
		TestNotNull(TEXT("BP_PickupXP has a TrailFX Niagara component"), FindJuiceComponent<UNiagaraComponent>(Cube.Get(), TEXT("TrailFX")));
		TestFalse(TEXT("TrailFX inactive during the pop"), IsJuiceTrailActive(Cube.Get()));
		// task-028-03: the saved Trail blocked the miner (it climbed onto the cube); no pickup component may collide.
		TInlineComponentArray<UPrimitiveComponent*> Primitives(Cube.Get());
		for (const UPrimitiveComponent* Primitive : Primitives)
		{
			TestEqual(FString::Printf(TEXT("BP_PickupXP %s has no collision"), *Primitive->GetName()),
				static_cast<int32>(Primitive->GetCollisionEnabled()), static_cast<int32>(ECollisionEnabled::NoCollision));
			TestEqual(FString::Printf(TEXT("BP_PickupXP %s cannot be stepped on"), *Primitive->GetName()),
				static_cast<int32>(Primitive->CanCharacterStepUpOn.GetValue()), static_cast<int32>(ECB_No));
		}
	}

	// Pop, rest, then the stream: every cube's TrailFX is active once magnetized, until collected.
	TSet<const ARRPickup*> TrailSeen;
	int32 MagnetFramesWithoutTrail = 0;
	int32 Frame = 0;
	double FirstMagnetTime = -1.0;
	for (; Frame < 300 && CollectedAt.Num() < 3; ++Frame)
	{
		TestWorld.Tick();
		Sparkles.Poll(TestWorld.Get(), SparkleSystem);
		for (const TWeakObjectPtr<ARRPickup>& Cube : Cubes)
		{
			if (Cube.IsValid() && !Cube->IsActorBeingDestroyed() && Cube->IsMagnetized())
			{
				FirstMagnetTime = FirstMagnetTime < 0.0 ? TestWorld.Get()->GetTimeSeconds() : FirstMagnetTime;
				const bool bTrail = IsJuiceTrailActive(Cube.Get());
				MagnetFramesWithoutTrail += bTrail ? 0 : 1;
				if (bTrail)
				{
					TrailSeen.Add(Cube.Get());
				}
			}
		}
	}
	AddInfo(FString::Printf(TEXT("JUICE stream: magnet from %.3f s, %d collected by frame %d (%.3f s), %d trails seen, %d sparkles"), FirstMagnetTime,
		CollectedAt.Num(), Frame, TestWorld.Get()->GetTimeSeconds(), TrailSeen.Num(), Sparkles.Entries.Num()));
	TestEqual(TEXT("3 cubes collected"), CollectedAt.Num(), 3);
	TestTrue(TEXT("cubes rest before streaming (magnet after pop + rest)"), FirstMagnetTime >= 1.0);
	// Niagara never activates nor spawns when the application cannot render (UNiagaraComponent::Activate and
	// CreateNiagaraSystem check FApp::CanEverRender(), false under -nullrhi): the Niagara checks need a real RHI (-RenderOffScreen).
	const bool bNiagaraRuns = FApp::CanEverRender();
	if (!bNiagaraRuns)
	{
		AddInfo(TEXT("JUICE Niagara checks SKIPPED (TrailFX activation, NS_PickupSparkle count): FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen to check them."));
	}
	if (bNiagaraRuns)
	{
		TestEqual(TEXT("every cube's TrailFX was active while magnetized"), TrailSeen.Num(), 3);
		TestEqual(TEXT("TrailFX active on every magnetized frame"), MagnetFramesWithoutTrail, 0);
	}
	TestEqual(TEXT("XP +1 (chaser XP split over 3 cubes)"), JuiceTotalXP() - XPBefore, 1.f, 1e-3f);

	// One NS_PickupSparkle component per collect, at the collect location, short-lived (auto-destroy).
	if (bNiagaraRuns)
	{
		TestEqual(TEXT("one NS_PickupSparkle per collected cube"), Sparkles.Entries.Num(), 3);
	}
	for (const FVector& Collect : CollectedAt)
	{
		if (bNiagaraRuns)
		{
			const bool bSparkleThere = Sparkles.Entries.ContainsByPredicate([&Collect](const FJuiceNiagaraLog::FEntry& Entry)
			{
				return FVector::Dist(Entry.Location, Collect) < 1.f;
			});
			TestTrue(TEXT("NS_PickupSparkle at the collect location"), bSparkleThere);
		}
		TestTrue(TEXT("collect within 50 uu of the miner"), FVector::Dist2D(Collect, Miner->GetActorLocation()) < 60.f);
	}
	for (int32 Extra = 0; Extra < 120; ++Extra)
	{
		TestWorld.Tick();
	}
	int32 SparklesAlive = 0;
	for (const FJuiceNiagaraLog::FEntry& Entry : Sparkles.Entries)
	{
		SparklesAlive += Entry.Component.IsValid() && !Entry.Component->IsBeingDestroyed() && Entry.Component->IsActive() ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("JUICE sparkles: %d NS_PickupSparkle spawned, %d still active 2 s later"), Sparkles.Entries.Num(), SparklesAlive));
	TestEqual(TEXT("sparkles gone 2 s later"), SparklesAlive, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRJuiceKamikazeFlareTest, "RogueRigs.Integration.JuiceKamikazeFlaresBeforeExploding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRJuiceKamikazeFlareTest::RunTest(const FString& Parameters)
{
	using namespace RRJuiceTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRJuiceKamikazeFlare"));
	UClass* KamikazeClass = RRTestHarness::LoadTestClass<ARREnemy>(*this, JuiceKamikazePath);
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = KamikazeClass ? SpawnJuiceMiner(*this, TestWorld, Grid) : nullptr;
	if (!Miner)
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(JuiceMinerCell.X, JuiceMinerCell.Y);
	ARRKamikaze* Kamikaze = Cast<ARRKamikaze>(SpawnJuiceRobot(*this, TestWorld, KamikazeClass, MinerFloor + FVector(600.f, 0.f, 0.f)));
	if (!TestNotNull(TEXT("BP_Kamikaze is an ARRKamikaze"), Kamikaze))
	{
		return false;
	}
	const UPointLightComponent* CoreLight = FindJuiceComponent<UPointLightComponent>(Kamikaze, TEXT("CoreLight"));
	if (!TestNotNull(TEXT("BP_Kamikaze has CoreLight"), CoreLight))
	{
		return false;
	}
	TWeakObjectPtr<ARRKamikaze> Weak(Kamikaze);
	const UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	const float HealthBefore = MinerASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());

	float MaxBeforeFlare = 0.f;
	float LastIntensity = 0.f;
	float PeakIntensity = 0.f;
	bool bMonotonic = true;
	bool bTracksProgress = true;
	int32 FlareFrames = 0;
	double FlareStart = -1.0;
	bool bShownInGame = true;
	for (int32 Frame = 0; Frame < 400 && Weak.IsValid() && !Weak->HasDetonated(); ++Frame)
	{
		TestWorld.Tick();
		if (!Weak.IsValid() || Weak->HasDetonated())
		{
			break;
		}
		const float Intensity = CoreLight->Intensity;
		if (!Weak->IsFlaring())
		{
			MaxBeforeFlare = FMath::Max(MaxBeforeFlare, Intensity);
			continue;
		}
		FlareStart = FlareStart < 0.0 ? TestWorld.Get()->GetTimeSeconds() : FlareStart;
		++FlareFrames;
		bMonotonic &= Intensity >= LastIntensity;
		// The Blueprint tick reads the progress of this frame or the previous one (tick order): within one frame of ramp.
		const float Expected = Weak->GetFlareProgress() * JuiceCoreLightPeak;
		bTracksProgress &= FMath::Abs(Intensity - Expected) <= JuiceCoreLightPeak * (RRTestHarness::FrameSeconds / Weak->GetFlareDuration()) + 1.f;
		bShownInGame &= IsShownInGame(CoreLight);
		LastIntensity = Intensity;
		PeakIntensity = FMath::Max(PeakIntensity, Intensity);
	}
	if (!TestTrue(TEXT("kamikaze detonated by its fuse"), Weak.IsValid() && Weak->HasDetonated()))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("JUICE flare: started %.3f s, %d frames, peak CoreLight %.0f, before flare max %.0f, explosion queued %.3f s"),
		FlareStart, FlareFrames, PeakIntensity, MaxBeforeFlare, Kamikaze->GetExplosionQueuedTime()));
	TestEqual(TEXT("CoreLight off while chasing"), MaxBeforeFlare, 0.f);
	TestTrue(TEXT("flare lasts about 0.65 s (39 frames)"), FlareFrames >= 38 && FlareFrames <= 40);
	TestTrue(TEXT("CoreLight ramps up monotonically"), bMonotonic);
	TestTrue(TEXT("CoreLight = flare progress x 10000"), bTracksProgress);
	TestTrue(TEXT("CoreLight near its 10000 peak before the explosion"), PeakIntensity >= 0.9f * JuiceCoreLightPeak && PeakIntensity <= JuiceCoreLightPeak);
	TestTrue(TEXT("CoreLight renders in game during the flare"), bShownInGame);
	TestTrue(TEXT("explosion after the flare"), Kamikaze->GetExplosionQueuedTime() >= FlareStart + 0.65 - 1e-3);
	TestEqual(TEXT("CoreLight back to 0 after the flare (OnFlareEnd)"), CoreLight->Intensity, 0.f);
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("miner hit once by the explosion (100 -> 70)"), MinerASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()), HealthBefore - 30.f, 0.01f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRJuiceHealRingTest, "RogueRigs.Integration.JuiceHealRing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRJuiceHealRingTest::RunTest(const FString& Parameters)
{
	using namespace RRJuiceTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRJuiceHealRing"));
	UClass* HealClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, JuicePickupHealPath);
	UClass* RingClass = RRTestHarness::LoadTestClass<ARRExplosionFX>(*this, JuiceHealRingPath);
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = HealClass && RingClass ? SpawnJuiceMiner(*this, TestWorld, Grid) : nullptr;
	if (!Miner)
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(JuiceMinerCell.X, JuiceMinerCell.Y);
	FJuiceSpawnLog Rings(TestWorld.Get(), RingClass);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	Miner->OnPickupCollectedAt.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollectedAt);
	const TArray<FVector>& HealCollects = Recorder->GetCollectedPickupLocations();

	// A heal chunk dropped 120 uu away (inside the 300 uu pickup radius) pops, rests, streams in, is collected.
	ARRPickup* Heal = TestWorld.Spawn<ARRPickup>(HealClass, MinerFloor + FVector(120.f, 0.f, 0.f));
	if (!TestNotNull(TEXT("BP_PickupHeal spawned"), Heal))
	{
		return false;
	}
	TestTrue(TEXT("heal chunk lands within one 67 uu cell (PopMaxDistance)"), FVector::Dist2D(Heal->GetPopEnd(), Heal->GetPopStart()) <= 67.f + 0.5f);
	int32 Frame = 0;
	for (; Frame < 300 && HealCollects.IsEmpty(); ++Frame)
	{
		TestWorld.Tick();
	}
	if (!TestEqual(TEXT("heal collected once"), HealCollects.Num(), 1))
	{
		return false;
	}
	const double CollectTime = TestWorld.Get()->GetTimeSeconds();
	TestTrue(TEXT("the collected pickup is a Heal"), Recorder->GetCollectedPickups().Num() == 1 && Recorder->GetCollectedPickups()[0] == ERRPickupType::Heal);
	if (!TestEqual(TEXT("one BP_HealRing on the heal collect"), Rings.Entries.Num(), 1))
	{
		return false;
	}
	const FJuiceSpawnLog::FEntry Ring = Rings.Entries[0];
	TestTrue(TEXT("ring spawned on the collect frame"), FMath::IsNearlyEqual(Ring.Time, CollectTime, 1e-6));
	TestTrue(TEXT("ring at the collect location (the miner)"), FVector::Dist(Ring.Location, HealCollects[0]) < 1.f && FVector::Dist2D(Ring.Location, Miner->GetActorLocation()) < 60.f);
	const ARRExplosionFX* RingFX = Cast<ARRExplosionFX>(Ring.Actor.Get());
	const float RingRadius = RingFX ? RingFX->GetRadius() : -1.f;
	TestEqual(TEXT("ring radius 140 (BeginPlay SetRadius)"), RingRadius, 140.f, 0.01f);
	TestTrue(TEXT("ring mesh renders in game"), RingFX && IsShownInGame(RingFX->FindComponentByClass<UStaticMeshComponent>()));

	// Duration 0.6 s, then destroyed.
	int32 AliveFrames = 0;
	for (int32 Extra = 0; Extra < 60; ++Extra)
	{
		TestWorld.Tick();
		AliveFrames += Ring.Actor.IsValid() && !Ring.Actor->IsActorBeingDestroyed() ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("JUICE heal ring: collect at %.3f s after %d frames, ring alive %d frames, radius %.0f"), CollectTime, Frame, AliveFrames,
		RingRadius));
	TestTrue(TEXT("ring lives about 0.6 s (36 frames)"), AliveFrames >= 34 && AliveFrames <= 37);
	TestEqual(TEXT("still one ring"), Rings.Entries.Num(), 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
