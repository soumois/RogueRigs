// Rogue Rigs - automation tests of the pickup motion and the kamikaze flare (RogueRigs.Pickups.Motion*, RogueRigs.Kamikaze.FlareTiming, RogueRigs.DamageNumbers.CrowdStacking): task-027-01.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Debug/RRStressSubsystem.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShooter.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/RRPickup.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPickupMotionTests
{
	using RRTestHarness::FTestWorld;
	using RRTestHarness::FrameSeconds;

	/** task-027-01 values (C++ defaults of ARRPickup, used by the native ARRTestPickupXP). */
	constexpr float PopMin = 80.f;
	constexpr float PopMax = 220.f;
	constexpr float PopSeconds = 0.35f;
	constexpr float RestHeight = 30.f;
	constexpr float RestSeconds = 1.f;
	constexpr float MagnetStart = 400.f;
	constexpr float MagnetAccel = 4000.f;
	constexpr float MagnetMax = 2500.f;
	constexpr float CollectRange = 50.f;

	/** Open floor cell of the legacy arena_m1 layout (as the kamikaze fuse test). */
	const FIntPoint MotionMinerCell(40, 40);

	/** Spawns a native XP pickup at Location; a yaw >= 0 forces its ejection direction. */
	ARRPickup* SpawnMotionPickup(const FTestWorld& TestWorld, const FVector& Location, float YawDegrees = -1.f)
	{
		const FTransform Transform(Location);
		ARRPickup* Pickup = TestWorld.Get()->SpawnActorDeferred<ARRPickup>(ARRTestPickupXP::StaticClass(), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Pickup)
		{
			if (YawDegrees >= 0.f)
			{
				Pickup->SetPopYaw(YawDegrees);
			}
			Pickup->FinishSpawning(Transform);
		}
		return Pickup;
	}

	/** Spawns a native robot of class T with its feet at FloorLocation (XP drop ARRTestPickupXP). */
	template <typename T>
	T* SpawnMotionRobot(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
	{
		const float HalfHeight = GetDefault<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		T* Robot = TestWorld.Get()->SpawnActorDeferred<T>(T::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("robot spawned"), Robot))
		{
			RRTestAccess::FEnemyAccess::XPPickupClassOf(Robot) = ARRTestPickupXP::StaticClass();
			Robot->FinishSpawning(SpawnTransform);
		}
		return Robot;
	}

	/** Spawns BP_Miner on MotionMinerCell, possessed by a player controller (player 0 pawn), weapons stopped. */
	ARRMiner* SpawnMotionMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, ARRArenaGrid*& OutGrid)
	{
		OutGrid = RRTestHarness::SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = OutGrid ? RRTestHarness::SpawnMiner(Test, TestWorld, OutGrid, MotionMinerCell) : nullptr;
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

	/** Pickups whose pop started within 1 uu (2D) of Location. */
	TArray<ARRPickup*> PickupsDroppedAt(UWorld* World, const FVector& Location)
	{
		TArray<ARRPickup*> Result;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			// task-092-01: idle pool pickups are not live pickups.
			if (URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				continue;
			}
			if (IsValid(*It) && !It->IsActorBeingDestroyed() && FVector::Dist2D(It->GetPopStart(), Location) < 1.f)
			{
				Result.Add(*It);
			}
		}
		return Result;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupMotionPopTest, "RogueRigs.Pickups.MotionPopArc",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupMotionPopTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupMotionTests;

	FTestWorld TestWorld(TEXT("RRPickupMotionPop"));
	URRPickupSubsystem* Subsystem = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!TestNotNull(TEXT("pickup subsystem exists in a game world"), Subsystem))
	{
		return false;
	}

	// Eight cubes with forced directions and twenty random ones, no miner (nothing is attracted).
	const FVector Origin(0.f, 0.f, 100.f);
	TArray<ARRPickup*> Forced;
	for (int32 Index = 0; Index < 8; ++Index)
	{
		Forced.Add(SpawnMotionPickup(TestWorld, Origin, 45.f * static_cast<float>(Index)));
	}
	TArray<ARRPickup*> Random;
	for (int32 Index = 0; Index < 20; ++Index)
	{
		Random.Add(SpawnMotionPickup(TestWorld, Origin));
	}
	if (Forced.Contains(nullptr) || Random.Contains(nullptr))
	{
		AddError(TEXT("pickups not spawned"));
		return false;
	}
	TestEqual(TEXT("28 pickups registered with the subsystem"), Subsystem->GetPickupCount(), 28);
	TestFalse(TEXT("no actor tick (moved by the subsystem)"), Forced[0]->IsActorTickEnabled());

	// Track the first forced cube: arc peak, landing then bounce.
	float PeakZ = -1.f;
	float LowAfterArc = TNumericLimits<float>::Max();
	float BouncePeak = -1.f;
	int32 RestFrame = INDEX_NONE;
	const int32 ArcEndFrame = FMath::FloorToInt(0.75f * PopSeconds / FrameSeconds);
	for (int32 Frame = 1; Frame <= 40; ++Frame)
	{
		TestWorld.Tick();
		const float Z = static_cast<float>(Forced[0]->GetActorLocation().Z - Origin.Z);
		if (Frame <= ArcEndFrame)
		{
			PeakZ = FMath::Max(PeakZ, Z);
		}
		else if (RestFrame == INDEX_NONE)
		{
			LowAfterArc = FMath::Min(LowAfterArc, Z);
			BouncePeak = FMath::Max(BouncePeak, Z);
		}
		if (RestFrame == INDEX_NONE && Forced[0]->GetPhase() == ERRPickupPhase::Rest)
		{
			RestFrame = Frame;
		}
	}
	AddInfo(FString::Printf(TEXT("PICKUPMOTION pop: arc peak %.1f uu, low after arc %.1f, bounce peak %.1f, rest at frame %d (%.3f s)"),
		PeakZ, LowAfterArc, BouncePeak, RestFrame, RestFrame * FrameSeconds));
	TestTrue(TEXT("rests after the 0.35 s pop (within one frame)"), RestFrame != INDEX_NONE && FMath::Abs(RestFrame * FrameSeconds - PopSeconds) <= FrameSeconds + 1e-3f);
	TestTrue(TEXT("main arc peaks well above the rest height"), PeakZ > RestHeight + 40.f);
	TestTrue(TEXT("small bounce after landing"), BouncePeak > RestHeight + 3.f && BouncePeak < PeakZ);

	for (int32 Index = 0; Index < Forced.Num(); ++Index)
	{
		const FVector Offset = Forced[Index]->GetActorLocation() - Origin;
		const float Distance = static_cast<float>(Offset.Size2D());
		const FVector Expected = FRotator(0.f, 45.f * static_cast<float>(Index), 0.f).Vector();
		TestTrue(FString::Printf(TEXT("forced cube %d rests 80-220 uu out (%.1f)"), Index, Distance), Distance >= PopMin - 0.5f && Distance <= PopMax + 0.5f);
		TestTrue(FString::Printf(TEXT("forced cube %d along its yaw"), Index), FVector::DotProduct(Offset.GetSafeNormal2D(), Expected) > 0.999);
		TestEqual(FString::Printf(TEXT("forced cube %d at the rest height"), Index), static_cast<float>(Offset.Z), RestHeight, 0.01f);
	}
	FVector Sum = FVector::ZeroVector;
	for (const ARRPickup* Pickup : Random)
	{
		const FVector Offset = Pickup->GetActorLocation() - Origin;
		const float Distance = static_cast<float>(Offset.Size2D());
		TestTrue(TEXT("random cube rests 80-220 uu out"), Distance >= PopMin - 0.5f && Distance <= PopMax + 0.5f);
		Sum += Offset.GetSafeNormal2D();
	}
	AddInfo(FString::Printf(TEXT("random cubes: mean direction length %.2f"), Sum.Size() / Random.Num()));
	TestTrue(TEXT("random directions spread (mean direction well under 1)"), Sum.Size() / Random.Num() < 0.7);

	// No miner: they rest in place.
	const FVector Rest = Forced[3]->GetActorLocation();
	for (int32 Frame = 0; Frame < 120; ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("resting cube does not move"), Forced[3]->GetActorLocation().Equals(Rest, 0.01));
	TestEqual(TEXT("phase Rest"), Forced[3]->GetPhase(), ERRPickupPhase::Rest);
	TestEqual(TEXT("none magnetized"), Subsystem->GetMagnetizedCount(), 0);

	Forced[0]->Destroy();
	TestEqual(TEXT("destroyed pickup unregistered"), Subsystem->GetPickupCount(), 27);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupMotionMagnetTest, "RogueRigs.Pickups.MotionMagnetStream",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupMotionMagnetTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupMotionTests;

	FTestWorld TestWorld(TEXT("RRPickupMotionMagnet"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = SpawnMotionMiner(*this, TestWorld, Grid);
	if (!Miner)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	Miner->OnPickupCollectedAt.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollectedAt);
	URRPickupSubsystem* Subsystem = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!TestNotNull(TEXT("pickup subsystem"), Subsystem))
	{
		return false;
	}

	// 1. Acceleration and top speed: a radius of 3000 uu and one cube resting about 2000 uu away along +X.
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetPickupRadiusAttribute(), 3000.f);
	const FVector MinerStart = Miner->GetActorLocation();
	ARRPickup* Far = SpawnMotionPickup(TestWorld, MinerStart + FVector(1850.f, 0.f, -60.f), 0.f);
	if (!TestNotNull(TEXT("far cube"), Far))
	{
		return false;
	}
	TWeakObjectPtr<ARRPickup> FarWeak(Far);
	int32 MagnetFrame = INDEX_NONE;
	TArray<float> Speeds;
	FVector Previous = Far->GetActorLocation();
	for (int32 Frame = 1; Frame <= 300 && FarWeak.IsValid() && !FarWeak->IsActorBeingDestroyed(); ++Frame)
	{
		TestWorld.Tick();
		if (!FarWeak.IsValid() || FarWeak->IsActorBeingDestroyed())
		{
			break;
		}
		if (FarWeak->IsMagnetized())
		{
			if (MagnetFrame == INDEX_NONE)
			{
				MagnetFrame = Frame;
			}
			Speeds.Add(static_cast<float>(FVector::Dist(FarWeak->GetActorLocation(), Previous) / FrameSeconds));
		}
		Previous = FarWeak->GetActorLocation();
	}
	const float FirstSpeed = Speeds.Num() > 0 ? Speeds[0] : 0.f;
	float TopSpeed = 0.f;
	bool bMonotonic = true;
	for (int32 Index = 1; Index < Speeds.Num(); ++Index)
	{
		TopSpeed = FMath::Max(TopSpeed, Speeds[Index]);
		bMonotonic &= Speeds[Index] >= Speeds[Index - 1] - 1.f;
	}
	AddInfo(FString::Printf(TEXT("PICKUPMOTION magnet: latched at frame %d (%.3f s), %d flight frames, first speed %.1f uu/s, top %.1f uu/s"),
		MagnetFrame, MagnetFrame * FrameSeconds, Speeds.Num(), FirstSpeed, TopSpeed));
	TestTrue(TEXT("latched after pop + rest (1.35 s, within two frames)"), MagnetFrame != INDEX_NONE
		&& MagnetFrame * FrameSeconds >= PopSeconds + RestSeconds - 1e-3f && MagnetFrame * FrameSeconds <= PopSeconds + RestSeconds + 2.f * FrameSeconds + 1e-3f);
	TestEqual(TEXT("first magnet frame: 400 uu/s + one frame of 4000 uu/s2"), FirstSpeed, MagnetStart + MagnetAccel * FrameSeconds, 2.f);
	TestTrue(TEXT("speed never decreases in flight (except the last clipped step)"), bMonotonic || Speeds.Num() < 3);
	TestTrue(TEXT("reaches the 2500 uu/s cap and never exceeds it"), TopSpeed >= MagnetMax - 2.f && TopSpeed <= MagnetMax + 2.f);
	TestEqual(TEXT("far cube collected"), Recorder->GetCollectedPickups().Num(), 1);

	// 2. Stream: twelve cubes in a ring at the miner (default radius 300): they rest 80-220 uu out, then all stream in.
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetPickupRadiusAttribute(), 300.f);
	const FVector Feet = Miner->GetActorLocation() - FVector(0.f, 0.f, Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	for (int32 Index = 0; Index < 12; ++Index)
	{
		SpawnMotionPickup(TestWorld, Feet, 30.f * static_cast<float>(Index));
	}
	int32 MaxMagnetized = 0;
	int32 Frames = 0;
	for (; Frames < 240 && Recorder->GetCollectedPickups().Num() < 13; ++Frames)
	{
		TestWorld.Tick();
		MaxMagnetized = FMath::Max(MaxMagnetized, Subsystem->GetMagnetizedCount());
	}
	AddInfo(FString::Printf(TEXT("PICKUPMOTION stream: 12 cubes collected in %d frames, up to %d magnetized at once"), Frames, MaxMagnetized));
	TestEqual(TEXT("all 13 collected"), Recorder->GetCollectedPickups().Num(), 13);
	TestTrue(TEXT("several cubes in flight together (stream)"), MaxMagnetized >= 6);
	TestEqual(TEXT("one located event per collection"), Recorder->GetCollectedPickupLocations().Num(), Recorder->GetCollectedPickups().Num());
	for (const FVector& Location : Recorder->GetCollectedPickupLocations())
	{
		TestTrue(TEXT("collected within 50 uu of the miner"), FVector::Dist(Location, Miner->GetActorLocation()) <= CollectRange + 0.1f);
	}
	TestEqual(TEXT("no pickup left"), Subsystem->GetPickupCount(), 0);

	// 3. Latched: once magnetized, a cube follows the miner even after it leaves the radius.
	ARRPickup* Latch = SpawnMotionPickup(TestWorld, Feet, 0.f);
	TWeakObjectPtr<ARRPickup> LatchWeak(Latch);
	bool bMoved = false;
	for (int32 Frame = 0; Frame < 240 && LatchWeak.IsValid() && !LatchWeak->IsActorBeingDestroyed(); ++Frame)
	{
		TestWorld.Tick();
		if (!bMoved && LatchWeak.IsValid() && LatchWeak->IsMagnetized())
		{
			Miner->SetActorLocation(Miner->GetActorLocation() + FVector(0.f, 0.f, 0.f) - FVector(800.f, 0.f, 0.f));
			bMoved = true;
		}
	}
	TestTrue(TEXT("miner moved away after the latch"), bMoved);
	TestEqual(TEXT("latched cube still collected (14)"), Recorder->GetCollectedPickups().Num(), 14);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupMotionSplitTest, "RogueRigs.Pickups.MotionXPSplit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupMotionSplitTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupMotionTests;

	FTestWorld TestWorld(TEXT("RRPickupMotionSplit"));
	ARRArenaGrid* Grid = RRTestHarness::SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	// Four robots 1200 uu apart (the kamikaze blast, 312 uu, reaches none of the others), killed before any tick.
	const FVector Base = Grid->GetCellCenter(MotionMinerCell.X, MotionMinerCell.Y);
	struct FCase
	{
		ARREnemy* Robot = nullptr;
		const TCHAR* Name = nullptr;
		int32 Cubes = 0;
		FVector Feet = FVector::ZeroVector;
	};
	TArray<FCase> Cases;
	Cases.Add({ SpawnMotionRobot<ARRChaser>(*this, TestWorld, Base), TEXT("chaser"), 3 });
	Cases.Add({ SpawnMotionRobot<ARRShooter>(*this, TestWorld, Base + FVector(0.f, 1200.f, 0.f)), TEXT("shooter"), 6 });
	Cases.Add({ SpawnMotionRobot<ARRHeavy>(*this, TestWorld, Base + FVector(0.f, -1200.f, 0.f)), TEXT("heavy"), 12 });
	Cases.Add({ SpawnMotionRobot<ARRKamikaze>(*this, TestWorld, Base + FVector(-1200.f, 0.f, 0.f)), TEXT("kamikaze"), 4 });
	for (FCase& Case : Cases)
	{
		if (!Case.Robot)
		{
			return false;
		}
		Case.Feet = Case.Robot->GetActorLocation() - FVector(0.f, 0.f, Case.Robot->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		TestTrue(FString::Printf(TEXT("%s killed"), Case.Name), URRStressSubsystem::KillEnemy(Case.Robot));
	}
	// Death shrink 0.15 s, then the burst.
	for (int32 Frame = 0; Frame < 15; ++Frame)
	{
		TestWorld.Tick();
	}

	float TotalXP = 0.f;
	for (const FCase& Case : Cases)
	{
		const TArray<ARRPickup*> Cubes = PickupsDroppedAt(TestWorld.Get(), Case.Feet);
		const int32 XPValue = Case.Robot->GetStats().XPValue;
		float Sum = 0.f;
		TArray<float> Yaws;
		for (const ARRPickup* Cube : Cubes)
		{
			Sum += Cube->GetXPAmount();
			TestEqual(FString::Printf(TEXT("%s cube worth XPValue / cubes"), Case.Name), Cube->GetXPAmount(), static_cast<float>(XPValue) / Case.Cubes, 1e-5f);
			Yaws.Add(static_cast<float>((Cube->GetPopEnd() - Cube->GetPopStart()).Rotation().Yaw));
		}
		Yaws.Sort();
		float MinGap = 360.f;
		for (int32 Index = 0; Index < Yaws.Num(); ++Index)
		{
			const float Next = Index + 1 < Yaws.Num() ? Yaws[Index + 1] : Yaws[0] + 360.f;
			MinGap = FMath::Min(MinGap, Next - Yaws[Index]);
		}
		AddInfo(FString::Printf(TEXT("PICKUPMOTION split: %s XPValue %d -> %d cubes, sum %.6f, smallest direction gap %.1f deg"),
			Case.Name, XPValue, Cubes.Num(), Sum, MinGap));
		TestEqual(FString::Printf(TEXT("%s bursts %d cubes"), Case.Name, Case.Cubes), Cubes.Num(), Case.Cubes);
		TestEqual(FString::Printf(TEXT("%s cube values add up to its XP"), Case.Name), Sum, static_cast<float>(XPValue), 1e-4f);
		TestTrue(FString::Printf(TEXT("%s cubes spread around the body"), Case.Name), MinGap >= 0.5f * 360.f / Case.Cubes - 0.5f);
		TotalXP += Sum;
	}
	TestEqual(TEXT("XP of chaser + shooter + heavy + kamikaze (1 + 2 + 5 + 1)"), TotalXP, 9.f, 1e-4f);

	// Fractional cubes reach the level threshold exactly: seven cubes of a seventh of the level 1 need (XPCurve.csv) = level 1 -> 2, XP back to 0.
	URRProgressionComponent* Progression = NewObject<URRProgressionComponent>(Grid);
	Progression->RegisterComponent();
	const int32 LevelBefore = Progression->GetLevel();
	const float LevelOneNeed = Progression->GetXPToNextLevel();
	for (int32 Index = 0; Index < 7; ++Index)
	{
		Progression->AddXP(LevelOneNeed / 7.f);
	}
	AddInfo(FString::Printf(TEXT("progression: level %d -> %d, XP %.6f"), LevelBefore, Progression->GetLevel(), Progression->GetXP()));
	TestEqual(TEXT("seven need/7 cubes give one level"), Progression->GetLevel(), LevelBefore + 1);
	TestEqual(TEXT("no XP left over"), Progression->GetXP(), 0.f, 1e-3f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRKamikazeFlareTest, "RogueRigs.Kamikaze.FlareTiming",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRKamikazeFlareTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupMotionTests;

	FTestWorld TestWorld(TEXT("RRKamikazeFlare"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = SpawnMotionMiner(*this, TestWorld, Grid);
	if (!Miner)
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(MotionMinerCell.X, MotionMinerCell.Y);

	// 1. Proximity: Chase -> flare 0.65 s -> explosion.
	ARRKamikaze* Kamikaze = SpawnMotionRobot<ARRKamikaze>(*this, TestWorld, MinerFloor + FVector(600.f, 0.f, 0.f));
	if (!Kamikaze)
	{
		return false;
	}
	TWeakObjectPtr<ARRKamikaze> Weak(Kamikaze);
	int32 FlareStarts = 0;
	double FlareStartTime = -1.0;
	Kamikaze->OnFlareStarted.AddLambda([&FlareStarts, &FlareStartTime](ARRKamikaze* Source)
	{
		++FlareStarts;
		FlareStartTime = Source->GetWorld()->GetTimeSeconds();
	});
	TestEqual(TEXT("flare 0.65 s (CSV WindUp)"), Kamikaze->GetFlareDuration(), 0.65f, 1e-4f);
	TestFalse(TEXT("not flaring while chasing"), Kamikaze->IsFlaring());
	TestEqual(TEXT("progress 0 while chasing"), Kamikaze->GetFlareProgress(), 0.f);

	float LastProgress = 0.f;
	bool bProgressMonotonic = true;
	int32 FlaringFrames = 0;
	for (int32 Frame = 0; Frame < 300 && Weak.IsValid() && !Weak->HasDetonated(); ++Frame)
	{
		TestWorld.Tick();
		if (Weak.IsValid() && Weak->IsFlaring())
		{
			const float Progress = Weak->GetFlareProgress();
			bProgressMonotonic &= Progress >= LastProgress && Progress <= 1.f;
			LastProgress = Progress;
			++FlaringFrames;
		}
	}
	if (!TestTrue(TEXT("kamikaze detonated"), Weak.IsValid() && Weak->HasDetonated()))
	{
		return false;
	}
	const double FlareLength = Kamikaze->GetExplosionQueuedTime() - FlareStartTime;
	AddInfo(FString::Printf(TEXT("KAMIKAZEFLARE proximity: flare started at %.3f s (arm %.3f s), explosion %.3f s later, %d flaring frames, last progress %.3f"),
		FlareStartTime, Kamikaze->GetArmTime(), FlareLength, FlaringFrames, LastProgress));
	TestEqual(TEXT("one flare start event"), FlareStarts, 1);
	TestEqual(TEXT("flare starts on arming"), FlareStartTime, Kamikaze->GetArmTime(), 1e-6);
	TestTrue(TEXT("explosion 0.65 s after the flare start (within one frame)"), FlareLength >= 0.65 - 1e-3 && FlareLength <= 0.65 + FrameSeconds + 1e-3);
	TestTrue(TEXT("flare progress rises monotonically"), bProgressMonotonic);
	TestTrue(TEXT("flare progress near 1 just before the explosion"), LastProgress >= 0.95f);
	TestEqual(TEXT("progress 1 after the fuse explosion"), Kamikaze->GetFlareProgress(), 1.f);
	TestFalse(TEXT("not flaring once dead"), Kamikaze->IsFlaring());

	// 2. Killed while chasing: instant explosion, no flare.
	ARRKamikaze* Second = SpawnMotionRobot<ARRKamikaze>(*this, TestWorld, MinerFloor + FVector(-1500.f, 0.f, 0.f));
	if (!Second)
	{
		return false;
	}
	int32 SecondFlares = 0;
	Second->OnFlareStarted.AddLambda([&SecondFlares](ARRKamikaze*) { ++SecondFlares; });
	TestWorld.Tick();
	const double KillTime = TestWorld.Get()->GetTimeSeconds();
	TestTrue(TEXT("second kamikaze killed"), URRStressSubsystem::KillEnemy(Second));
	AddInfo(FString::Printf(TEXT("KAMIKAZEFLARE death: killed at %.3f s, explosion queued at %.3f s"), KillTime, Second->GetExplosionQueuedTime()));
	TestEqual(TEXT("death explosion queued at once"), Second->GetExplosionQueuedTime(), KillTime, 1e-6);
	TestEqual(TEXT("no flare on a death explosion"), SecondFlares, 0);
	TestEqual(TEXT("death: flare progress 0"), Second->GetFlareProgress(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDamageNumberStackingTest, "RogueRigs.DamageNumbers.CrowdStacking",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDamageNumberStackingTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupMotionTests;

	FTestWorld TestWorld(TEXT("RRDamageNumberStacking"));
	URRDamageNumberSubsystem* Numbers = TestWorld.Get()->GetSubsystem<URRDamageNumberSubsystem>();
	if (!TestNotNull(TEXT("damage number subsystem"), Numbers))
	{
		return false;
	}
	TArray<FRRDamageNumber> Pushed;
	const FDelegateHandle Handle = Numbers->OnNumbersPushed.AddLambda([&Pushed](const TArray<FRRDamageNumber>& Batch) { Pushed.Append(Batch); });

	// Three robots of a dense crowd hit together (about 30 uu apart: their numbers would overlap), one far robot.
	TArray<AActor*> Targets;
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Targets.Add(TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector));
	}
	const FVector Crowd(0.f, 0.f, 150.f);
	Numbers->AddHit(Targets[0], 35.f, true, Crowd);
	Numbers->AddHit(Targets[1], 20.f, false, Crowd + FVector(30.f, 0.f, 0.f));
	Numbers->AddHit(Targets[2], 25.f, false, Crowd + FVector(0.f, 30.f, 0.f));
	const FVector FarLocation(2000.f, 2000.f, 150.f);
	Numbers->AddHit(Targets[3], 10.f, false, FarLocation);
	for (int32 Frame = 0; Frame < 20; ++Frame)
	{
		TestWorld.Tick();
	}
	Numbers->OnNumbersPushed.Remove(Handle);
	if (!TestEqual(TEXT("four numbers pushed"), Pushed.Num(), 4))
	{
		return false;
	}

	// Screen axes of the defaults: right = +Y (DigitDirection), up = camera up at pitch -50 (StackDirection).
	const FVector Right(0.f, 1.f, 0.f);
	const FVector Up = FVector(0.766f, 0.f, 0.643f).GetSafeNormal();
	auto Footprint = [](const FRRDamageNumber& Number)
	{
		const int32 Digits = FString::FromInt(FMath::Max(FMath::RoundToInt(Number.Value), 1)).Len();
		return FVector2D(0.62f * Number.Size * Digits, Number.Size);
	};
	int32 Overlaps = 0;
	for (int32 A = 0; A < 3; ++A)
	{
		for (int32 B = A + 1; B < 3; ++B)
		{
			const FVector Delta = Pushed[B].Location - Pushed[A].Location;
			const FVector2D SizeA = Footprint(Pushed[A]);
			const FVector2D SizeB = Footprint(Pushed[B]);
			const bool bOverlap = FMath::Abs(FVector::DotProduct(Delta, Right)) < 0.5 * (SizeA.X + SizeB.X) - 0.5
				&& FMath::Abs(FVector::DotProduct(Delta, Up)) < 0.5 * (SizeA.Y + SizeB.Y) - 0.5;
			AddInfo(FString::Printf(TEXT("DAMAGENUMBERS stack: %.0f vs %.0f: right %.1f, up %.1f"), Pushed[A].Value, Pushed[B].Value,
				FVector::DotProduct(Delta, Right), FVector::DotProduct(Delta, Up)));
			Overlaps += bOverlap ? 1 : 0;
		}
	}
	TestEqual(TEXT("no two crowd numbers overlap on screen"), Overlaps, 0);
	const int32 FarIndex = Pushed.IndexOfByPredicate([](const FRRDamageNumber& Number) { return FMath::IsNearlyEqual(Number.Value, 10.f); });
	TestTrue(TEXT("far number not stacked (at its hit height)"), FarIndex != INDEX_NONE && FMath::IsNearlyEqual(Pushed[FarIndex].Location.Z, FarLocation.Z, 0.01));
	const int32 CritIndex = Pushed.IndexOfByPredicate([](const FRRDamageNumber& Number) { return Number.bCritical; });
	TestTrue(TEXT("crit lighter orange (1, 0.7, 0.35)"), CritIndex != INDEX_NONE && Pushed[CritIndex].Color.Equals(FLinearColor(1.f, 0.7f, 0.35f)));
	TestTrue(TEXT("first number (opened first) not stacked"), FMath::IsNearlyEqual(Pushed[0].Location.Z, Crowd.Z, 0.01));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
