// Rogue Rigs - pressure integration tests with the final Blueprints (RogueRigs.Integration.Pressure*, task-026-04): the director config of the BP_EnemySpawner placed in L_ArenaM1 runs calm, lone robots then the first pack; a horde is announced 4 s before a front of 34+ robots with the WBP_HUD HordeBanner and alert icons; a BP_Chaser hit lights the WBP_HUD DamageVignette, stronger at low health.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "Debug/RRCheats.h"
#include "Debug/RRCheatSettings.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRWaveDirector.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "Grid/RRArenaGrid.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UI/RRHUDWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPressureIntegrationTests
{
	/** Final Blueprints and map of the pressure loop. */
	const TCHAR* const PressureGameModePath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRGameMode.BP_RRGameMode_C");
	const TCHAR* const PressureSpawnerPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner.BP_EnemySpawner_C");
	const TCHAR* const PressureHUDPath = TEXT("/Game/RogueRigs/UI/WBP_HUD.WBP_HUD_C");
	const TCHAR* const PressureChaserPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");
	const TCHAR* const PressureMapPackage = TEXT("/Game/RogueRigs/Maps/L_ArenaM1");

	/** Miner cell on arena_m1_fine (open floor), as in the spawner and director tests. */
	const FIntPoint PressureMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Chaser start of the hit test: legacy (43, 40), the start of Integration.ChasersKillMinerGameOver, 9 fine cells east. */
	const FIntPoint PressureChaserCell = RRTestHarness::FineCell(FIntPoint(43, 40));

	/** Half extent (uu) of the fixed spawner view rectangle centered on the miner. */
	const FVector2D PressureViewHalfExtent(1600.f, 1000.f);

	/** Width (px) of the WBP_HUD alert bar: an icon's RenderTranslation X is Position x this (wbp-hud). */
	constexpr float PressureAlertBarWidth = 600.f;

	/** Frame length of the tests. */
	constexpr float PressureFrame = 1.f / 60.f;

	/** Game instance of a test world. Declare it before the FTestWorld: it outlives the world, then drops its context pointer. */
	struct FPressureGameInstance
	{
		/** Clears the (destroyed) world context pointer of the game instance and releases it. */
		~FPressureGameInstance()
		{
			if (GameInstance.IsValid())
			{
				RRTestAccess::FGameInstanceAccess::WorldContextOf(GameInstance.Get()) = nullptr;
			}
		}

		/** The game instance, kept alive for the test. */
		TStrongObjectPtr<UGameInstance> GameInstance;
	};

	/** Resets the persistent cheats these tests touch (invulnerable, light halo). Declare after the world: runs before its teardown. */
	struct FPressureCheatGuard
	{
		explicit FPressureCheatGuard(UWorld* InWorld) : World(InWorld) { Reset(); }
		~FPressureCheatGuard()
		{
			RRCheats::SetInvulnerable(World, false, TEXT("RogueRigs.Integration.Pressure"));
			Reset();
		}

		/** Neutral values of the persistent cheat properties. */
		static void Reset()
		{
			URRCheatSettings* Settings = GetMutableDefault<URRCheatSettings>();
			Settings->bInvulnerable = false;
			Settings->bLightHalo = false;
		}

		/** World whose miner may be made invulnerable. */
		UWorld* World = nullptr;
	};

	/** Gives the test world a game instance and BP_RRGameMode as authority game mode (widgets with a world, run flow config). */
	bool StartPressureGameMode(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, FPressureGameInstance& OutGameInstance)
	{
		UClass* GameModeClass = RRTestHarness::LoadTestClass<AGameModeBase>(Test, PressureGameModePath);
		UWorld* World = TestWorld.Get();
		FWorldContext* Context = GEngine->GetWorldContextFromWorld(World);
		if (!GameModeClass || !Test.TestNotNull(TEXT("test world has a world context"), Context))
		{
			return false;
		}
		UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine);
		OutGameInstance.GameInstance.Reset(GameInstance);
		Context->OwningGameInstance = GameInstance;
		RRTestAccess::FGameInstanceAccess::WorldContextOf(GameInstance) = Context;
		World->SetGameInstance(GameInstance);
		World->GetWorldSettings()->DefaultGameMode = GameModeClass;
		World->SetGameMode(FURL());
		const AGameModeBase* GameMode = World->GetAuthGameMode();
		return Test.TestTrue(TEXT("authority game mode is BP_RRGameMode"), GameMode && GameMode->IsA(GameModeClass));
	}

	/** The ARREnemySpawner placed in L_ArenaM1 as saved on disk (level actors, else its external actor packages); null with a failure. */
	ARREnemySpawner* LoadPlacedSpawner(FAutomationTestBase& Test)
	{
		UPackage* MapPackage = LoadPackage(nullptr, PressureMapPackage, LOAD_None);
		UWorld* MapWorld = MapPackage ? UWorld::FindWorldInPackage(MapPackage) : nullptr;
		if (!Test.TestNotNull(TEXT("L_ArenaM1 loads"), MapWorld) || !Test.TestNotNull(TEXT("L_ArenaM1 level"), MapWorld->PersistentLevel.Get()))
		{
			return nullptr;
		}
		ULevel* Level = MapWorld->PersistentLevel;
		TArray<ARREnemySpawner*> Spawners;
		for (AActor* Actor : Level->Actors)
		{
			if (ARREnemySpawner* Spawner = Cast<ARREnemySpawner>(Actor))
			{
				Spawners.AddUnique(Spawner);
			}
		}
#if WITH_EDITOR // external actor packages (one file per actor) exist only in editor builds; a cooked map holds its actors
		if (Spawners.IsEmpty() && Level->IsUsingExternalActors())
		{
			for (const FString& ActorPackageName : Level->GetOnDiskExternalActorPackages(/*bTryUsingPackageLoadedPath*/ true))
			{
				if (UPackage* ActorPackage = LoadPackage(nullptr, *ActorPackageName, LOAD_None))
				{
					ForEachObjectWithPackage(ActorPackage, [&Spawners](UObject* Object)
					{
						if (ARREnemySpawner* Spawner = Cast<ARREnemySpawner>(Object); Spawner && !Spawner->IsTemplate())
						{
							Spawners.AddUnique(Spawner);
						}
						return true;
					});
				}
			}
		}
#endif // WITH_EDITOR
		return Test.TestEqual(TEXT("exactly one spawner placed in L_ArenaM1"), Spawners.Num(), 1) ? Spawners[0] : nullptr;
	}

	/** World pieces of a pressure loop. */
	struct FPressureLoop
	{
		/** Unbinds the HUD (the world is destroyed after this). */
		~FPressureLoop()
		{
			if (HUD)
			{
				HUD->BindToMiner(nullptr);
				HUD->BindToRunFlow(nullptr);
				HUD->BindToSpawner(nullptr);
			}
		}

		/** BP_ArenaGrid on the game layout. */
		const ARRArenaGrid* Grid = nullptr;
		/** Final BP_Miner on PressureMinerCell, weapons stopped. */
		ARRMiner* Miner = nullptr;
		/** BP_EnemySpawner carrying the director, weights and cap of the placed spawner; disabled until the test enables it. */
		ARREnemySpawner* Spawner = nullptr;
		/** WBP_HUD bound to the miner, the run flow and the spawner (null when not requested). */
		URRHUDWidget* HUD = nullptr;
		/** Keeps WBP_HUD alive: the world tick may run a garbage collection and nothing else references the widget. */
		TStrongObjectPtr<URRHUDWidget> HUDRoot;
		/** Records the director events of the spawner. */
		TStrongObjectPtr<URRTestEventRecorder> Recorder;
	};

	/**
	 * Fine grid, BP_Miner (weapons stopped), BP_EnemySpawner given the DirectorConfig, WeightedEnemyClasses and MaxAlive of the spawner
	 * placed in L_ArenaM1 (Director: that config, editable by the caller before enabling), a fixed view around the miner, seeded,
	 * director events recorded; with bHUD, WBP_HUD bound to the miner, the run flow and the spawner.
	 */
	bool StartPressureLoop(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, bool bHUD, FPressureLoop& Out,
		TFunctionRef<void(FRRWaveDirectorConfig&)> EditDirector)
	{
		using namespace RRTestAccess;
		UClass* SpawnerClass = RRTestHarness::LoadTestClass<ARREnemySpawner>(Test, PressureSpawnerPath);
		UClass* HUDClass = bHUD ? RRTestHarness::LoadTestClass<URRHUDWidget>(Test, PressureHUDPath) : nullptr;
		ARREnemySpawner* Placed = LoadPlacedSpawner(Test);
		Out.Grid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Out.Grid, PressureMinerCell) : nullptr;
		if (!SpawnerClass || (bHUD && !HUDClass) || !Placed || !Out.Miner)
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
		Out.Spawner = TestWorld.Spawn<ARREnemySpawner>(SpawnerClass, FVector::ZeroVector);
		if (!Test.TestNotNull(TEXT("BP_EnemySpawner spawned"), Out.Spawner))
		{
			return false;
		}
		Out.Spawner->SetSpawnMaxFrames(0); // task-091-01: robots delivered at once (these checks read spawn results in the same frame); spread covered by RogueRigs.RobotPool.
		Out.Spawner->SetSpawningEnabled(false);
		FSpawnerAccess::DirectorOf(Out.Spawner) = FSpawnerAccess::DirectorOf(Placed);
		FSpawnerAccess::WeightsOf(Out.Spawner) = FSpawnerAccess::WeightsOf(Placed);
		FSpawnerAccess::MaxAliveOf(Out.Spawner) = FSpawnerAccess::MaxAliveOf(Placed);
		EditDirector(FSpawnerAccess::DirectorOf(Out.Spawner));
		const FVector2D Center(Out.Miner->GetActorLocation());
		Out.Spawner->SetViewRectangleOverride(FBox2D(Center - PressureViewHalfExtent, Center + PressureViewHalfExtent));
		Out.Spawner->SeedRandomStream(20261005);
		Out.Recorder.Reset(NewObject<URRTestEventRecorder>());
		Out.Spawner->OnDirectorPhaseChanged.AddDynamic(Out.Recorder.Get(), &URRTestEventRecorder::HandleDirectorPhaseChanged);
		Out.Spawner->OnHordeLull.AddDynamic(Out.Recorder.Get(), &URRTestEventRecorder::HandleHordeLull);
		Out.Spawner->OnHordeIncoming.AddDynamic(Out.Recorder.Get(), &URRTestEventRecorder::HandleHordeIncoming);
		Out.Spawner->OnHordeStarted.AddDynamic(Out.Recorder.Get(), &URRTestEventRecorder::HandleHordeStarted);
		Out.Spawner->OnHordeFinished.AddDynamic(Out.Recorder.Get(), &URRTestEventRecorder::HandleHordeFinished);
		if (bHUD)
		{
			Out.HUD = CreateWidget<URRHUDWidget>(TestWorld.Get(), HUDClass);
			Out.HUDRoot.Reset(Out.HUD);
			if (!Test.TestNotNull(TEXT("WBP_HUD created"), Out.HUD))
			{
				return false;
			}
			Out.HUD->BindToMiner(Out.Miner);
			Out.HUD->BindToRunFlow(TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>());
			Out.HUD->BindToSpawner(Out.Spawner);
		}
		return true;
	}

	/** One world frame, then the HUD's native tick (Event Tick: UpdateVignette) as Slate would run it in game. */
	void TickPressureFrame(const RRTestHarness::FTestWorld& TestWorld, URRHUDWidget* HUD)
	{
		TestWorld.Tick();
		if (HUD)
		{
			RRTestAccess::FUserWidgetAccess::NativeTickOf(HUD, PressureFrame);
		}
	}

	/** Widget Name of WBP_HUD (null with a failure). */
	UWidget* FindHUDWidget(FAutomationTestBase& Test, URRHUDWidget* HUD, const TCHAR* Name)
	{
		UWidget* Widget = HUD->GetWidgetFromName(Name);
		Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has %s"), Name), Widget);
		return Widget;
	}

	/** True when Widget is drawn (not Collapsed, not Hidden). */
	bool IsHUDWidgetShown(const UWidget* Widget)
	{
		const ESlateVisibility Visibility = Widget ? Widget->GetVisibility() : ESlateVisibility::Collapsed;
		return Visibility != ESlateVisibility::Collapsed && Visibility != ESlateVisibility::Hidden;
	}

	/** Value of the numeric Blueprint variable Name of WBP_HUD (-1 with a failure when missing). */
	double GetHUDNumber(FAutomationTestBase& Test, const URRHUDWidget* HUD, const TCHAR* Name)
	{
		const FNumericProperty* Property = CastField<FNumericProperty>(HUD->GetClass()->FindPropertyByName(Name));
		if (!Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has variable %s"), Name), Property))
		{
			return -1.0;
		}
		const void* Value = Property->ContainerPtrToValuePtr<void>(HUD);
		return Property->IsFloatingPoint() ? Property->GetFloatingPointPropertyValue(Value) : static_cast<double>(Property->GetSignedIntPropertyValue(Value));
	}

	/** Index of the first director event starting with Prefix, or INDEX_NONE. */
	int32 FindDirectorEvent(const TArray<FString>& Events, const TCHAR* Prefix)
	{
		return Events.IndexOfByPredicate([Prefix](const FString& Event) { return Event.StartsWith(Prefix); });
	}

	/** Spawns BP_Chaser with its feet on the floor of Cell (it walks to the miner). */
	ARRChaser* SpawnPressureChaser(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, UClass* ChaserClass, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		const float HalfHeight = ChaserClass->GetDefaultObject<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRChaser* Chaser = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ChaserClass, SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("BP_Chaser spawned"), Chaser))
		{
			Chaser->FinishSpawning(SpawnTransform);
		}
		return Chaser;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPressureDirectorPhasesTest, "RogueRigs.Integration.PressureDirectorPhases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPressureDirectorPhasesTest::RunTest(const FString& Parameters)
{
	using namespace RRPressureIntegrationTests;
	using namespace RRTestAccess;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRPressureDirectorPhases"));
	FPressureCheatGuard CheatGuard(TestWorld.Get());
	FPressureLoop Loop;
	if (!StartPressureLoop(*this, TestWorld, /*bHUD*/ false, Loop, [](FRRWaveDirectorConfig&) {}))
	{
		return false;
	}

	// The placed spawner carries the provisional default pacing (sys-spawner-director).
	const FRRWaveDirectorConfig& Config = FSpawnerAccess::DirectorOf(Loop.Spawner);
	const FRRWaveDirectorConfig Default = FRRWaveDirectorConfig::MakeDefault();
	TestTrue(TEXT("placed director enabled"), Config.bEnabled);
	TestEqual(TEXT("placed MaxAlive 80"), FSpawnerAccess::MaxAliveOf(Loop.Spawner), 80);
	if (!TestEqual(TEXT("placed phases match the default count"), Config.Phases.Num(), Default.Phases.Num()) || Config.Phases.Num() < 4)
	{
		return false;
	}
	FString PhaseLog;
	for (int32 Index = 0; Index < Config.Phases.Num(); ++Index)
	{
		PhaseLog += FString::Printf(TEXT(" %s@%.1f"), *Config.Phases[Index].Name.ToString(), Config.Phases[Index].StartTime);
		TestEqual(FString::Printf(TEXT("phase %d name"), Index), Config.Phases[Index].Name, Default.Phases[Index].Name);
		TestEqual(FString::Printf(TEXT("phase %d start"), Index), Config.Phases[Index].StartTime, Default.Phases[Index].StartTime);
	}
	if (TestEqual(TEXT("two hordes"), Config.Hordes.Num(), 2))
	{
		TestEqual(TEXT("horde 0 front at 95 s"), Config.Hordes[0].TriggerTime, 95.f);
		TestEqual(TEXT("horde 1 front at 175 s"), Config.Hordes[1].TriggerTime, 175.f);
		TestEqual(TEXT("banner 4 s before"), Config.Hordes[0].WarningSeconds, 4.f);
	}
	AddInfo(FString::Printf(TEXT("placed phases:%s"), *PhaseLog));

	// Run to just past the first pack (Isolated 1 s every 2 s, FirstPack 15 s); the miner cannot die, robots cannot die.
	TestEqual(TEXT("miner made invulnerable"), RRCheats::SetInvulnerable(TestWorld.Get(), true, TEXT("RogueRigs.Integration.Pressure")), 1);
	const int32 IsolatedIndex = Config.Phases.IndexOfByPredicate([](const FRRDirectorPhase& Phase) { return Phase.Name == TEXT("Isolated"); });
	const int32 PackIndex = Config.Phases.IndexOfByPredicate([](const FRRDirectorPhase& Phase) { return Phase.Name == TEXT("FirstPack"); });
	if (!TestTrue(TEXT("Isolated then FirstPack phases"), IsolatedIndex != INDEX_NONE && PackIndex == IsolatedIndex + 1))
	{
		return false;
	}
	const float IsolatedStart = Config.Phases[IsolatedIndex].StartTime;
	const float PackStart = Config.Phases[PackIndex].StartTime;
	TestTrue(*FString::Printf(TEXT("first robot at about 1 s (%.1f s)"), IsolatedStart), IsolatedStart > 0.f && IsolatedStart <= 1.f);
	const int32 IsolatedFrame = FMath::RoundToInt(IsolatedStart * 60.f);
	const int32 PackFrame = FMath::RoundToInt(PackStart * 60.f);
	const int32 Frames = PackFrame + 60;
	// Recycling off: this test counts spawns through the alive count; far frozen robots would be recycled (task-034-01 margin 0).
	RRTestAccess::FSpawnerAccess::RecycleEnabledOf(Loop.Spawner) = false;
	Loop.Spawner->SetSpawningEnabled(true);
	TArray<int32> SpawnFrames;
	TArray<int32> SpawnCounts;
	int32 AliveBefore = 0;
	int32 AliveBeforeIsolated = 0;
	int32 AliveBeforePack = 0;
	TMap<FString, int32> PhaseFrames;
	int32 SeenEvents = 0;
	for (int32 Frame = 1; Frame <= Frames; ++Frame)
	{
		TestWorld.Tick();
		const TArray<FString>& Events = Loop.Recorder->GetDirectorEvents();
		for (; SeenEvents < Events.Num(); ++SeenEvents)
		{
			PhaseFrames.Add(Events[SeenEvents], Frame);
		}
		const int32 Alive = Loop.Spawner->GetAliveCount();
		if (Alive != AliveBefore)
		{
			SpawnFrames.Add(Frame);
			SpawnCounts.Add(Alive - AliveBefore);
			AliveBefore = Alive;
		}
		AliveBeforeIsolated = Frame == IsolatedFrame - 2 ? Alive : AliveBeforeIsolated;
		AliveBeforePack = Frame == PackFrame - 2 ? Alive : AliveBeforePack;
	}
	FString SpawnLog;
	for (int32 Index = 0; Index < SpawnFrames.Num(); ++Index)
	{
		SpawnLog += FString::Printf(TEXT(" %+d@%.2fs"), SpawnCounts[Index], SpawnFrames[Index] / 60.f);
	}
	AddInfo(FString::Printf(TEXT("director events: %s; alive changes:%s"), *FString::Join(Loop.Recorder->GetDirectorEvents(), TEXT(", ")), *SpawnLog));

	auto CheckPhase = [this, &PhaseFrames, &Config](int32 Index, int32 Expected)
	{
		const FString Key = FString::Printf(TEXT("Phase:%d:%s"), Index, *Config.Phases[Index].Name.ToString());
		const int32* Frame = PhaseFrames.Find(Key);
		TestTrue(*FString::Printf(TEXT("%s at frame %d (%d)"), *Key, Expected, Frame ? *Frame : -1), Frame && FMath::Abs(*Frame - Expected) <= 1);
	};
	CheckPhase(IsolatedIndex, IsolatedFrame);
	CheckPhase(PackIndex, PackFrame);
	TestEqual(TEXT("no robot before the isolated phase"), AliveBeforeIsolated, 0);
	if (TestTrue(TEXT("robots spawned"), SpawnFrames.Num() > 0))
	{
		TestTrue(*FString::Printf(TEXT("first robot when the isolated phase starts (%.2f s)"), SpawnFrames[0] / 60.f), FMath::Abs(SpawnFrames[0] - IsolatedFrame) <= 1);
	}
	// Lone robots one at a time every SingleInterval until the pack.
	const float SingleInterval = Config.Phases[IsolatedIndex].SingleInterval;
	const int32 ExpectedSingles = SingleInterval > 0.f ? FMath::CeilToInt((PackStart - IsolatedStart) / SingleInterval - KINDA_SMALL_NUMBER) : 0;
	TestEqual(*FString::Printf(TEXT("%d lone robots before the pack"), ExpectedSingles), AliveBeforePack, ExpectedSingles);
	int32 PackSize = 0;
	for (int32 Index = 0; Index < SpawnFrames.Num(); ++Index)
	{
		if (SpawnFrames[Index] < PackFrame - 1)
		{
			TestEqual(*FString::Printf(TEXT("lone robot at %.2f s is one robot"), SpawnFrames[Index] / 60.f), SpawnCounts[Index], 1);
		}
		else if (FMath::Abs(SpawnFrames[Index] - PackFrame) <= 1)
		{
			PackSize += SpawnCounts[Index];
		}
	}
	TestTrue(*FString::Printf(TEXT("first pack of 6-8 chasers at %.1f s (%d)"), PackStart, PackSize), PackSize >= 6 && PackSize <= 8);
	int32 Chasers = 0;
	for (const ARREnemy* Enemy : Loop.Spawner->GetAliveEnemies())
	{
		Chasers += Enemy && Enemy->IsA(ARRChaser::StaticClass()) ? 1 : 0;
	}
	TestEqual(TEXT("every robot so far is a chaser"), Chasers, Loop.Spawner->GetAliveCount());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPressureHordeBannerTest, "RogueRigs.Integration.PressureHordeBanner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPressureHordeBannerTest::RunTest(const FString& Parameters)
{
	using namespace RRPressureIntegrationTests;
	using namespace RRTestAccess;

	FPressureGameInstance GameInstance;
	RRTestHarness::FTestWorld TestWorld(TEXT("RRPressureHordeBanner"));
	FPressureCheatGuard CheatGuard(TestWorld.Get());
	if (!StartPressureGameMode(*this, TestWorld, GameInstance))
	{
		return false;
	}
	// Placed config with its hordes moved closer: no phases (nothing else spawns), horde 0 front at 8 s (lull, banner and size as
	// placed), horde 1 kept 80 s later; time scales of the run flow untouched.
	constexpr float FrontTime = 8.f;
	float Warning = 0.f;
	FPressureLoop Loop;
	if (!StartPressureLoop(*this, TestWorld, /*bHUD*/ true, Loop, [&Warning, FrontTime](FRRWaveDirectorConfig& Director)
		{
			Director.Phases.Reset();
			Director.HordeTrigger = ERRHordeTrigger::RunTime;
			if (Director.Hordes.Num() == 2)
			{
				Director.Hordes[1].TriggerTime = FrontTime + Director.Hordes[1].TriggerTime - Director.Hordes[0].TriggerTime;
				Director.Hordes[0].TriggerTime = FrontTime;
				Warning = Director.Hordes[0].WarningSeconds;
			}
		}))
	{
		return false;
	}
	const FRRWaveDirectorConfig& Config = FSpawnerAccess::DirectorOf(Loop.Spawner);
	if (!TestEqual(TEXT("two placed hordes"), Config.Hordes.Num(), 2) || !TestEqual(TEXT("placed banner 4 s before the front"), Warning, 4.f))
	{
		return false;
	}
	TestTrue(TEXT("placed horde of 34+ robots"), Config.Hordes[0].SizeMin >= 34);
	TestEqual(TEXT("miner made invulnerable"), RRCheats::SetInvulnerable(TestWorld.Get(), true, TEXT("RogueRigs.Integration.Pressure")), 1);
	URRHUDWidget* HUD = Loop.HUD;
	UWidget* Banner = FindHUDWidget(*this, HUD, TEXT("HordeBanner"));
	UWidget* ConsoleIcon = FindHUDWidget(*this, HUD, TEXT("AlertIconConsole"));
	UWidget* ShuttleIcon = FindHUDWidget(*this, HUD, TEXT("AlertIconShuttle"));
	UWidget* HordeIcons[4] = { nullptr, nullptr, nullptr, nullptr };
	for (int32 Index = 0; Index < 4; ++Index)
	{
		HordeIcons[Index] = FindHUDWidget(*this, HUD, *FString::Printf(TEXT("AlertIconHorde%d"), Index));
	}
	if (!Banner || !ConsoleIcon || !ShuttleIcon || !HordeIcons[0] || !HordeIcons[1] || !HordeIcons[2] || !HordeIcons[3])
	{
		return false;
	}

	// Alert icons placed from OnAlertEventsChanged: shown at Position x bar width, opaque while pending; unused horde slots collapsed.
	auto CheckIcon = [this](const TCHAR* What, const UWidget* Icon, const FRRAlertBarEvent* Event)
	{
		if (!TestNotNull(*FString::Printf(TEXT("%s event listed"), What), Event))
		{
			return;
		}
		TestTrue(*FString::Printf(TEXT("%s icon shown"), What), IsHUDWidgetShown(Icon));
		TestNearlyEqual(*FString::Printf(TEXT("%s icon at Position x 600 (%.3f)"), What, Event->Position), static_cast<float>(Icon->GetRenderTransform().Translation.X),
			Event->Position * PressureAlertBarWidth, 0.5f);
		TestNearlyEqual(*FString::Printf(TEXT("%s icon opacity (done %d)"), What, Event->bDone ? 1 : 0), Icon->GetRenderOpacity(), Event->bDone ? 0.35f : 1.f, 0.01f);
	};
	auto FindEvent = [HUD](ERRAlertBarEventType Type, int32 Index) -> const FRRAlertBarEvent*
	{
		return HUD->GetAlertBarEvents().FindByPredicate([Type, Index](const FRRAlertBarEvent& Event) { return Event.Type == Type && (Index == INDEX_NONE || Event.Index == Index); });
	};
	TickPressureFrame(TestWorld, HUD);
	TestEqual(TEXT("five alert events (console, bonus chest, 2 hordes, shuttle)"), HUD->GetAlertBarEvents().Num(), 5);
	CheckIcon(TEXT("console"), ConsoleIcon, FindEvent(ERRAlertBarEventType::Console, INDEX_NONE));
	CheckIcon(TEXT("shuttle"), ShuttleIcon, FindEvent(ERRAlertBarEventType::Extraction, INDEX_NONE));
	CheckIcon(TEXT("horde 0"), HordeIcons[0], FindEvent(ERRAlertBarEventType::Horde, 0));
	CheckIcon(TEXT("horde 1"), HordeIcons[1], FindEvent(ERRAlertBarEventType::Horde, 1));
	TestFalse(TEXT("horde icon 2 collapsed"), IsHUDWidgetShown(HordeIcons[2]));
	TestFalse(TEXT("horde icon 3 collapsed"), IsHUDWidgetShown(HordeIcons[3]));
	TestFalse(TEXT("banner collapsed at start"), IsHUDWidgetShown(Banner));

	// Run past the front: banner frames, director frames, robots.
	Loop.Spawner->SetSpawningEnabled(true);
	const float Spread = Config.Hordes[0].SpreadSeconds;
	const int32 Frames = FMath::RoundToInt((FrontTime + Spread) * 60.f) + 30;
	int32 BannerOn = INDEX_NONE;
	int32 BannerOff = INDEX_NONE;
	int32 BannerShows = 0;
	int32 StartFrame = INDEX_NONE;
	int32 FinishFrame = INDEX_NONE;
	int32 AliveBeforeFront = -1;
	FString BannerText;
	for (int32 Frame = 1; Frame <= Frames; ++Frame)
	{
		const bool bWasShown = IsHUDWidgetShown(Banner);
		TickPressureFrame(TestWorld, HUD);
		const bool bShown = IsHUDWidgetShown(Banner);
		if (bShown && !bWasShown)
		{
			++BannerShows;
			BannerOn = BannerOn == INDEX_NONE ? Frame : BannerOn;
			if (const UTextBlock* Text = Cast<UTextBlock>(Banner))
			{
				BannerText = Text->GetText().ToString();
			}
		}
		else if (!bShown && bWasShown && BannerOff == INDEX_NONE)
		{
			BannerOff = Frame;
		}
		const TArray<FString>& Events = Loop.Recorder->GetDirectorEvents();
		StartFrame = StartFrame == INDEX_NONE && FindDirectorEvent(Events, TEXT("Started:0")) != INDEX_NONE ? Frame : StartFrame;
		FinishFrame = FinishFrame == INDEX_NONE && FindDirectorEvent(Events, TEXT("Finished:0")) != INDEX_NONE ? Frame : FinishFrame;
		AliveBeforeFront = StartFrame == INDEX_NONE ? Loop.Spawner->GetAliveCount() : AliveBeforeFront;
		if (Frame == FMath::RoundToInt(FrontTime * 60.f) + 2)
		{
			// Front started: the horde 0 icon is done (faded), horde 1 still pending.
			CheckIcon(TEXT("horde 0 after the front"), HordeIcons[0], FindEvent(ERRAlertBarEventType::Horde, 0));
			const FRRAlertBarEvent* Done = FindEvent(ERRAlertBarEventType::Horde, 0);
			TestTrue(TEXT("horde 0 event done at the front"), Done && Done->bDone);
			CheckIcon(TEXT("horde 1 after the front"), HordeIcons[1], FindEvent(ERRAlertBarEventType::Horde, 1));
		}
	}
	const TArray<FString>& Events = Loop.Recorder->GetDirectorEvents();
	AddInfo(FString::Printf(TEXT("banner '%s' shown at %.2f s, hidden at %.2f s (%d shows); front %.2f s, over %.2f s; events %s"), *BannerText,
		BannerOn / 60.f, BannerOff / 60.f, BannerShows, StartFrame / 60.f, FinishFrame / 60.f, *FString::Join(Events, TEXT(", "))));

	// Banner: once, 4 s before the front (the Blueprint Delay hides it after SecondsUntil).
	const int32 FrontFrame = FMath::RoundToInt(FrontTime * 60.f);
	const int32 BannerFrame = FMath::RoundToInt((FrontTime - Warning) * 60.f);
	TestEqual(TEXT("banner shown once"), BannerShows, 1);
	TestTrue(*FString::Printf(TEXT("banner shown 4 s before the front (frame %d, expected %d)"), BannerOn, BannerFrame), FMath::Abs(BannerOn - BannerFrame) <= 2);
	TestTrue(*FString::Printf(TEXT("banner hidden about 4 s later (frame %d, expected %d)"), BannerOff, FrontFrame), BannerOff != INDEX_NONE && FMath::Abs(BannerOff - FrontFrame) <= 3);
	TestTrue(TEXT("banner text announces the horde"), BannerText.Contains(TEXT("HORDE")));
	if (TestEqual(TEXT("one OnHordeIncoming"), Loop.Recorder->GetHordeIncomingSeconds().Num(), 1))
	{
		TestNearlyEqual(TEXT("OnHordeIncoming announces 4 s"), Loop.Recorder->GetHordeIncomingSeconds()[0], Warning, 2.f / 60.f);
	}

	// Front: at 8 s, 34+ robots over the spread (task-045-01), nothing before it.
	TestTrue(*FString::Printf(TEXT("front at %.1f s (frame %d)"), FrontTime, StartFrame), FMath::Abs(StartFrame - FrontFrame) <= 1);
	TestEqual(TEXT("nothing spawned before the front"), AliveBeforeFront, 0);
	const int32 Finished = FindDirectorEvent(Events, TEXT("Finished:0"));
	int32 Spawned = 0;
	if (TestTrue(TEXT("horde finished"), Finished != INDEX_NONE))
	{
		LexFromString(Spawned, *Events[Finished].RightChop(FCString::Strlen(TEXT("Finished:0:"))));
	}
	TestTrue(*FString::Printf(TEXT("front of 34+ robots (%d spawned, %d alive)"), Spawned, Loop.Spawner->GetAliveCount()), Spawned >= 34 && Loop.Spawner->GetAliveCount() >= 34);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPressureVignetteTest, "RogueRigs.Integration.PressureVignetteOnHit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPressureVignetteTest::RunTest(const FString& Parameters)
{
	using namespace RRPressureIntegrationTests;

	FPressureGameInstance GameInstance;
	RRTestHarness::FTestWorld TestWorld(TEXT("RRPressureVignette"));
	FPressureCheatGuard CheatGuard(TestWorld.Get());
	UClass* ChaserClass = RRTestHarness::LoadTestClass<ARRChaser>(*this, PressureChaserPath);
	if (!ChaserClass || !StartPressureGameMode(*this, TestWorld, GameInstance))
	{
		return false;
	}
	// Spawner present (the HUD binds it) but never enabled.
	FPressureLoop Loop;
	if (!StartPressureLoop(*this, TestWorld, /*bHUD*/ true, Loop, [](FRRWaveDirectorConfig&) {}))
	{
		return false;
	}
	URRHUDWidget* HUD = Loop.HUD;
	ARRMiner* Miner = Loop.Miner;
	UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent();
	UWidget* Vignette = FindHUDWidget(*this, HUD, TEXT("DamageVignette"));
	const ARRArenaGrid* Grid = Loop.Grid;
	if (!Vignette || !ASC || !TestNotNull(TEXT("grid"), Grid))
	{
		return false;
	}
	const float MaxHealth = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	auto Health = [ASC]() { return ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()); };
	for (int32 Frame = 0; Frame < 5; ++Frame)
	{
		TickPressureFrame(TestWorld, HUD);
	}
	// "No vignette" is the invisible warm-up floor that keeps its material rendering (task-055-01).
	const float WarmOpacity = HUD->GetVignetteWarmOpacity();
	TestNearlyEqual(TEXT("no vignette at full health (warm floor)"), Vignette->GetRenderOpacity(), WarmOpacity, KINDA_SMALL_NUMBER);

	// A real BP_Chaser hit: ticks until the miner's Health drops; returns the vignette opacity on that frame (-1 when no hit).
	auto HitByChaser = [this, &TestWorld, HUD, Vignette, ChaserClass, Grid, &Health](const TCHAR* What, float& OutHitFlash) -> float
	{
		ARRChaser* Chaser = SpawnPressureChaser(*this, TestWorld, ChaserClass, Grid, PressureChaserCell);
		if (!Chaser)
		{
			return -1.f;
		}
		const float Before = Health();
		float Opacity = -1.f;
		for (int32 Frame = 1; Frame <= 900; ++Frame)
		{
			TickPressureFrame(TestWorld, HUD);
			if (Health() < Before)
			{
				Opacity = Vignette->GetRenderOpacity();
				OutHitFlash = GetHUDNumber(*this, HUD, TEXT("HitFlash"));
				AddInfo(FString::Printf(TEXT("%s: hit at %.2f s, Health %.0f -> %.0f, HitFlash %.3f, vignette %.3f"), What, Frame / 60.f, Before, Health(), OutHitFlash, Opacity));
				break;
			}
		}
		Chaser->Destroy();
		TestTrue(*FString::Printf(TEXT("%s: the chaser hit the miner"), What), Opacity >= 0.f);
		return Opacity;
	};

	// Hit at full health: a flash that fades back to nothing.
	float FullHitFlash = 0.f;
	const float FullPeak = HitByChaser(TEXT("full health"), FullHitFlash);
	TestTrue(*FString::Printf(TEXT("vignette shown after a hit (%.3f)"), FullPeak), FullPeak > WarmOpacity);
	TestTrue(TEXT("OnPlayerDamaged set HitFlash"), FullHitFlash > 0.f);
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TickPressureFrame(TestWorld, HUD);
	}
	TestNearlyEqual(TEXT("the flash fades within 1 s above 40 % health"), Vignette->GetRenderOpacity(), WarmOpacity, KINDA_SMALL_NUMBER);

	// Low health (25 %): a steady base once the flash is gone, and a stronger flash on the next hit.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), MaxHealth * 0.25f);
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TickPressureFrame(TestWorld, HUD);
	}
	const float LowBase = Vignette->GetRenderOpacity();
	AddInfo(FString::Printf(TEXT("low health %.0f / %.0f: steady vignette %.3f, LowHealthAlpha %.3f"), Health(), MaxHealth, LowBase,
		GetHUDNumber(*this, HUD, TEXT("LowHealthAlpha"))));
	TestTrue(*FString::Printf(TEXT("steady vignette below 40 %% health (%.3f)"), LowBase), LowBase > WarmOpacity);
	float LowHitFlash = 0.f;
	const float LowPeak = HitByChaser(TEXT("low health"), LowHitFlash);
	TestTrue(*FString::Printf(TEXT("stronger vignette on a hit at low health (%.3f > %.3f)"), LowPeak, FullPeak), LowPeak > FullPeak);
	TestTrue(TEXT("HitFlash larger at low health"), LowHitFlash > FullHitFlash);
	TestTrue(*FString::Printf(TEXT("vignette clamped to 0.9 (%.3f)"), LowPeak), LowPeak <= 0.9f + KINDA_SMALL_NUMBER);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
