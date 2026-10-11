// Rogue Rigs - XP pacing bot: a scripted -game run (real director, real pickups) that aims at the nearest robot, stands still, walks to resting XP cubes or runs straight, picks the first card and logs the time of each level.

#include "Debug/RRPacingBotSubsystem.h"

#include "Debug/RRCheats.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Grid/RRArenaGrid.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/SpringArmComponent.h"
#include "HAL/IConsoleManager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"

namespace RRPacingBot
{
	TAutoConsoleVariable<float> CVarCollectRange(TEXT("rr.Pacing.CollectRange"), 2500.f,
		TEXT("Pacing bot: farthest XP cube (uu) the miner walks to; beyond it the miner stands still."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarCollectRadiusFraction(TEXT("rr.Pacing.CollectRadiusFraction"), 0.8f,
		TEXT("Pacing bot: a cube closer than this fraction of the pickup radius is left to the magnet."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarStatusInterval(TEXT("rr.Pacing.StatusInterval"), 15.f,
		TEXT("Pacing bot: seconds of run clock between two status lines."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarRunLookahead(TEXT("rr.Pacing.RunLookahead"), 200.f,
		TEXT("Pacing bot, run profile: the miner turns when less open floor than this (uu) lies ahead."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarRunProbe(TEXT("rr.Pacing.RunProbe"), 3000.f,
		TEXT("Pacing bot, run profile: longest free distance (uu) measured when choosing a new heading."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarRunStallDistance(TEXT("rr.Pacing.RunStallDistance"), 60.f,
		TEXT("Pacing bot, run profile: moving less than this (uu) in 0.5 s of run clock counts as blocked and makes the miner turn."), ECVF_Cheat);

	/** Label of the log lines and of the cheat calls. */
	const TCHAR* const Source = TEXT("rr.Pacing.Run");

	void HandleRun(const TArray<FString>& Args, UWorld* World)
	{
		const float Seconds = Args.IsValidIndex(0) ? FCString::Atof(*Args[0]) : 200.f;
		const int32 Seed = Args.IsValidIndex(1) ? FCString::Atoi(*Args[1]) : 1;
		const bool bQuit = !Args.IsValidIndex(2) || FCString::Atoi(*Args[2]) != 0;
		const int32 ProfileIndex = Args.IsValidIndex(3) ? FMath::Clamp(FCString::Atoi(*Args[3]), 0, 3) : 1;
		URRPacingBotSubsystem* Bot = World ? World->GetSubsystem<URRPacingBotSubsystem>() : nullptr;
		if (!Bot || !Bot->StartRun(Seconds > 0.f ? Seconds : 200.f, Seed, bQuit, static_cast<ERRPacingProfile>(ProfileIndex)))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: refused (no game world, miner or spawner, or a run is active)."), Source);
		}
	}

	FAutoConsoleCommandWithWorldAndArgs CmdRun(TEXT("rr.Pacing.Run"),
		TEXT("rr.Pacing.Run [Seconds=200] [Seed=1] [Quit=1] [Profile=1]: XP pacing bot (invulnerable, aims at the nearest robot, picks card 0; Profile 0 still, 1 walks to XP cubes, 2 runs straight and turns at walls, 3 collects when a cube is in range else runs); level times in the log and Saved/Pacing."),
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

bool URRPacingBotSubsystem::StartRun(float DurationSeconds, int32 Seed, bool bQuit, ERRPacingProfile InProfile)
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
	Profile = InProfile;
	RunSeed = Seed;
	TActorIterator<ARRArenaGrid> GridIt(World);
	Grid = GridIt ? *GridIt : nullptr;
	// Run profile starts toward screen north (camera forward).
	const USpringArmComponent* Arm = Miner->FindComponentByClass<USpringArmComponent>();
	RunHeading = Arm ? FRotator(0.f, Arm->GetComponentRotation().Yaw, 0.f).Vector() : FVector::ForwardVector;
	StallCheckLocation = Miner->GetActorLocation();
	RunTurns = 0;
	StartSeconds = FoundSpawner->GetRunTime();
	EndSeconds = StartSeconds + DurationSeconds;
	LastLevel = Miner->GetProgression()->GetLevel();
	CardsPicked = 0;
	NextStatusSeconds = StartSeconds;
	StallCheckSeconds = StartSeconds;
	LevelTimes.Reset();
	UE_LOG(LogRogueRigs, Log, TEXT("Pacing: run started, seed %d, %.0f s, profile %d, spawner %s (enabled %d, clock %.2f s), miner level %d, pickup radius %.0f uu."),
		Seed, DurationSeconds, static_cast<int32>(Profile), *FoundSpawner->GetName(), FoundSpawner->IsSpawningEnabled() ? 1 : 0, StartSeconds,
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

	if (Now >= NextStatusSeconds)
	{
		NextStatusSeconds = Now + FMath::Max(RRPacingBot::CVarStatusInterval.GetValueOnGameThread(), 1.f);
		int32 Robots = 0;
		int32 Cubes = 0;
		CountWorld(Robots, Cubes);
		const FVector Location = Miner->GetActorLocation();
		UE_LOG(LogRogueRigs, Log, TEXT("Pacing: status %.1f s, miner (%.0f, %.0f, %.0f), level %d, XP %.1f / %.1f, %d robots alive, %d XP cubes lying, heading (%.2f, %.2f), %d turns."),
			Now, Location.X, Location.Y, Location.Z, Level, Miner->GetProgression()->GetXP(), Miner->GetProgression()->GetXPToNextLevel(), Robots, Cubes,
			RunHeading.X, RunHeading.Y, RunTurns);
	}

	AimAtNearestRobot(Miner);
	if (Profile == ERRPacingProfile::Collect)
	{
		WalkToNearestCube(Miner);
	}
	else if (Profile == ERRPacingProfile::Run)
	{
		RunStraight(Miner, Now);
	}
	else if (Profile == ERRPacingProfile::RunCollect && !WalkToNearestCube(Miner))
	{
		RunStraight(Miner, Now);
	}
}

void URRPacingBotSubsystem::MoveAlong(ARRMiner* Miner, const FVector& Direction)
{
	// ApplyMoveInput takes screen axes (X right, Y up through the camera yaw): project the world direction on them.
	const USpringArmComponent* Arm = Miner->FindComponentByClass<USpringArmComponent>();
	if (!Arm)
	{
		return;
	}
	const FRotator CameraYaw(0.f, Arm->GetComponentRotation().Yaw, 0.f);
	Miner->ApplyMoveInput(FVector2D(FVector::DotProduct(Direction, CameraYaw.RotateVector(FVector::RightVector)),
		FVector::DotProduct(Direction, CameraYaw.RotateVector(FVector::ForwardVector))));
}

float URRPacingBotSubsystem::GetFreeDistance(const ARRArenaGrid& ArenaGrid, const FVector& Origin, const FVector& Direction, float MaxDistance) const
{
	const float Step = FMath::Max(ArenaGrid.GetCellSize() * 0.5f, 10.f);
	const FVector Side = FVector::CrossProduct(FVector::UpVector, Direction);
	const ARRMiner* Miner = RRCheats::FindPlayerMiner(GetWorld());
	const float HalfWidth = Miner ? Miner->GetSimpleCollisionRadius() : 40.f;
	for (float Distance = Step; Distance <= MaxDistance; Distance += Step)
	{
		for (const float Lateral : { 0.f, -HalfWidth, HalfWidth })
		{
			const ERRCellType Type = ArenaGrid.GetCellType(ArenaGrid.GetCellAtWorld(Origin + Direction * Distance + Side * Lateral));
			if (Type != ERRCellType::Floor && Type != ERRCellType::Spawn)
			{
				return Distance - Step;
			}
		}
	}
	return MaxDistance;
}

void URRPacingBotSubsystem::RunStraight(ARRMiner* Miner, float Now)
{
	const ARRArenaGrid* ArenaGrid = Grid.Get();
	const FVector Origin = Miner->GetActorLocation();
	const float Lookahead = RRPacingBot::CVarRunLookahead.GetValueOnGameThread();
	bool bTurn = ArenaGrid && GetFreeDistance(*ArenaGrid, Origin, RunHeading, Lookahead) < Lookahead;
	if (Now - StallCheckSeconds >= 0.5f)
	{
		bTurn |= FVector::Dist2D(Origin, StallCheckLocation) < RRPacingBot::CVarRunStallDistance.GetValueOnGameThread();
		StallCheckSeconds = Now;
		StallCheckLocation = Origin;
	}

	if (bTurn && ArenaGrid)
	{
		// Freest of the 7 other 45-degree headings; going back counts half; a little seeded noise breaks ties.
		const float Probe = RRPacingBot::CVarRunProbe.GetValueOnGameThread();
		FVector Best = -RunHeading;
		float BestScore = -1.f;
		for (int32 Turn = 1; Turn < 8; ++Turn)
		{
			const FVector Candidate = RunHeading.RotateAngleAxis(45.f * Turn, FVector::UpVector).GetSafeNormal2D();
			const float Score = GetFreeDistance(*ArenaGrid, Origin, Candidate, Probe) * (Turn == 4 ? 0.5f : 1.f) + FMath::FRand() * ArenaGrid->GetCellSize();
			if (Score > BestScore)
			{
				BestScore = Score;
				Best = Candidate;
			}
		}
		RunHeading = Best;
		++RunTurns;
		StallCheckSeconds = Now;
		StallCheckLocation = Origin;
	}
	MoveAlong(Miner, RunHeading);
}

void URRPacingBotSubsystem::AimAtNearestRobot(ARRMiner* Miner) const
{
	const FVector Origin = Miner->GetActorLocation();
	const ARREnemy* Nearest = nullptr;
	double NearestSquared = TNumericLimits<double>::Max();
	for (TActorIterator<ARREnemy> It(GetWorld()); It; ++It)
	{
		if (It->IsDead() || URRActorPoolSubsystem::IsPooledIdle(*It))
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

bool URRPacingBotSubsystem::WalkToNearestCube(ARRMiner* Miner) const
{
	const FVector Origin = Miner->GetActorLocation();
	const double MinSquared = FMath::Square(Miner->GetPickupRadius() * RRPacingBot::CVarCollectRadiusFraction.GetValueOnGameThread());
	double NearestSquared = FMath::Square(static_cast<double>(RRPacingBot::CVarCollectRange.GetValueOnGameThread()));
	const ARRPickup* Nearest = nullptr;
	for (TActorIterator<ARRPickup> It(GetWorld()); It; ++It)
	{
		// task-092-01: idle pool pickups are not live pickups.
		if (URRActorPoolSubsystem::IsPooledIdle(*It))
		{
			continue;
		}
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
	if (!Nearest)
	{
		return false;
	}

	MoveAlong(Miner, (Nearest->GetActorLocation() - Origin).GetSafeNormal2D());
	return true;
}

void URRPacingBotSubsystem::CountWorld(int32& OutRobots, int32& OutCubes) const
{
	OutRobots = 0;
	OutCubes = 0;
	for (TActorIterator<ARREnemy> It(GetWorld()); It; ++It)
	{
		OutRobots += It->IsDead() || URRActorPoolSubsystem::IsPooledIdle(*It) ? 0 : 1;
	}
	for (TActorIterator<ARRPickup> It(GetWorld()); It; ++It)
	{
		// task-092-01: idle pool pickups are not live pickups.
		if (URRActorPoolSubsystem::IsPooledIdle(*It))
		{
			continue;
		}
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
	UE_LOG(LogRogueRigs, Log, TEXT("Pacing: run done, seed %d, profile %d, %d turns, start clock %.2f s, end level %d (XP %.1f / %.1f), %d cards picked; times:%s"),
		RunSeed, static_cast<int32>(Profile), RunTurns, StartSeconds, Progression ? Progression->GetLevel() : 0, Progression ? Progression->GetXP() : 0.f,
		Progression ? Progression->GetXPToNextLevel() : 0.f, CardsPicked, *Times);

	const FString Path = FPaths::ProjectSavedDir() / TEXT("Pacing") /
		FString::Printf(TEXT("Pacing_s%d_p%d_%s.csv"), RunSeed, static_cast<int32>(Profile), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	const bool bSaved = FFileHelper::SaveStringToFile(Csv, *Path);
	UE_LOG(LogRogueRigs, Log, TEXT("Pacing: %s %s."), bSaved ? TEXT("wrote") : TEXT("could not write"), *Path);

	if (bQuitWhenDone)
	{
		FPlatformMisc::RequestExit(false, RRPacingBot::Source);
	}
}
