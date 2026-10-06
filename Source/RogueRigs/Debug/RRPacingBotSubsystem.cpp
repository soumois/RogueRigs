// Rogue Rigs - XP pacing bot: a scripted -game run (real director, real pickups) that aims at the nearest robot, walks to resting XP cubes, picks the first card and logs the time of each level.

#include "Debug/RRPacingBotSubsystem.h"

#include "Debug/RRCheats.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/SpringArmComponent.h"
#include "HAL/IConsoleManager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"

namespace RRPacingBot
{
	TAutoConsoleVariable<float> CVarCollectRange(TEXT("rr.Pacing.CollectRange"), 2500.f,
		TEXT("Pacing bot: farthest XP cube (uu) the miner walks to; beyond it the miner stands still."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarCollectRadiusFraction(TEXT("rr.Pacing.CollectRadiusFraction"), 0.8f,
		TEXT("Pacing bot: a cube closer than this fraction of the pickup radius is left to the magnet."), ECVF_Cheat);

	/** Label of the log lines and of the cheat calls. */
	const TCHAR* const Source = TEXT("rr.Pacing.Run");

	void HandleRun(const TArray<FString>& Args, UWorld* World)
	{
		const float Seconds = Args.IsValidIndex(0) ? FCString::Atof(*Args[0]) : 200.f;
		const int32 Seed = Args.IsValidIndex(1) ? FCString::Atoi(*Args[1]) : 1;
		const bool bQuit = !Args.IsValidIndex(2) || FCString::Atoi(*Args[2]) != 0;
		const bool bCollect = !Args.IsValidIndex(3) || FCString::Atoi(*Args[3]) != 0;
		URRPacingBotSubsystem* Bot = World ? World->GetSubsystem<URRPacingBotSubsystem>() : nullptr;
		if (!Bot || !Bot->StartRun(Seconds > 0.f ? Seconds : 200.f, Seed, bQuit, bCollect))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: refused (no game world, miner or spawner, or a run is active)."), Source);
		}
	}

	FAutoConsoleCommandWithWorldAndArgs CmdRun(TEXT("rr.Pacing.Run"),
		TEXT("rr.Pacing.Run [Seconds=200] [Seed=1] [Quit=1] [Collect=1]: XP pacing bot (invulnerable, aims at the nearest robot, walks to XP cubes, picks card 0); level times in the log and Saved/Pacing."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleRun), ECVF_Cheat);
}

bool URRPacingBotSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Debug tooling never exists in shipping builds (same gate as URRCheatSubsystem).
#if UE_BUILD_SHIPPING
	return false;
#else
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
#endif
}

TStatId URRPacingBotSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRPacingBotSubsystem, STATGROUP_Tickables);
}

ARREnemySpawner* URRPacingBotSubsystem::FindSpawner() const
{
	TActorIterator<ARREnemySpawner> It(GetWorld());
	return It ? *It : nullptr;
}

bool URRPacingBotSubsystem::StartRun(float DurationSeconds, int32 Seed, bool bQuit, bool bCollect)
{
	UWorld* World = GetWorld();
	ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
	ARREnemySpawner* FoundSpawner = FindSpawner();
	if (bRunning || !Miner || !Miner->GetProgression() || !FoundSpawner)
	{
		return false;
	}

	Spawner = FoundSpawner;
	FoundSpawner->SeedRandomStream(Seed);
	FMath::RandInit(Seed);
	FMath::SRandInit(Seed);
	RRCheats::SetInvulnerable(World, true, RRPacingBot::Source);

	bRunning = true;
	bQuitWhenDone = bQuit;
	bCollectCubes = bCollect;
	RunSeed = Seed;
	StartSeconds = FoundSpawner->GetRunTime();
	EndSeconds = StartSeconds + DurationSeconds;
	LastLevel = Miner->GetProgression()->GetLevel();
	CardsPicked = 0;
	LevelTimes.Reset();
	UE_LOG(LogRogueRigs, Log, TEXT("Pacing: run started, seed %d, %.0f s, collect %d, spawner %s (enabled %d, clock %.2f s), miner level %d, pickup radius %.0f uu."),
		Seed, DurationSeconds, bCollect ? 1 : 0, *FoundSpawner->GetName(), FoundSpawner->IsSpawningEnabled() ? 1 : 0, StartSeconds,
		LastLevel, Miner->GetPickupRadius());
	return true;
}

