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

		static const TArray<FRRGridPieceVisual>& PieceVisualsOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::PieceVisuals); }
		static const TArray<int32>& CornerVisuals(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CornerPieceVisual); }
		static const TArray<int32>& CornerInstances(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CornerPieceInstance); }
		static FRRRockPieceMeshes& RockPieceMeshesOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::RockPieceMeshes); }
		static FRRRockPieceMeshes& RockPieceBrokenMeshesOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::RockPieceBrokenMeshes); }
		static UStaticMesh* OreCrystalMeshOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OreCrystalMesh); }
		static float MarkedValueOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::MarkedCustomDataValue); }
		static TArray<TObjectPtr<UStaticMesh>>& OreDepositMeshesOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OreDepositMeshes); }
		static TArray<TObjectPtr<UStaticMesh>>& HealDepositMeshesOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::HealDepositMeshes); }
		static TObjectPtr<UMaterialInterface>& OreDepositMaterialOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OreDepositMaterial); }
		static FName& DepositCrystalSlotNameOf(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::DepositCrystalSlotName); }
		static const TArray<FRRGridDepositVisual>& DepositVisualsOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::DepositVisuals); }
		static bool UsesDepositCrystalsOf(const ARRArenaGrid* Grid, ERRCellType Type) { return (Grid->*(&FGridAccess::UsesDepositCrystals))(Type); }
		static FTransform DepositCrystalRestOf(const ARRArenaGrid* Grid, int32 DepositId) { return (Grid->*(&FGridAccess::GetDepositCrystalTransform))(DepositId); }

		/** Deposit crystal ISM of DepositId and its instance (task-234-03); nullptr when the deposit has no crystal. */
		static UInstancedStaticMeshComponent* DepositCrystalOf(const ARRArenaGrid* Grid, int32 DepositId, int32& OutInstance)
		{
			return (Grid->*(&FGridAccess::GetDepositCrystalComponent))(DepositId, OutInstance);
		}

		/** Deposit crystal instances over every deposit crystal ISM of Type. */
		static int32 DepositCrystalCount(const ARRArenaGrid* Grid, ERRCellType Type)
		{
			int32 Count = 0;
			for (const FRRGridDepositVisual& Visual : DepositVisualsOf(Grid))
			{
				Count += Visual.CellType == Type && Visual.Render ? Visual.Render->GetInstanceCount() : 0;
			}
			return Count;
		}

		/** Piece visual of Corner (nullptr for an empty corner or one outside the grid). */
		static const FRRGridPieceVisual* CornerPiece(const ARRArenaGrid* Grid, FIntPoint Corner)
		{
			if (Corner.X < 0 || Corner.Y < 0 || Corner.X > Width(Grid) || Corner.Y > Height(Grid))
			{
				return nullptr;
			}
			const int32 CornerIndex = Corner.Y * (Width(Grid) + 1) + Corner.X;
			const int32 VisualIndex = CornerVisuals(Grid).IsValidIndex(CornerIndex) ? CornerVisuals(Grid)[CornerIndex] : INDEX_NONE;
			return VisualIndex != INDEX_NONE ? &PieceVisualsOf(Grid)[VisualIndex] : nullptr;
		}

		/** Custom data float DataIndex of the piece at Corner (task-230-08 layout, ARRArenaGrid::Piece*CustomDataIndex); -1 when unreadable. */
		static float PieceData(const ARRArenaGrid* Grid, FIntPoint Corner, int32 DataIndex)
		{
			const FRRGridPieceVisual* Visual = CornerPiece(Grid, Corner);
			if (!Visual || !Visual->Render)
			{
				return -1.f;
			}
			const int32 Instance = CornerInstances(Grid)[Corner.Y * (Width(Grid) + 1) + Corner.X];
			const int32 Index = Instance * Visual->Render->NumCustomDataFloats + DataIndex;
			return DataIndex < Visual->Render->NumCustomDataFloats && Visual->Render->PerInstanceSMCustomData.IsValidIndex(Index) ? Visual->Render->PerInstanceSMCustomData[Index] : -1.f;
		}

		/** Quadrant of Cell around Corner (q = (+X side) + 2 * (+Y side)); INDEX_NONE when Cell is not one of the 4 cells of Corner. */
		static int32 QuadrantOf(FIntPoint Corner, FIntPoint Cell)
		{
			const int32 DX = Cell.X - Corner.X;
			const int32 DY = Cell.Y - Corner.Y;
			if (DX < -1 || DX > 0 || DY < -1 || DY > 0)
			{
				return INDEX_NONE;
			}
			return (DX == 0 ? 1 : 0) + (DY == 0 ? 2 : 0);
		}

		/** Smallest marked quadrant float of Cell over the pieces of its 4 corners (rock piece outline, task-230-08); -1 when a corner has no readable piece. */
		static float GetPieceMarkedData(const ARRArenaGrid* Grid, FIntPoint Cell)
		{
			float Smallest = TNumericLimits<float>::Max();
			for (int32 DY = 0; DY <= 1; ++DY)
			{
				for (int32 DX = 0; DX <= 1; ++DX)
				{
					const FIntPoint Corner(Cell.X + DX, Cell.Y + DY);
					Smallest = FMath::Min(Smallest, PieceData(Grid, Corner, ARRArenaGrid::PieceMarkedCustomDataIndex + QuadrantOf(Corner, Cell)));
				}
			}
			return Smallest;
		}
		static bool BlocksFillFloorSlabOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::bBlocksFillFloorSlab); }
		static bool PieceImportFlipsYOf(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::bRockPieceImportFlipsY); }
		static void Rebuild(ARRArenaGrid* Grid) { (Grid->*(&FGridAccess::RebuildFromLayout))(); }

		/** Block instances of a visual type, summed over its chunk components: the rendering HISM (crystals for ore and heal ore), or the collision ISM for rock and special ore (no per-cell rendering since task-230-07). */
		static int32 InstanceCount(const ARRArenaGrid* Grid, ERRCellType Key)
		{
			int32 Count = 0;
			for (const FRRGridChunkVisual& Visual : Visuals(Grid))
			{
				if (Visual.MapKey == Key)
				{
					Count += Visual.Render ? Visual.Render->GetInstanceCount() : (Visual.Collision ? Visual.Collision->GetInstanceCount() : 0);
				}
			}
			return Count;
		}

		/** Rock piece instances over every piece ISM. */
		static int32 PieceInstanceCount(const ARRArenaGrid* Grid)
		{
			int32 Count = 0;
			for (const FRRGridPieceVisual& Visual : PieceVisualsOf(Grid))
			{
				Count += Visual.Render ? Visual.Render->GetInstanceCount() : 0;
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

	/** ARRDrone: the laser mesh, the arc block cap, dig hover values, the idle gesture and the patrol state (task-251-01). */
	struct FDroneAccess : public ARRDrone
	{
		static const UStaticMeshComponent* Laser(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserMesh); }
		static int32& MaxArcBlocksOf(ARRDrone* Drone) { return Drone->*(&FDroneAccess::MaxArcBlocks); }
		static float BeamBaseHeight(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserMeshBaseHeight); }
		static float DigHoverFaceDistanceOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::DigHoverFaceDistance); }
		static float DigHoverHeightOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::DigHoverHeight); }
		static FVector2D PatrolOffsetOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::PatrolOffset); }
		static bool PatrolFallbackOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::bPatrolFallback); }
		static float PatrolMinRadiusOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::PatrolMinRadius); }
		static float PatrolMaxRadiusOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::PatrolMaxRadius); }
		static float PatrolMaxIntervalOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::PatrolMaxInterval); }
		static bool IsPatrolOffsetClearOf(const ARRDrone* Drone, const ARRArenaGrid* Grid, const FVector2D& Offset) { return (Drone->*(&FDroneAccess::IsPatrolOffsetClear))(Grid, Offset); }

		/** Reseeds the patrol stream with Seed and restarts the patrol (new point, drone snapped to it), so a test gets the same picks twice. */
		static void RestartPatrolOf(ARRDrone* Drone, int32 Seed)
		{
			(Drone->*(&FDroneAccess::PatrolRandom)).Initialize(Seed);
			(Drone->*(&FDroneAccess::RestartPatrol))();
		}
		static ERRDroneIdleGesture IdleGestureOf(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::IdleGesture); }
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

	/** ARRPickup: the pop start location and the rest orientation drawn each life (task-192-03). */
	struct FPickupAccess : public ARRPickup
	{
		static const FVector& PopStartOf(const ARRPickup* Pickup) { return Pickup->*(&FPickupAccess::PopStart); }
		static ERRPickupRestOrientation RestOrientationOf(const ARRPickup* Pickup) { return Pickup->*(&FPickupAccess::RestOrientation); }
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
