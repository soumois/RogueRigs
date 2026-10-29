// Rogue Rigs - shared harness of the automation tests: transient game world ticked manually, Blueprint class loading, grid, miner and still-chaser spawning.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/RRChaser.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/IConsoleManager.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Player/RRMiner.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRShotgun.h"

namespace RRTestHarness
{
	/** Fixed tick of every test world. */
	constexpr float FrameSeconds = 1.f / 60.f;

	/** Blueprint classes used by several test files. */
	inline const TCHAR* const ArenaGridClassPath = TEXT("/Game/RogueRigs/Blueprints/Terrain/BP_ArenaGrid.BP_ArenaGrid_C");
	inline const TCHAR* const MinerClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_Miner.BP_Miner_C");
	inline const TCHAR* const ShotgunClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_Shotgun.BP_Shotgun_C");

	/** Rock cell of arena_m1 with a floor cell to its west; the grid checks below use it to prove the layout loaded. */
	inline const FIntPoint RockCell(43, 38);
	inline const FIntPoint RockWestFloorCell(42, 38);

	/** Legacy layout of the logic tests (80 x 80) and its cell size; the game uses arena_m1_fine at 67 uu since task-029-04. */
	inline const TCHAR* const LegacyLayoutName = TEXT("arena_m1");
	constexpr float LegacyCellSize = 200.f;

	/** Game layout (240 x 240, each legacy cell split 3 x 3, edges roughened) and its cell size. */
	inline const TCHAR* const FineLayoutName = TEXT("arena_m1_fine");
	constexpr float FineCellSize = 67.f;

	/** Center cell of the 3 x 3 fine cells a legacy cell became (same place in the world within half a percent). */
	inline FIntPoint FineCell(FIntPoint LegacyCell) { return FIntPoint(LegacyCell.X * 3 + 1, LegacyCell.Y * 3 + 1); }

	/** Rock cell of arena_m1_fine with a floor cell to its west (the west column of the legacy rock (43, 38), the roughening opened its east one). */
	inline const FIntPoint FineRockCell(129, 115);
	inline const FIntPoint FineRockWestFloorCell(128, 115);

	/** Transient game world ticked manually: actors begin play on spawn, no game mode, no local player. Destroyed with the scope. */
	class FTestWorld
	{
	public:
		/** Creates, initializes and begins play on a game world named Name. */
		explicit FTestWorld(const TCHAR* Name = TEXT("RRTestWorld"))
		{
			// Spawners in test worlds do not prewarm the pools on their own (pool counters stay exact); prewarm tests call
			// ARREnemySpawner::QueuePoolPrewarm or set rr.Pool.Prewarm back to 1 (task-111-01).
			if (IConsoleVariable* Prewarm = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.Pool.Prewarm")))
			{
				PreviousPoolPrewarm = Prewarm->GetInt();
				Prewarm->Set(0, ECVF_SetByCode);
			}
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false, Name);
			FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
			Context.SetCurrentWorld(World);
			World->InitializeActorsForPlay(FURL());
			// The arrival extractor clears the blocks of its footprint next to the miner (task-372-01): test scenes are built there.
			if (URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>())
			{
				RunFlow->SetArrivalClearsFootprint(false);
			}
			World->BeginPlay();
			// No game mode: dispatch BeginPlay ourselves; actors spawned afterwards begin play on spawn.
			World->GetWorldSettings()->NotifyBeginPlay();
		}

		/** Ends play and destroys the world and its context. */
		~FTestWorld()
		{
			World->EndPlay(EEndPlayReason::Quit);
			World->DestroyWorld(/*bInformEngineOfWorld*/ false);
			GEngine->DestroyWorldContext(World);
			if (IConsoleVariable* Prewarm = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.Pool.Prewarm")); Prewarm && PreviousPoolPrewarm != INDEX_NONE)
			{
				Prewarm->Set(PreviousPoolPrewarm, ECVF_SetByCode);
			}
		}

		/** The world. */
		UWorld* Get() const { return World; }

		/**
		 * Advances the world by one fixed frame. The tick task manager runs a tick function at most once per
		 * GFrameCounter value, and a synchronous test runs inside one engine frame: advance the counter per simulated frame.
		 */
		void Tick() const
		{
			++GFrameCounter;
			World->Tick(LEVELTICK_All, FrameSeconds);
		}

		/** Spawns Class at Location (rotation Rotation), ignoring collisions. */
		template <typename T>
		T* Spawn(UClass* Class, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator) const
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			return World->SpawnActor<T>(Class, Location, Rotation, Params);
		}

	protected:
		/** The transient world. */
		UWorld* World = nullptr;