void URRPacingBotSubsystem::Tick(float DeltaTime)
{
	if (!bRunning)
	{
		return;
	}
	UWorld* World = GetWorld();
	ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
	ARREnemySpawner* RunSpawner = Spawner.Get();
	if (!Miner || !Miner->GetProgression() || !RunSpawner)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Pacing: miner or spawner lost, run ended."));
		FinishRun();
		return;
	}

	const float Now = RunSpawner->GetRunTime();
	const int32 Level = Miner->GetProgression()->GetLevel();
	if (Level > LastLevel)
	{
		int32 Robots = 0;
		int32 Cubes = 0;
		CountWorld(Robots, Cubes);
		for (int32 Reached = LastLevel + 1; Reached <= Level; ++Reached)
		{
			FRRPacingLevelTime& Entry = LevelTimes.AddDefaulted_GetRef();
			Entry.Level = Reached;
			Entry.Seconds = Now;
			Entry.RobotsAlive = Robots;
			Entry.CubesLeft = Cubes;
			UE_LOG(LogRogueRigs, Log, TEXT("Pacing: level %d at %.1f s (%d:%04.1f), %d robots alive, %d XP cubes lying."),
				Reached, Now, FMath::FloorToInt32(Now / 60.f), FMath::Fmod(Now, 60.f), Robots, Cubes);
		}
		LastLevel = Level;
	}

	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	if (World->IsPaused())
	{
		// Level-up screen: pick the first card; nothing else moves while paused.
		if (LevelUp && LevelUp->IsAwaitingChoice() && RRCheats::ChooseUpgrade(World, 0, RRPacingBot::Source))
		{
			++CardsPicked;
		}
		return;
	}

	if (Now >= EndSeconds)
	{
		FinishRun();
		return;
	}

	AimAtNearestRobot(Miner);
	if (bCollectCubes)
	{
		WalkToNearestCube(Miner);
	}
}

void URRPacingBotSubsystem::AimAtNearestRobot(ARRMiner* Miner) const
{
	const FVector Origin = Miner->GetActorLocation();
	const ARREnemy* Nearest = nullptr;
	double NearestSquared = TNumericLimits<double>::Max();
	for (TActorIterator<ARREnemy> It(GetWorld()); It; ++It)
	{
		if (It->IsDead())
		{
			continue;
		}
		const double DistanceSquared = FVector::DistSquared2D(Origin, It->GetActorLocation());
		if (DistanceSquared < NearestSquared)
		{
			NearestSquared = DistanceSquared;
			Nearest = *It;
		}
	}
	const USpringArmComponent* Arm = Miner->FindComponentByClass<USpringArmComponent>();
	if (!Nearest || !Arm)
	{
		return;
	}

	// ApplyStickAimInput takes screen axes (X right, Y up through the camera yaw): project the world direction on them.
	const FVector Direction = (Nearest->GetActorLocation() - Origin).GetSafeNormal2D();
	const FRotator CameraYaw(0.f, Arm->GetComponentRotation().Yaw, 0.f);
	const FVector2D Stick(FVector::DotProduct(Direction, CameraYaw.RotateVector(FVector::RightVector)),
		FVector::DotProduct(Direction, CameraYaw.RotateVector(FVector::ForwardVector)));
	Miner->ApplyStickAimInput(Stick);
}

