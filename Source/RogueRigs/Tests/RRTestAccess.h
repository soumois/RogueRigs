// Rogue Rigs - test-only access to protected members of gameplay classes, gathered in one place (namespace RRTestAccess).

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
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
	/** ARRArenaGrid: chunk visuals and instance maps, grid size, shake and drop state, pickup classes, marked custom data. */
	struct FGridAccess : public ARRArenaGrid
	{
		static const TArray<FRRGridChunkVisual>& Visuals(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::ChunkVisuals); }
		static const TArray<int32>& InstanceIndices(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CellInstanceIndex); }
		static const TArray<int32>& FloorInstanceIndices(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CellFloorInstanceIndex); }
		static const TMap<FIntPoint, float>& Shaking(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::ShakingCells); }
		static int32 Width(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridWidth); }
		static int32 Height(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridHeight); }
		static int32 ChunkSide(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::ChunkCells); }
		static int32 DropsOnDestroy(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::DestroyDropCount); }
		static float DropIntervalOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::DropInterval); }
		static FString& LayoutNameOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::LayoutName); }
		static float& CellSizeOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CellSize); }
		static TSubclassOf<ARRPickup>& HealClass(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::HealPickupClass); }
		static TSubclassOf<ARRPickup> OreClass(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OrePickupClass); }
		static TSubclassOf<ARRPickup>& OreClassRef(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OrePickupClass); }
		static bool& PlaceOreDepositsOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::bPlaceOreDeposits); }
		static int32& OreSeedOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OreSeed); }

		/** Rendering instances of a visual type, summed over its chunk HISMs. */
		static int32 InstanceCount(const ARRArenaGrid* Grid, ERRCellType Key)
		{
			int32 Count = 0;
			for (const FRRGridChunkVisual& Visual : Visuals(Grid))
			{
				Count += Visual.MapKey == Key && Visual.Render ? Visual.Render->GetInstanceCount() : 0;
			}
			return Count;
		}

		/** Chunk HISM rendering the current instance of Cell; nullptr when none. */
		static UHierarchicalInstancedStaticMeshComponent* CellComponent(const ARRArenaGrid* Grid, FIntPoint Cell)
		{
			return (Grid->*(&FGridAccess::GetCellRenderComponent))(Cell);
		}

		/** Hidden collision ISM of the chunk visual holding the block in Cell; nullptr for a floor or void cell. */
		static UInstancedStaticMeshComponent* CellCollisionComponent(const ARRArenaGrid* Grid, FIntPoint Cell)
		{
			const int32 Index = (Grid->*(&FGridAccess::GetVisualIndex))((Grid->*(&FGridAccess::GetCellMapKey))(Cell), Cell);
			return Index != INDEX_NONE ? Visuals(Grid)[Index].Collision.Get() : nullptr;
		}

		/** Marked custom data float of the rendering instance of the block in Cell; -1 when unreadable. */
		static float GetMarkedData(const ARRArenaGrid* Grid, FIntPoint Cell)
		{
			UHierarchicalInstancedStaticMeshComponent* BlockComponent = Grid->IsBlockCell(Cell) ? CellComponent(Grid, Cell) : nullptr;
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

	/** ARRDrone: the laser mesh and the arc block cap. */
	struct FDroneAccess : public ARRDrone
	{
		static const UStaticMeshComponent* Laser(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserMesh); }
		static int32& MaxArcBlocksOf(ARRDrone* Drone) { return Drone->*(&FDroneAccess::MaxArcBlocks); }
		static const TArray<TObjectPtr<UStaticMeshComponent>>& Beams(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserBeams); }
		static float BeamBaseHeight(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserMeshBaseHeight); }
	};

	/** ARREnemy: XP pickup class (tests drop ARRTestPickupXP) and the CSV stats copy. */
	struct FEnemyAccess : public ARREnemy
	{
		static TSubclassOf<ARRPickup>& XPPickupClassOf(ARREnemy* Enemy) { return Enemy->*(&FEnemyAccess::XPPickupClass); }
		static FRREnemyStatsRow& StatsOf(ARREnemy* Enemy) { return Enemy->*(&FEnemyAccess::Stats); }
	};

	/** ARRShooter: laser component, shot class, flat damage spec. */
	struct FShooterAccess : public ARRShooter
	{
		static UStaticMeshComponent* LaserOf(const ARRShooter* Shooter) { return Shooter->*(&FShooterAccess::LaserBeam); }
		static TSubclassOf<ARRProjectile> ProjectileClassOf(const ARRShooter* Shooter) { return Shooter->*(&FShooterAccess::ProjectileClass); }
		static FGameplayEffectSpecHandle DamageSpecOf(const ARRShooter* Shooter)
		{
			return (Shooter->*(&FShooterAccess::MakeDamageSpec))(Shooter->*(&FShooterAccess::DamageEffectClass), Shooter->GetStats().Damage);
		}
	};

	/** ARREnemySpawner: class, weights, every pacing tunable, the director data and the horde plan. */
	struct FSpawnerAccess : public ARREnemySpawner
	{
		static TSubclassOf<ARREnemy>& EnemyClassOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::EnemyClass); }
		static float& SpawnIntervalOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SpawnInterval); }
		static int32& MaxAliveOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::MaxAlive); }
		static float& BandMinOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SpawnBandMinDistance); }
		static float& BandMaxOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SpawnBandMaxDistance); }
		static float& BandFallbackMaxOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SpawnBandFallbackMaxDistance); }
		static float& HeadingMinSpeedOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::HeadingMinSpeed); }
		static float& HeadingWindowOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::HeadingWindowSeconds); }
		static float& FrontWeightOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::FrontSectorWeight); }
		static float& SideWeightOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SideSectorWeight); }
		static float& BackWeightOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::BackSectorWeight); }
		static float& FrontHalfAngleOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::FrontHalfAngle); }
		static float& BackHalfAngleOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::BackHalfAngle); }
		static float& TrickleDurationOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::TrickleDuration); }
		static float& PackIntervalOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::PackInterval); }
		static int32& PackSizeMinOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::PackSizeMin); }
		static int32& PackSizeMaxOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::PackSizeMax); }
		static float& LoneSpawnChanceOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::LoneSpawnChance); }
		static TArray<FRRWeightedEnemyClass>& WeightsOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::WeightedEnemyClasses); }
		static FRRWaveDirectorConfig& DirectorOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::DirectorConfig); }
		static const TArray<FRRHordeSlot>& HordePlanOf(const ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::HordePlan); }
		static int32 HordeSpawnedOf(const ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::HordeSpawned); }
		static ERRSpawnSide PrepareHordeOf(ARREnemySpawner* Spawner, int32 HordeIndex) { return (Spawner->*(&FSpawnerAccess::PrepareHorde))(HordeIndex); }
		static bool& RecycleEnabledOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::bRecycleEnabled); }
		static float& RecycleDistanceOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::RecycleDistance); }
		static float& RecycleDelayOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::RecycleDelay); }
		static float& RecycleMarginOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::RecycleOffScreenMargin); }
		static float& RecycleUnseenDelayOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::RecycleUnseenDelay); }
	};

	/** ARRExplosionFX: growth duration (Duration). */
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
		static TSubclassOf<ARRDrone>& DroneClassOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::DroneClass); }
		static FVector& PendingMoveInputOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::PendingMoveInput); }
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
		static int32& WeaponCardLevelIntervalOf(URRUpgradeComponent* Upgrades) { return Upgrades->*(&FUpgradeAccess::WeaponCardLevelInterval); }
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

	/** UUserWidget: its native tick (latent actions, then the Blueprint Event Tick), which headless tests never get from Slate. */
	struct FUserWidgetAccess : public UUserWidget
	{
		/** Runs Widget's NativeTick (virtual: the subclass override runs) with its cached geometry (empty headless; SlateCore is not a module dependency). */
		static void NativeTickOf(UUserWidget* Widget, float DeltaTime) { (Widget->*(&FUserWidgetAccess::NativeTick))(Widget->GetCachedGeometry(), DeltaTime); }

		/**
		 * Runs NativeConstruct (then the Blueprint Construct) of every user widget nested in Root, children first, as Slate does when
		 * the HUD is added to the viewport in game; headless tests never build Slate widgets. The HUD panels bind their buttons there
		 * (WBP_AlertObjectives RestartButton). Root itself is not constructed (unchanged headless behaviour of the root HUD).
		 */
		static void ConstructNestedOf(UUserWidget* Root)
		{
			if (!Root || !Root->WidgetTree)
			{
				return;
			}
			TArray<UUserWidget*> Children;
			Root->WidgetTree->ForEachWidget([&Children](UWidget* Widget)
			{
				if (UUserWidget* Child = Cast<UUserWidget>(Widget))
				{
					Children.Add(Child);
				}
			});
			for (UUserWidget* Child : Children)
			{
				ConstructNestedOf(Child);
				(Child->*(&FUserWidgetAccess::NativeConstruct))();
			}
		}
	};
}

#endif // WITH_DEV_AUTOMATION_TESTS
