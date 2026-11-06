// Rogue Rigs - placed turret: copies its launcher's stats at placement, locks the nearest robot in range with grid line of sight, fires piercing bullets until its lifetime ends.

#include "Weapons/RRTurret.h"

#include "AttributeSet.h"
#include "Enemies/RREnemy.h"
#include "EngineUtils.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "RogueRigs.h"
#include "UObject/UnrealType.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "World/RRBlobShadowComponent.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTurret)

ARRTurret::ARRTurret(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bStartFiringOnAcquire = false;
	// task-414 Q3: turret bullets show no muzzle flash (it has no hand weapon anyway; cleared so the intent is explicit).
	MuzzleFlashSystem = nullptr;

	// Fixed base, yawing head (task-350-01): the Muzzle (created under Root by ARRWeapon) follows the head.
	HeadPivot = CreateDefaultSubobject<USceneComponent>(TEXT("HeadPivot"));
	HeadPivot->SetupAttachment(Root);
	Muzzle->SetupAttachment(HeadPivot);

	// task-472-01: light-independent shadow, fitted to the Blueprint meshes at BeginPlay.
	BlobShadow = CreateDefaultSubobject<URRBlobShadowComponent>(TEXT("BlobShadow"));
	BlobShadow->SetupAttachment(Root);
}

void ARRTurret::BeginPlay()
{
	Super::BeginPlay();
	BlobShadow->FitToOwnerMeshes();
}

bool ARRTurret::InitFromLauncher(const ARRTurretLauncher& Launcher)
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// Every weapon attribute, current value of the launcher (upgrades included) as frozen base value (sys-weapons: attribute copy).
	const UAbilitySystemComponent* LauncherASC = Launcher.GetAbilitySystemComponent();
	if (LauncherASC)
	{
		for (TFieldIterator<FProperty> It(URRWeaponAttributeSet::StaticClass()); It; ++It)
		{
			if (FGameplayAttribute::IsGameplayAttributeDataProperty(*It))
			{
				const FGameplayAttribute Attribute(*It);
				AbilitySystemComponent->SetNumericAttributeBase(Attribute, LauncherASC->GetNumericAttribute(Attribute));
			}
		}

		// Behavior tags of the launcher (impact effects, projectile behaviors) travel with the stats.
		FGameplayTagContainer LauncherTags;
		LauncherASC->GetOwnedGameplayTags(LauncherTags);
		if (!LauncherTags.IsEmpty())
		{
			AbilitySystemComponent->AddLooseGameplayTags(LauncherTags);
		}
	}

	ProjectileClass = Launcher.GetProjectileClass();
	DamageEffectClass = Launcher.GetDamageEffectClass();
	BurstCount = Launcher.GetBurstCount();
	BurstDuty = Launcher.GetBurstDuty();
	ReloadTime = Launcher.GetReloadTime();
	ProjectileSpeed = Launcher.GetProjectileSpeed();
	Range = Launcher.GetRange();
	SpreadDeg = Launcher.GetSpreadDeg();
	Lifetime = Launcher.GetTurretLifetime();

	if (Lifetime > 0.f)
	{
		SetLifeSpan(Lifetime);
	}
	else
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: TurretLifetime %.2f is not positive; the turret never expires."), *GetName(), Lifetime);
	}

	ArenaGrid = RRWorld::FindArenaGrid(GetWorld());

	// MagazineSize is the launcher's charge count: a turret fires without magazine for its whole life.
	AutoFire->SetUseMagazine(false);
	if (!AutoFire->Configure(AbilitySystemComponent, BurstCount, BurstDuty, ReloadTime))
	{
		return false;
	}
	AutoFire->SetShotHandler(FSimpleDelegate::CreateUObject(this, &ARRTurret::FireOnce));
	// Paused until a target is locked (Tick).
	AutoFire->StopFiring();
	bArmed = true;
	TimeToNextSearch = 0.f;
	return true;
}

