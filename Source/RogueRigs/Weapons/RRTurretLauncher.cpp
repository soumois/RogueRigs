// Rogue Rigs - turret launcher: a miner weapon whose magazine is turret charges; a button places a turret on the aimed floor cell, charges recharge one at a time.

#include "Weapons/RRTurretLauncher.h"

#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "Projectiles/RRTurretBullet.h"
#include "RogueRigs.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRGE_ConsumeAmmo.h"
#include "Weapons/RRTurret.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "Weapons/RRWeaponStatsRow.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTurretLauncher)

ARRTurretLauncher::ARRTurretLauncher(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	StatsRowName = TEXT("TurretLauncher");
	ProjectileClass = ARRTurretBullet::StaticClass();
	TurretClass = ARRTurret::StaticClass();

	// Placement is always a player button (turret gdd): the auto-fire never runs on the launcher.
	bStartFiringOnAcquire = false;
	AutoFire->PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ARRTurretLauncher::ApplyStatsRow(const FRRWeaponStatsRow& Row)
{
	Super::ApplyStatsRow(Row);
	TurretLifetime = FMath::Max(Row.TurretLifetime, 0.f);
}

int32 ARRTurretLauncher::GetCharges() const
{
	const float Ammo = AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()) : 0.f;
	return FMath::Max(FMath::FloorToInt32(Ammo + KINDA_SMALL_NUMBER), 0);
}

int32 ARRTurretLauncher::GetMaxCharges() const
{
	const float Magazine = AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetMagazineSizeAttribute()) : 0.f;
	return FMath::Max(FMath::FloorToInt32(Magazine + KINDA_SMALL_NUMBER), 0);
}

float ARRTurretLauncher::GetRechargeDuration() const
{
	const float ReloadSpeed = AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetReloadSpeedAttribute()) : 0.f;
	return ReloadSpeed > 0.f ? ReloadTime / ReloadSpeed : 0.f;
}

float ARRTurretLauncher::GetRechargeProgress() const
{
	const float Duration = GetRechargeDuration();
	if (!bRecharging || Duration <= 0.f)
	{
		return 0.f;
	}
	return FMath::Clamp(1.f - RechargeRemaining / Duration, 0.f, 1.f);
}

ARRArenaGrid* ARRTurretLauncher::GetGrid() const
{
	if (OwnerMiner && OwnerMiner->GetArenaGrid())
	{
		return OwnerMiner->GetArenaGrid();
	}
	return RRWorld::FindArenaGrid(GetWorld());
}

bool ARRTurretLauncher::FindPlacementLocation(FVector& OutLocation) const
{
	const FVector Origin = OwnerMiner ? OwnerMiner->GetActorLocation() : GetActorLocation();
	const FVector Aim = GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const float MaxDistance = FMath::Max(PlacementMaxDistance, 0.f);

	// Aimed point: the cursor point with mouse aim, else PlacementDistance ahead; never beyond MaxDistance.
	float Desired = PlacementDistance;
	FVector CursorPoint;
	if (OwnerMiner && OwnerMiner->GetMouseAimPoint(CursorPoint))
	{
		Desired = static_cast<float>(FVector::Dist2D(Origin, CursorPoint));
	}
	Desired = FMath::Clamp(Desired, 0.f, MaxDistance);

	const ARRArenaGrid* Grid = GetGrid();
	if (!Grid)
	{
		const UCapsuleComponent* Capsule = OwnerMiner ? OwnerMiner->GetCapsuleComponent() : nullptr;
		const double FeetZ = Capsule ? Origin.Z - Capsule->GetScaledCapsuleHalfHeight() : Origin.Z;
		const FVector Point = Origin + Aim * Desired;
		OutLocation = FVector(Point.X, Point.Y, FeetZ);
		return true;
	}

	// A floor cell the miner sees (no turret behind a wall); the cell center on the floor top.
	auto TryCell = [Grid, &Origin](const FVector& Point, FVector& OutCenter)
	{
		const FIntPoint Cell = Grid->GetCellAtWorld(Point);
		if (Grid->GetCellType(Cell) != ERRCellType::Floor)
		{
			return false;
		}
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		if (!Grid->HasLineOfSight(Origin, Center))
		{
			return false;
		}
		OutCenter = Center;
		return true;
	};

	if (TryCell(Origin + Aim * Desired, OutLocation))
	{
		return true;
	}

	// Aimed cell blocked or hidden: the qualifying cell along the aim nearest to the aimed point.
	const float Step = FMath::Max(Grid->GetCellSize() * 0.5f, 1.f);
	bool bFound = false;
	float BestGap = TNumericLimits<float>::Max();
	for (float Distance = 0.f; Distance <= MaxDistance + KINDA_SMALL_NUMBER; Distance += Step)
	{
		const float Gap = FMath::Abs(Distance - Desired);
		FVector Center;
		if (Gap < BestGap && TryCell(Origin + Aim * Distance, Center))
		{
			BestGap = Gap;
			OutLocation = Center;
			bFound = true;
		}
	}
	return bFound;
}