		/** rr.Pool.Prewarm before this world (restored on destruction); INDEX_NONE when the variable was not found. */
		int32 PreviousPoolPrewarm = INDEX_NONE;
	};

	/** Loads a Blueprint class, failing the test when it is missing. */
	template <typename T>
	UClass* LoadTestClass(FAutomationTestBase& Test, const TCHAR* Path)
	{
		UClass* Class = LoadClass<T>(nullptr, Path);
		Test.TestNotNull(FString::Printf(TEXT("class %s loads"), Path), Class);
		return Class;
	}

	/**
	 * Spawns BP_ArenaGrid at the origin with LayoutName and CellSize set before its construction script and BeginPlay, and
	 * checks it built (CheckCell is not void). Used by SpawnGrid (legacy 200 uu layout) and SpawnFineGrid (game defaults).
	 */
	inline ARRArenaGrid* SpawnGridWithLayout(FAutomationTestBase& Test, const FTestWorld& TestWorld, const TCHAR* LayoutName, float CellSize, FIntPoint CheckCell)
	{
		UClass* GridClass = LoadTestClass<ARRArenaGrid>(Test, ArenaGridClassPath);
		if (!GridClass)
		{
			return nullptr;
		}
		const FTransform Origin(FVector::ZeroVector);
		ARRArenaGrid* Grid = TestWorld.Get()->SpawnActorDeferred<ARRArenaGrid>(GridClass, Origin, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Test.TestNotNull(TEXT("grid spawned"), Grid))
		{
			return nullptr;
		}
		RRTestAccess::FGridAccess::LayoutNameOf(Grid) = LayoutName;
		RRTestAccess::FGridAccess::CellSizeOf(Grid) = CellSize;
		Grid->FinishSpawning(Origin);
		Test.TestTrue(TEXT("grid has begun play"), Grid->HasActorBegunPlay());
		Test.TestTrue(*FString::Printf(TEXT("grid layout %s loaded"), LayoutName), Grid->GetCellType(CheckCell) != ERRCellType::Void);
		return Grid;
	}

	/**
	 * Spawns BP_ArenaGrid at the origin on the legacy layout arena_m1 (80 x 80) at LegacyCellSize (200 uu). Logic tests keep it:
	 * their cells were chosen on that layout and their rules do not depend on the cell size. Every other grid property keeps
	 * the game default (block health, drops). Tests of cell-counted tunables use SpawnFineGrid.
	 */
	inline ARRArenaGrid* SpawnGrid(FAutomationTestBase& Test, const FTestWorld& TestWorld)
	{
		return SpawnGridWithLayout(Test, TestWorld, LegacyLayoutName, LegacyCellSize, RockCell);
	}

	/** Spawns BP_ArenaGrid at the origin with the game layout and cell size (arena_m1_fine, 240 x 240 cells of 67 uu). */
	inline ARRArenaGrid* SpawnFineGrid(FAutomationTestBase& Test, const FTestWorld& TestWorld)
	{
		return SpawnGridWithLayout(Test, TestWorld, FineLayoutName, FineCellSize, FineCell(RockCell));
	}