FVector ARRTurret::GetAimDirection() const
{
	if (const ARREnemy* Locked = Target.Get())
	{
		const FVector TargetLocation = Locked->GetActorLocation();
		const FVector FromCentre = (TargetLocation - GetTargetingOrigin()).GetSafeNormal2D();
		const FVector FromMuzzle = (TargetLocation - GetShotOrigin()).GetSafeNormal2D();
		// From the canon tip, unless the robot is level with or behind it (closer than the canon): then from the centre.
		if (!FromMuzzle.IsZero() && (FromCentre.IsZero() || FVector::DotProduct(FromMuzzle, FromCentre) > 0.f))
		{
			return FromMuzzle;
		}
		if (!FromCentre.IsZero())
		{
			return FromCentre;
		}
	}
	return HeadPivot->GetForwardVector().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
}

FVector ARRTurret::GetShotOrigin() const
{
	const FVector MuzzleLocation = Muzzle->GetComponentLocation();
	return FVector(MuzzleLocation.X, MuzzleLocation.Y, GetActorLocation().Z + ShotHeight);
}

FVector ARRTurret::GetTargetingOrigin() const
{
	return GetActorLocation() + FVector(0.f, 0.f, ShotHeight);
}

void ARRTurret::UpdateHeadYaw()
{
	const ARREnemy* Locked = Target.Get();
	if (!Locked)
	{
		return;
	}
	const FVector ToTarget = Locked->GetActorLocation() - HeadPivot->GetComponentLocation();
	if (ToTarget.SizeSquared2D() < UE_KINDA_SMALL_NUMBER)
	{
		return;
	}
	FRotator Relative = HeadPivot->GetRelativeRotation();
	Relative.Yaw = FRotator::NormalizeAxis(ToTarget.Rotation().Yaw - GetActorRotation().Yaw);
	HeadPivot->SetRelativeRotation(Relative);
}

bool ARRTurret::IsValidTarget(const ARREnemy* Enemy) const
{
	if (!IsValid(Enemy) || Enemy->IsActorBeingDestroyed() || Enemy->IsDead() || URRActorPoolSubsystem::IsPooledIdle(Enemy))
	{
		return false;
	}
	const FVector Origin = GetTargetingOrigin();
	if (FVector::DistSquared2D(Origin, Enemy->GetActorLocation()) > FMath::Square(Range))
	{
		return false;
	}
	const ARRArenaGrid* Grid = ArenaGrid.Get();
	return !Grid || Grid->HasLineOfSight(Origin, Enemy->GetActorLocation());
}

ARREnemy* ARRTurret::FindNearestTarget() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	const FVector Origin = GetTargetingOrigin();
	const double RangeSquared = FMath::Square(Range);
	ARREnemy* Nearest = nullptr;
	double NearestDistanceSquared = TNumericLimits<double>::Max();
	for (TActorIterator<ARREnemy> It(World); It; ++It)
	{
		ARREnemy* Enemy = *It;
		const double DistanceSquared = FVector::DistSquared2D(Origin, Enemy->GetActorLocation());
		// Cheap distance filter first; the sight check (grid walk) only for closer candidates.
		if (DistanceSquared > RangeSquared || DistanceSquared >= NearestDistanceSquared)
		{
			continue;
		}
		if (IsValidTarget(Enemy))
		{
			Nearest = Enemy;
			NearestDistanceSquared = DistanceSquared;
		}
	}
	return Nearest;
}

void ARRTurret::UpdateTarget(float DeltaSeconds)
{
	if (IsValidTarget(Target.Get()))
	{
		return;
	}

	// Target lost (or none): search now if it was just lost, else at most every TargetSearchInterval.
	const bool bJustLost = Target.IsValid() || Target.IsStale();
	Target.Reset();
	TimeToNextSearch -= DeltaSeconds;
	if (!bJustLost && TimeToNextSearch > 0.f)
	{
		return;
	}
	TimeToNextSearch = TargetSearchInterval;
	Target = FindNearestTarget();
}

void ARRTurret::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bArmed)
	{
		return;
	}

	UpdateTarget(DeltaSeconds);
	if (Target.IsValid())
	{
		// The base keeps its placement yaw; only the head turns (owner Q3, task-350).
		UpdateHeadYaw();
		AutoFire->StartFiring();
	}
	else
	{
		AutoFire->StopFiring();
	}
}

void ARRTurret::FireOnce()
{
	if (!IsValidTarget(Target.Get()))
	{
		return;
	}
	// The auto-fire may run before this tick's head turn: aim the canon first so the Muzzle is current.
	UpdateHeadYaw();
	Super::FireOnce();
	++ShotCount;
	OnTurretFired(GetShotOrigin(), GetAimDirection());
}