ARRTurret* ARRTurretLauncher::TryPlaceTurret()
{
	UWorld* World = GetWorld();
	if (!World || GetCharges() < 1)
	{
		return nullptr;
	}
	if (!TurretClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: TurretClass is not set; no turret placed."), *GetName());
		return nullptr;
	}

	FVector Location;
	if (!FindPlacementLocation(Location))
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("%s: no floor cell to place a turret on."), *GetName());
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = OwnerMiner ? static_cast<APawn*>(OwnerMiner) : GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FRotator Rotation(0.f, GetAimDirection().Rotation().Yaw, 0.f);
	ARRTurret* Turret = World->SpawnActor<ARRTurret>(TurretClass, Location, Rotation, SpawnParameters);
	if (!Turret)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: turret of class %s could not be spawned."), *GetName(), *GetNameSafe(TurretClass));
		return nullptr;
	}

	// Stats are copied before the charge is spent: the turret's Ammo is irrelevant (it fires without magazine).
	Turret->InitFromLauncher(*this);

	AbilitySystemComponent->ApplyGameplayEffectToSelf(GetDefault<URRGE_ConsumeAmmo>(), 1.f, AbilitySystemComponent->MakeEffectContext());
	++TurretsPlaced;

	RefreshRecharge();
	BroadcastChargesIfChanged();
	OnTurretPlaced.Broadcast(Turret);
	return Turret;
}

void ARRTurretLauncher::RefreshRecharge()
{
	const bool bMissing = GetCharges() < GetMaxCharges();
	if (bMissing && !bRecharging)
	{
		bRecharging = true;
		RechargeRemaining = GetRechargeDuration();
	}
	else if (!bMissing)
	{
		bRecharging = false;
		RechargeRemaining = 0.f;
	}
}

void ARRTurretLauncher::AdvanceRecharge(float DeltaSeconds)
{
	// Also catches a MagazineSize upgrade (a new empty charge) or a magazine shrink.
	RefreshRecharge();

	float Remaining = FMath::Max(DeltaSeconds, 0.f);
	const int32 MaxChargesPerAdvance = 64;
	for (int32 Count = 0; bRecharging && Count < MaxChargesPerAdvance; ++Count)
	{
		// ReloadSpeed <= 0: the recharge cannot progress.
		if (AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetReloadSpeedAttribute()) <= 0.f)
		{
			break;
		}
		if (RechargeRemaining > Remaining)
		{
			RechargeRemaining -= Remaining;
			break;
		}

		// One charge back; the leftover time goes to the next missing charge.
		Remaining -= RechargeRemaining;
		const float Ammo = AbilitySystemComponent->GetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute());
		AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(), Ammo + 1.f);
		bRecharging = false;
		RechargeRemaining = 0.f;
		RefreshRecharge();
	}

	BroadcastChargesIfChanged();
}

void ARRTurretLauncher::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AdvanceRecharge(DeltaSeconds);
}

void ARRTurretLauncher::BroadcastChargesIfChanged(bool bForce)
{
	const int32 Charges = GetCharges();
	const int32 MaxCharges = GetMaxCharges();
	if (!bForce && Charges == LastBroadcastCharges && MaxCharges == LastBroadcastMaxCharges && bRecharging == bLastBroadcastRecharging)
	{
		return;
	}
	LastBroadcastCharges = Charges;
	LastBroadcastMaxCharges = MaxCharges;
	bLastBroadcastRecharging = bRecharging;
	OnTurretChargesChanged.Broadcast(Charges, MaxCharges, bRecharging ? GetRechargeDuration() : 0.f);
}