	/**
	 * Spawns BP_Miner standing on the floor of Cell (its BeginPlay spawns the drone and the starting weapons). The spawn is
	 * deferred: BeforeBeginPlay runs on the new miner before its construction script and BeginPlay (e.g. to override a property).
	 */
	inline ARRMiner* SpawnMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, const ARRArenaGrid* Grid, FIntPoint Cell,
		TFunctionRef<void(ARRMiner*)> BeforeBeginPlay = [](ARRMiner*) {})
	{
		UClass* MinerClass = LoadTestClass<ARRMiner>(Test, MinerClassPath);
		if (!MinerClass)
		{
			return nullptr;
		}
		const float HalfHeight = MinerClass->GetDefaultObject<ARRMiner>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRMiner* Miner = TestWorld.Get()->SpawnActorDeferred<ARRMiner>(MinerClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("miner spawned"), Miner))
		{
			BeforeBeginPlay(Miner);
			Miner->FinishSpawning(SpawnTransform);
		}
		return Miner;
	}

	/** MaxHealth and starting Health of a still chaser (the retired target dummy's value, which the hit tests were measured with). */
	constexpr float StillChaserHealth = 1000.f;

	/**
	 * Spawns a native ARRChaser as an immobile hit target (replaces the target dummy, task-031-01): feet on the floor at
	 * FloorLocation, no XP pickup class, actor tick off (no behavior, no attack), CharacterMovement and flow-field movement off,
	 * MaxHealth = Health = Health. Its capsule (radius 24, half height 62) overlaps projectiles; hits go through the health set
	 * and the damage numbers as on any robot. Death stays enabled: a lethal hit kills it, but the shrink never runs (no tick).
	 */
	inline ARRChaser* SpawnStillChaser(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation, float Health = StillChaserHealth)
	{
		const float HalfHeight = GetDefault<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight));
		ARRChaser* Chaser = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ARRChaser::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Test.TestNotNull(TEXT("still chaser spawned"), Chaser))
		{
			return nullptr;
		}
		Chaser->FinishSpawning(SpawnTransform);
		Chaser->SetActorTickEnabled(false);
		Chaser->GetCharacterMovement()->SetComponentTickEnabled(false);
		Chaser->GetEnemyMovement()->SetMovementEnabled(false);
		Chaser->GetEnemyMovement()->SetComponentTickEnabled(false);
		// The walking floor check at BeginPlay keeps the capsule about 2.4 uu above the floor: put the feet back on it.
		Chaser->SetActorLocation(SpawnTransform.GetLocation(), /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
		UAbilitySystemComponent* ASC = Chaser->GetAbilitySystemComponent();
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), Health);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), Health);
		return Chaser;
	}

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/**
	 * SpawnMiner hook: the miner starts with the burst rifle only (shotgun entries removed from StartingWeapons, no guaranteed
	 * class). For tests whose seeded draws or counts were measured with a single weapon (upgrade rules).
	 */
	inline void KeepRifleOnly(ARRMiner* Miner)
	{
		FMinerAccess::StartingWeaponsOf(Miner).RemoveAll([](const TSubclassOf<ARRWeapon>& Entry)
		{
			return !Entry || Entry->IsChildOf(ARRShotgun::StaticClass());
		});
		FMinerAccess::GuaranteedWeaponClassesOf(Miner).Reset();
		// Seeded offers of these tests were measured without new-weapon cards (task-024-04 adds them).
		FUpgradeAccess::WeaponCardLevelIntervalOf(Miner->GetUpgrades()) = 0;
	}

	/** SpawnMiner hook: offers never contain a new-weapon card (level interval 0), so seeded stat draws match the pre-024-04 measures. */
	inline void DisableWeaponCards(ARRMiner* Miner)
	{
		FUpgradeAccess::WeaponCardLevelIntervalOf(Miner->GetUpgrades()) = 0;
	}

	/**
	 * SpawnMiner hook: BP_Shotgun (native ARRShotgun if it fails to load) appended to StartingWeapons unless an entry is already
	 * a shotgun. BP_Miner starts with the rifle only since task-024-04; tests measured with the rifle + shotgun loadout add it here.
	 */
	inline void AddStartingShotgun(ARRMiner* Miner)
	{
		TArray<TSubclassOf<ARRWeapon>>& Starting = FMinerAccess::StartingWeaponsOf(Miner);
		const bool bHasShotgun = Starting.ContainsByPredicate([](const TSubclassOf<ARRWeapon>& Entry)
		{
			return Entry && Entry->IsChildOf(ARRShotgun::StaticClass());
		});
		if (!bHasShotgun)
		{
			UClass* ShotgunClass = LoadClass<ARRShotgun>(nullptr, ShotgunClassPath);
			Starting.Add(ShotgunClass ? ShotgunClass : ARRShotgun::StaticClass());
		}
	}

	/** SpawnMiner hook: the loadout the tests before task-024-04 were measured with: rifle + shotgun (AddStartingShotgun), no new-weapon cards. */
	inline void M1TestLoadout(ARRMiner* Miner)
	{
		AddStartingShotgun(Miner);
		DisableWeaponCards(Miner);
	}

	/** The miner's first weapon that is not a shotgun (the burst rifle); null when none. */
	inline ARRWeapon* FindRifle(const ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && !Weapon->IsA<ARRShotgun>())
			{
				return Weapon;
			}
		}
		return nullptr;
	}

	/** The miner's shotgun; null when it has none. */
	inline ARRShotgun* FindShotgun(const ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (ARRShotgun* Shotgun = Cast<ARRShotgun>(Weapon))
			{
				return Shotgun;
			}
		}
		return nullptr;
	}

	/** Stops the miner's shotgun auto-fire (rifle tests keep the M1 loadout but must not hit robots with the wave). */
	inline void StopShotgun(const ARRMiner* Miner)
	{
		if (ARRShotgun* Shotgun = FindShotgun(Miner))
		{
			Shotgun->GetAutoFireComponent()->StopFiring();
		}
	}
}

#endif // WITH_DEV_AUTOMATION_TESTS
