// Rogue Rigs - shared harness of the automation tests: transient game world ticked manually, Blueprint class loading, grid and miner spawning.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Grid/RRArenaGrid.h"
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

	/** Rock cell of arena_m1 with a floor cell to its west; the grid checks below use it to prove the layout loaded. */
	inline const FIntPoint RockCell(43, 38);
	inline const FIntPoint RockWestFloorCell(42, 38);

	/** Transient game world ticked manually: actors begin play on spawn, no game mode, no local player. Destroyed with the scope. */
	class FTestWorld
	{
	public:
		/** Creates, initializes and begins play on a game world named Name. */
		explicit FTestWorld(const TCHAR* Name = TEXT("RRTestWorld"))
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false, Name);
			FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
			Context.SetCurrentWorld(World);
			World->InitializeActorsForPlay(FURL());
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
	};

	/** Loads a Blueprint class, failing the test when it is missing. */
	template <typename T>
	UClass* LoadTestClass(FAutomationTestBase& Test, const TCHAR* Path)
	{
		UClass* Class = LoadClass<T>(nullptr, Path);
		Test.TestNotNull(FString::Printf(TEXT("class %s loads"), Path), Class);
		return Class;
	}

	/** Spawns BP_ArenaGrid at the origin and checks it rebuilt from arena_m1 at BeginPlay. */
	inline ARRArenaGrid* SpawnGrid(FAutomationTestBase& Test, const FTestWorld& TestWorld)
	{
		UClass* GridClass = LoadTestClass<ARRArenaGrid>(Test, ArenaGridClassPath);
		if (!GridClass)
		{
			return nullptr;
		}
		ARRArenaGrid* Grid = TestWorld.Spawn<ARRArenaGrid>(GridClass, FVector::ZeroVector);
		if (!Test.TestNotNull(TEXT("grid spawned"), Grid))
		{
			return nullptr;
		}
		Test.TestTrue(TEXT("grid has begun play"), Grid->HasActorBegunPlay());
		Test.TestTrue(TEXT("grid layout loaded"), Grid->GetCellType(RockCell) != ERRCellType::Void);
		return Grid;
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

	/** Read and write access to the miner's protected loadout lists. Never instantiated. */
	struct FMinerLoadoutAccess : public ARRMiner
	{
		static TArray<TSubclassOf<ARRWeapon>>& StartingWeaponsOf(ARRMiner* Miner) { return Miner->*(&FMinerLoadoutAccess::StartingWeapons); }
		static TArray<TSubclassOf<ARRWeapon>>& GuaranteedWeaponClassesOf(ARRMiner* Miner) { return Miner->*(&FMinerLoadoutAccess::GuaranteedWeaponClasses); }
	};

	/**
	 * SpawnMiner hook: the miner starts with the burst rifle only (shotgun entries removed from StartingWeapons, no guaranteed
	 * class). For tests whose seeded draws or counts were measured with a single weapon (upgrade rules).
	 */
	inline void KeepRifleOnly(ARRMiner* Miner)
	{
		FMinerLoadoutAccess::StartingWeaponsOf(Miner).RemoveAll([](const TSubclassOf<ARRWeapon>& Entry)
		{
			return !Entry || Entry->IsChildOf(ARRShotgun::StaticClass());
		});
		FMinerLoadoutAccess::GuaranteedWeaponClassesOf(Miner).Reset();
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
