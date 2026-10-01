// Rogue Rigs - base weapon actor: own ASC and weapon attributes, CSV stats, auto-fire toward the owning miner's aim.

#include "Weapons/RRWeapon.h"

#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "Player/RRMiner.h"
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

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// The attribute set, a default subobject of the weapon, is registered by the ASC's InitializeComponent.
	AbilitySystemComponent = CreateDefaultSubobject<URRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	WeaponSet = CreateDefaultSubobject<URRWeaponAttributeSet>(TEXT("WeaponSet"));

	AutoFire = CreateDefaultSubobject<URRAutoFireComponent>(TEXT("AutoFire"));
}

UAbilitySystemComponent* ARRWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ARRWeapon::Acquire(ARRMiner* InOwnerMiner)
{
	OwnerMiner = InOwnerMiner;

	// Acquisition order (sys-weapons): actor info, base values from CSV; upgrade replay and registry come later.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	FRRWeaponStatsRow Row;
	if (!LoadStatsRow(Row))
	{
		return false;
	}
	ApplyStatsRow(Row);

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
	}

	if (bStartFiringOnAcquire)
	{
		AutoFire->StartFiring();
	}
	return true;
}

bool ARRWeapon::LoadStatsRow(FRRWeaponStatsRow& OutRow) const
{
	const UDataTable* Table = FRRBaseStatsLoader::LoadTableFromCSV(WeaponStatsPath, FRRWeaponStatsRow::StaticStruct());
	if (Table == nullptr)
	{
		return false;
	}

	const FRRWeaponStatsRow* Row = Table->FindRow<FRRWeaponStatsRow>(StatsRowName, TEXT("ARRWeapon::LoadStatsRow"), false);
	if (Row == nullptr)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: row '%s' not found in %s; the weapon will not fire."), *GetName(), *StatsRowName.ToString(), WeaponStatsPath);
		return false;
	}

	OutRow = *Row;
	return true;
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
}

FVector ARRWeapon::GetAimDirection() const
{
	if (OwnerMiner)
	{
		return OwnerMiner->GetAimDirection();
	}
	return GetActorForwardVector().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
}

void ARRWeapon::FireOnce()
{
}

void ARRWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AutoFire)
	{
		AutoFire->StopFiring();
	}
	Super::EndPlay(EndPlayReason);
}
