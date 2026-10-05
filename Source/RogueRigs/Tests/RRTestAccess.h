// Rogue Rigs - test-only access to protected members of gameplay classes, gathered in one place (namespace RRTestAccess).

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Debug/RRTargetDummy.h"
#include "Drone/RRDrone.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRShooter.h"
#include "Engine/GameInstance.h"
#include "Explosions/RRExplosionFX.h"
#include "Grid/RRArenaGrid.h"
#include "Pickups/RRPickup.h"
#include "Player/RRGameMode.h"
#include "InputActionValue.h"
#include "Player/RRMiner.h"
#include "Player/RRPlayerController.h"
#include "Projectiles/RRProjectile.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRWeapon.h"

/**
 * The idiom: gameplay classes keep their state protected (no test hook in shipping code, see cpp-standard). A struct derived
 * from the class may name a protected member, so `&FGridAccess::GridWidth` is a legal pointer to member of ARRArenaGrid;
 * applying it to any instance (`Grid->*(&FGridAccess::GridWidth)`) reads or writes that member without a cast. The structs
 * are never instantiated: they only hold static accessors. Add a member here instead of making gameplay state public.
 */
namespace RRTestAccess
{
	/** ARRArenaGrid: instance maps and components, grid size, shake and drop state, pickup classes, marked custom data. */
	struct FGridAccess : public ARRArenaGrid
	{
		static const TMap<ERRCellType, FRRInstanceCellMap>& Maps(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::InstanceCellMaps); }
		static const TArray<int32>& InstanceIndices(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CellInstanceIndex); }
		static const TMap<FIntPoint, float>& Shaking(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::ShakingCells); }
		static int32 Width(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridWidth); }
		static int32 Height(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridHeight); }
		static int32 DropsOnDestroy(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::DestroyDropCount); }
		static TSubclassOf<ARRPickup>& HealClass(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::HealPickupClass); }
		static TSubclassOf<ARRPickup> OreClass(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OrePickupClass); }

		/** Rendering HISM of a cell type; nullptr for an unknown type. */
		static UHierarchicalInstancedStaticMeshComponent* Component(const ARRArenaGrid* Grid, ERRCellType Key)
		{
			switch (Key)
			{
			case ERRCellType::Floor:      return Grid->*(&FGridAccess::FloorInstances);
			case ERRCellType::Rock:       return Grid->*(&FGridAccess::RockInstances);
			case ERRCellType::Ore:        return Grid->*(&FGridAccess::OreInstances);
			case ERRCellType::HealOre:    return Grid->*(&FGridAccess::HealOreInstances);
			case ERRCellType::SpecialOre: return Grid->*(&FGridAccess::SpecialOreInstances);
			case ERRCellType::Barrel:     return Grid->*(&FGridAccess::BarrelInstances);
			default:                      return nullptr;
			}
		}

		/** Hidden collision ISM of a block type; nullptr for Floor (the floor HISM collides itself). */
		static UInstancedStaticMeshComponent* CollisionComponent(const ARRArenaGrid* Grid, ERRCellType Key)
		{
			switch (Key)
			{
			case ERRCellType::Rock:       return Grid->*(&FGridAccess::RockCollision);
			case ERRCellType::Ore:        return Grid->*(&FGridAccess::OreCollision);
			case ERRCellType::HealOre:    return Grid->*(&FGridAccess::HealOreCollision);
			case ERRCellType::SpecialOre: return Grid->*(&FGridAccess::SpecialOreCollision);
			case ERRCellType::Barrel:     return Grid->*(&FGridAccess::BarrelCollision);
			default:                      return nullptr;
			}
		}

		/** Marked custom data float of the rendering instance of the block in Cell; -1 when unreadable. */
		static float GetMarkedData(const ARRArenaGrid* Grid, FIntPoint Cell)
		{
			UHierarchicalInstancedStaticMeshComponent* BlockComponent = (Grid->*(&FGridAccess::GetBlockComponent))(Grid->GetCellType(Cell));
			const TArray<int32>& Indices = Grid->*(&FGridAccess::CellInstanceIndex);
			const int32 CellIndex = Cell.Y * Grid->GetGridSize().X + Cell.X;
			if (!BlockComponent || !Indices.IsValidIndex(CellIndex) || Indices[CellIndex] == INDEX_NONE)
			{
				return -1.f;
			}
			const int32 DataIndex = Indices[CellIndex] * BlockComponent->NumCustomDataFloats + Grid->GetMarkedCustomDataIndex();
			return BlockComponent->PerInstanceSMCustomData.IsValidIndex(DataIndex) ? BlockComponent->PerInstanceSMCustomData[DataIndex] : -1.f;
		}
	};

	/** ARRDrone: the laser mesh. */
	struct FDroneAccess : public ARRDrone
	{
		static const UStaticMeshComponent* Laser(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserMesh); }
	};

	/** ARREnemy: XP pickup class (tests drop ARRTestPickupXP) and the CSV stats copy. */
	struct FEnemyAccess : public ARREnemy
	{
		static TSubclassOf<ARRPickup>& XPPickupClassOf(ARREnemy* Enemy) { return Enemy->*(&FEnemyAccess::XPPickupClass); }
		static FRREnemyStatsRow& StatsOf(ARREnemy* Enemy) { return Enemy->*(&FEnemyAccess::Stats); }
	};

	/** ARRShooter: laser component, shot class, flat damage spec and the repositioning search. */
	struct FShooterAccess : public ARRShooter
	{
		static UStaticMeshComponent* LaserOf(const ARRShooter* Shooter) { return Shooter->*(&FShooterAccess::LaserBeam); }
		static TSubclassOf<ARRProjectile> ProjectileClassOf(const ARRShooter* Shooter) { return Shooter->*(&FShooterAccess::ProjectileClass); }
		static FGameplayEffectSpecHandle DamageSpecOf(const ARRShooter* Shooter)
		{
			return (Shooter->*(&FShooterAccess::MakeDamageSpec))(Shooter->*(&FShooterAccess::DamageEffectClass), Shooter->GetStats().Damage);
		}
		static bool FindPath(ARRShooter* Shooter, const ARRMiner* Target, TArray<FVector>& OutWaypoints, FIntPoint& OutCell)
		{
			return (Shooter->*(&FShooterAccess::FindRepositionPath))(Target, OutWaypoints, OutCell);
		}
	};

	/** ARREnemySpawner: class, weights and every pacing tunable. */
	struct FSpawnerAccess : public ARREnemySpawner
	{
		static TSubclassOf<ARREnemy>& EnemyClassOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::EnemyClass); }
		static float& SpawnIntervalOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SpawnInterval); }
		static int32& MaxAliveOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::MaxAlive); }
		static int32& ViewMarginOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::ViewMargin); }
		static int32& MaxSpawnDistanceOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::MaxSpawnDistanceCells); }
		static float& TrickleDurationOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::TrickleDuration); }
		static float& PackIntervalOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::PackInterval); }
		static int32& PackSizeMinOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::PackSizeMin); }
		static int32& PackSizeMaxOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::PackSizeMax); }
		static float& LoneSpawnChanceOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::LoneSpawnChance); }
		static TArray<FRRWeightedEnemyClass>& WeightsOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::WeightedEnemyClasses); }
	};

	/** ARRExplosionFX: visual duration. */
	struct FExplosionFXAccess : public ARRExplosionFX
	{
		static float DurationOf(const ARRExplosionFX* FX) { return FX->*(&FExplosionFXAccess::Duration); }
	};

	/** UGameInstance: the world context pointer (tests clear it before the world goes away). */
	struct FGameInstanceAccess : public UGameInstance
	{
		static FWorldContext*& WorldContextOf(UGameInstance* GameInstance) { return GameInstance->*(&FGameInstanceAccess::WorldContext); }
	};

	/** ARRPickup: the pop start location. */
	struct FPickupAccess : public ARRPickup
	{
		static const FVector& PopStartOf(const ARRPickup* Pickup) { return Pickup->*(&FPickupAccess::PopStart); }
	};

	/** ARRGameMode: the restart delay. */
	struct FGameModeAccess : public ARRGameMode
	{
		static float& Delay(ARRGameMode* GameMode) { return GameMode->*(&FGameModeAccess::RestartDelay); }
	};

	/** ARRMiner: loadout lists (RRTestHarness::KeepRifleOnly) and weapon attachment settings. */
	struct FMinerAccess : public ARRMiner
	{
		static TArray<TSubclassOf<ARRWeapon>>& StartingWeaponsOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::StartingWeapons); }
		static TArray<TSubclassOf<ARRWeapon>>& GuaranteedWeaponClassesOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::GuaranteedWeaponClasses); }
		static FName& WeaponAttachSocketNameOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::WeaponAttachSocketName); }
		static FVector WeaponAttachOffsetOf(const ARRMiner* Miner) { return Miner->*(&FMinerAccess::WeaponAttachOffset); }
		static TArray<FRRWeaponCatalogueEntry>& WeaponCatalogueOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::WeaponCatalogue); }
		static int32& MaxWeaponsOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::MaxWeapons); }
	};

	/** ARRPlayerController: the place-turret input path (action resolution and its Started handler). */
	struct FPlayerControllerAccess : public ARRPlayerController
	{
		/** The place-turret action the controller binds (PlaceTurretAction, else its transient action; no mapping context added). */
		static UInputAction* PlaceTurretActionOf(ARRPlayerController* Controller) { return (Controller->*(&FPlayerControllerAccess::ResolvePlaceTurretAction))(nullptr); }
		/** Runs the handler bound to the place-turret action on Started, as one button press does. */
		static void PressPlaceTurret(ARRPlayerController* Controller) { (Controller->*(&FPlayerControllerAccess::HandlePlaceTurret))(FInputActionValue(true)); }
	};

	/** URRUpgradeComponent: new-weapon card tunables (RRTestHarness::DisableWeaponCards, WeaponCards tests). */
	struct FUpgradeAccess : public URRUpgradeComponent
	{
		static float& NewWeaponCardChanceOf(URRUpgradeComponent* Upgrades) { return Upgrades->*(&FUpgradeAccess::NewWeaponCardChance); }
		static int32& MaxNewWeaponCardsPerOfferOf(URRUpgradeComponent* Upgrades) { return Upgrades->*(&FUpgradeAccess::MaxNewWeaponCardsPerOffer); }
	};

	/** ARRWeapon: the muzzle component. */
	struct FWeaponAccess : public ARRWeapon
	{
		static USceneComponent* MuzzleOf(const ARRWeapon* Weapon) { return Weapon->*(&FWeaponAccess::Muzzle); }
		static float& SpreadDegOf(ARRWeapon* Weapon) { return Weapon->*(&FWeaponAccess::SpreadDeg); }
	};

	/** ARRProjectile: the hit filter class. */
	struct FProjectileAccess : public ARRProjectile
	{
		static TSubclassOf<AActor>& HitActorClassOf(ARRProjectile* Projectile) { return Projectile->*(&FProjectileAccess::HitActorClass); }
	};

	/** ARRTargetDummy: its health set. */
	struct FDummyAccess : public ARRTargetDummy
	{
		static URRHealthAttributeSet* HealthSetOf(const ARRTargetDummy* Dummy) { return Dummy->*(&FDummyAccess::HealthSet); }
	};
}

#endif // WITH_DEV_AUTOMATION_TESTS