void URRPacingBotSubsystem::WalkToNearestCube(ARRMiner* Miner) const
{
	const FVector Origin = Miner->GetActorLocation();
	const double MinSquared = FMath::Square(Miner->GetPickupRadius() * RRPacingBot::CVarCollectRadiusFraction.GetValueOnGameThread());
	double NearestSquared = FMath::Square(static_cast<double>(RRPacingBot::CVarCollectRange.GetValueOnGameThread()));
	const ARRPickup* Nearest = nullptr;
	for (TActorIterator<ARRPickup> It(GetWorld()); It; ++It)
	{
		if (It->GetPickupType() != ERRPickupType::XP || It->GetPhase() == ERRPickupPhase::Magnet || It->GetPhase() == ERRPickupPhase::Collected)
		{
			continue;
		}
		const double DistanceSquared = FVector::DistSquared2D(Origin, It->GetActorLocation());
		if (DistanceSquared > MinSquared && DistanceSquared < NearestSquared)
		{
			NearestSquared = DistanceSquared;
			Nearest = *It;
		}
	}
	const USpringArmComponent* Arm = Miner->FindComponentByClass<USpringArmComponent>();
	if (!Nearest || !Arm)
	{
		return;
	}

	const FVector Direction = (Nearest->GetActorLocation() - Origin).GetSafeNormal2D();
	const FRotator CameraYaw(0.f, Arm->GetComponentRotation().Yaw, 0.f);
	Miner->ApplyMoveInput(FVector2D(FVector::DotProduct(Direction, CameraYaw.RotateVector(FVector::RightVector)),
		FVector::DotProduct(Direction, CameraYaw.RotateVector(FVector::ForwardVector))));
}

void URRPacingBotSubsystem::CountWorld(int32& OutRobots, int32& OutCubes) const
{
	OutRobots = 0;
	OutCubes = 0;
	for (TActorIterator<ARREnemy> It(GetWorld()); It; ++It)
	{
		OutRobots += It->IsDead() ? 0 : 1;
	}
	for (TActorIterator<ARRPickup> It(GetWorld()); It; ++It)
	{
		OutCubes += It->GetPickupType() == ERRPickupType::XP && It->GetPhase() != ERRPickupPhase::Collected ? 1 : 0;
	}
}

void URRPacingBotSubsystem::FinishRun()
{
	bRunning = false;
	UWorld* World = GetWorld();
	RRCheats::SetInvulnerable(World, false, RRPacingBot::Source);

	const ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
	const URRProgressionComponent* Progression = Miner ? Miner->GetProgression() : nullptr;
	FString Times;
	FString Csv = TEXT("Level,Seconds,RobotsAlive,CubesLeft\n");
	for (const FRRPacingLevelTime& Entry : LevelTimes)
	{
		Times += FString::Printf(TEXT(" L%d=%.1f"), Entry.Level, Entry.Seconds);
		Csv += FString::Printf(TEXT("%d,%.2f,%d,%d\n"), Entry.Level, Entry.Seconds, Entry.RobotsAlive, Entry.CubesLeft);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Pacing: run done, seed %d, collect %d, start clock %.2f s, end level %d (XP %.1f / %.1f), %d cards picked; times:%s"),
		RunSeed, bCollectCubes ? 1 : 0, StartSeconds, Progression ? Progression->GetLevel() : 0, Progression ? Progression->GetXP() : 0.f,
		Progression ? Progression->GetXPToNextLevel() : 0.f, CardsPicked, *Times);

	const FString Path = FPaths::ProjectSavedDir() / TEXT("Pacing") /
		FString::Printf(TEXT("Pacing_s%d_c%d_%s.csv"), RunSeed, bCollectCubes ? 1 : 0, *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	const bool bSaved = FFileHelper::SaveStringToFile(Csv, *Path);
	UE_LOG(LogRogueRigs, Log, TEXT("Pacing: %s %s."), bSaved ? TEXT("wrote") : TEXT("could not write"), *Path);

	if (bQuitWhenDone)
	{
		FPlatformMisc::RequestExit(false, RRPacingBot::Source);
	}
}
