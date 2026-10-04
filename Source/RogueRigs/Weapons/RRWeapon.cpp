// Rogue Rigs - base weapon actor: own ASC and weapon attributes, CSV stats, auto-fire toward the owning miner's aim.

#include "Weapons/RRWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Damage/RRGE_Damage.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "Projectiles/RRProjectile.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "RogueRigs.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "Weapons/RRWeaponStatsRow.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRWeapon)

const TCHAR* ARRWeapon::WeaponStatsPath = TEXT("RogueRigs/Data/Tables/WeaponStats.csv");

ARRWeapon::ARRWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	// A held weapon sits inside the miner capsule: any blocking collision would push the miner (depenetration) as it moves.
	SetActorEnableCollision(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// The attribute set, a default subobject of the weapon, is registered by the ASC's InitializeComponent.
	AbilitySystemComponent = CreateDefaultSubobject<URRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	WeaponSet = CreateDefaultSubobject<URRWeaponAttributeSet>(TEXT("WeaponSet"));

	AutoFire = CreateDefaultSubobject<URRAutoFireComponent>(TEXT("AutoFire"));

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(Root);

	DamageEffectClass = URRGE_Damage::StaticClass();
}

UAbilitySystemComponent* ARRWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ARRWeapon::Acquire(ARRMiner* InOwnerMiner)
{
	OwnerMiner = InOwnerMiner;

	// Acquisition order (sys-weapons): actor info, base values from CSV, replay of the global upgrades; registry comes later.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	FRRWeaponStatsRow Row;
	if (!LoadStatsRow(Row))
	{
		return false;
	}
	ApplyStatsRow(Row);

	if (OwnerMiner && OwnerMiner->GetUpgrades())
	{
		OwnerMiner->GetUpgrades()->ReplayGlobalUpgrades(*this);
	}

	if (!AutoFire->Configure(AbilitySystemComponent, BurstCount, BurstDuty, ReloadTime))
	{
		return false;
	}
	AutoFire->RefillAmmo();
	AutoFire->SetShotHandler(FSimpleDelegate::CreateUObject(this, &ARRWeapon::FireOnce));

	// Read the aim after the miner updated it this frame.
	if (OwnerMiner)
	{
		AutoFire->AddTickPrerequisiteActor(OwnerMiner);

		const ARRArenaGrid* Grid = OwnerMiner->GetArenaGrid();
		if (Grid && ShotHeight >= Grid->GetBlockHeight())
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: ShotHeight %.1f is not below the grid BlockHeight %.1f; shots will fly over blocks."), *GetName(), ShotHeight, Grid->GetBlockHeight());
		}
	}

	if (bStartFiringOnAcquire)
	{
		AutoFire->StartFiring();
	}
	return true;
}

bool ARRWeapon::LoadStatsRow(FRRWeaponStatsRow& OutRow) const
{
	const FString Context = FString::Printf(TEXT("%s; the weapon will not fire"), *GetName());
	return FRRBaseStatsLoader::FindRow(WeaponStatsPath, StatsRowName, OutRow, *Context);
}

void ARRWeapon::ApplyStatsRow(const FRRWeaponStatsRow& Row)
{
	// MagazineSize before Ammo: Ammo is clamped against it (Ammo is filled by the auto-fire's refill).
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetDamageAttribute(), Row.Damage);
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetFireRateAttribute(), Row.FireRate);
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetReloadSpeedAttribute(), Row.ReloadSpeed);
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), Row.CritChance);
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritDamageAttribute(), Row.CritDamage);
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetMagazineSizeAttribute(), Row.MagazineSize);

	BurstCount = FMath::Max(Row.BurstCount, 1);
	BurstDuty = Row.BurstDuty;
	ReloadTime = Row.ReloadTime;
	ProjectileSpeed = Row.ProjectileSpeed;
	Range = Row.Range;
	SpreadDeg = Row.SpreadDeg;
	DisplayName = Row.DisplayName;
}

FText ARRWeapon::GetDisplayName() const
{
	return FText::FromString(DisplayName.IsEmpty() ? StatsRowName.ToString() : DisplayName);
}

FVector ARRWeapon::GetAimDirection() const
{
	if (OwnerMiner)
	{
		return OwnerMiner->GetAimDirection();
	}
	return GetActorForwardVector().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
}

FVector ARRWeapon::GetShotOrigin() const
{
	const UCapsuleComponent* Capsule = OwnerMiner ? OwnerMiner->GetCapsuleComponent() : nullptr;
	if (Capsule == nullptr)
	{
		return Muzzle->GetComponentLocation();
	}

	// Floor under the miner: capsule bottom, lowered by the gap CharacterMovement keeps above a walkable floor.
	double FloorZ = Capsule->GetComponentLocation().Z - Capsule->GetScaledCapsuleHalfHeight();
	const UCharacterMovementComponent* Movement = OwnerMiner->GetCharacterMovement();
	if (Movement && Movement->IsMovingOnGround() && Movement->CurrentFloor.IsWalkableFloor())
	{
		FloorZ -= Movement->CurrentFloor.FloorDist;
	}

	const FVector Forward = GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const FVector Base(Capsule->GetComponentLocation().X, Capsule->GetComponentLocation().Y, FloorZ + ShotHeight);
	return Base + Forward * (Capsule->GetScaledCapsuleRadius() + ShotForwardOffset);
}

FGameplayEffectSpecHandle ARRWeapon::MakeDamageSpec() const
{
	if (!DamageEffectClass)
	{
		return FGameplayEffectSpecHandle();
	}
	// MakeEffectContext adds the ASC's owner and avatar (both this weapon) as instigator and causer (sys-damage).
	return AbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, 1.f, AbilitySystemComponent->MakeEffectContext());
}

void ARRWeapon::FireOnce()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (!ProjectileClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: ProjectileClass is not set; shot skipped."), *GetName());
		return;
	}
	if (ProjectileSpeed <= 0.f || Range <= 0.f)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: ProjectileSpeed %.1f or Range %.1f is not positive; shot skipped."), *GetName(), ProjectileSpeed, Range);
		return;
	}

	const FGameplayEffectSpecHandle Spec = MakeDamageSpec();
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no damage spec (DamageEffectClass unset); the projectile will deal no damage."), *GetName());
	}

	// Flat shot: aim yaw plus a uniform spread in [-SpreadDeg, SpreadDeg].
	const float SpreadYaw = SpreadDeg > 0.f ? FMath::FRandRange(-SpreadDeg, SpreadDeg) : 0.f;
	const FRotator ShotRotation(0.f, GetAimDirection().Rotation().Yaw + SpreadYaw, 0.f);
	const FTransform SpawnTransform(ShotRotation, GetShotOrigin());

	APawn* InstigatorPawn = OwnerMiner ? static_cast<APawn*>(OwnerMiner) : GetInstigator();
	ARRProjectile::SpawnProjectile(World, ProjectileClass, SpawnTransform, this, InstigatorPawn, Spec, ProjectileSpeed, Range / ProjectileSpeed);
}

void ARRWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AutoFire)
	{
		AutoFire->StopFiring();
	}
	Super::EndPlay(EndPlayReason);
}
