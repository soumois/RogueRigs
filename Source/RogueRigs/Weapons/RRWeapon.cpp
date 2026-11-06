// Rogue Rigs - base weapon actor: own ASC and weapon attributes, CSV stats, auto-fire toward the owning miner's aim.

#include "Weapons/RRWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Damage/RRGE_Damage.h"
#include "Debug/RRShotFirstFrameDebug.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Player/RRMiner.h"
#include "Projectiles/RRProjectile.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAimAssist.h"
#include "RogueRigs.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "Weapons/RRWeaponStatsRow.h"
#include "World/RRShadows.h"
#include "UObject/ConstructorHelpers.h"

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

	// task-414 Q5: constructor reference (RRDrone pattern), so BP_BurstRifle needs no edit; a Blueprint may override or clear it.
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MuzzleFlashFinder(TEXT("/Game/RogueRigs/FX/NS_MuzzleFlash.NS_MuzzleFlash"));
	if (MuzzleFlashFinder.Succeeded())
	{
		MuzzleFlashSystem = MuzzleFlashFinder.Object;
	}
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

	// HUD ammo event (sys-hud): bound before the CSV values so their first application is sent.
	if (!AmmoChangedHandle.IsValid())
	{
		AmmoChangedHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRWeaponAttributeSet::GetAmmoAttribute())
			.AddUObject(this, &ARRWeapon::HandleAmmoAttributeChanged);
		MagazineSizeChangedHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRWeaponAttributeSet::GetMagazineSizeAttribute())
			.AddUObject(this, &ARRWeapon::HandleAmmoAttributeChanged);
	}

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

	// Impact spark components are created here (once per pool, up to its cap), never at a hit (task-432).
	ARRProjectile::PrewarmImpactSparks(GetWorld(), ProjectileClass);

	// Read the aim after the miner updated it this frame.
	if (OwnerMiner)
	{
		AutoFire->AddTickPrerequisiteActor(OwnerMiner);

		const ARRArenaGrid* Grid = OwnerMiner->GetArenaGrid();
		if (Grid && ShotHeight >= Grid->GetBlockHeight())
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: ShotHeight %.1f is not below the grid BlockHeight %.1f; shots will fly over blocks."), *GetName(), ShotHeight, Grid->GetBlockHeight());
		}
		if (Grid && ShotFloorClearance >= Grid->GetBlockHeight())
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: ShotFloorClearance %.1f is not below the grid BlockHeight %.1f; Muzzle shots will fly over blocks."), *GetName(), ShotFloorClearance, Grid->GetBlockHeight());
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
	AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), Row.Pierce);

	BurstCount = FMath::Max(Row.BurstCount, 1);
	BurstDuty = Row.BurstDuty;
	ReloadTime = Row.ReloadTime;
	ProjectileSpeed = Row.ProjectileSpeed;
	Range = Row.Range;
	SpreadDeg = Row.SpreadDeg;
	AimAssistDeg = Row.AimAssistDeg;
	DisplayName = Row.DisplayName;
}

FText ARRWeapon::GetDisplayName() const
{
	return FText::FromString(DisplayName.IsEmpty() ? StatsRowName.ToString() : DisplayName);
}

FText ARRWeapon::GetClassDisplayName(TSubclassOf<ARRWeapon> WeaponClass)
{
	if (!WeaponClass)
	{
		return FText::GetEmpty();
	}
	const ARRWeapon* Default = WeaponClass->GetDefaultObject<ARRWeapon>();
	FRRWeaponStatsRow Row;
	if (Default->LoadStatsRow(Row) && !Row.DisplayName.IsEmpty())
	{
		return FText::FromString(Row.DisplayName);
	}
	return FText::FromString(Default->StatsRowName.ToString());
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
	if (OwnerMiner == nullptr || OwnerMiner->GetCapsuleComponent() == nullptr)
	{
		return Muzzle->GetComponentLocation();
	}
	return GetShotPlacement().Origin;
}

bool ARRWeapon::GetFloorZUnderMiner(double& OutFloorZ) const
{
	const UCapsuleComponent* Capsule = OwnerMiner ? OwnerMiner->GetCapsuleComponent() : nullptr;
	if (Capsule == nullptr)
	{
		return false;
	}
	// Floor under the miner: capsule bottom, lowered by the gap CharacterMovement keeps above a walkable floor.
	OutFloorZ = Capsule->GetComponentLocation().Z - Capsule->GetScaledCapsuleHalfHeight();
	const UCharacterMovementComponent* Movement = OwnerMiner->GetCharacterMovement();
	if (Movement && Movement->IsMovingOnGround() && Movement->CurrentFloor.IsWalkableFloor())
	{
		OutFloorZ -= Movement->CurrentFloor.FloorDist;
	}
	return true;
}

bool ARRWeapon::IsHeldOnHandSocket() const
{
	return OwnerMiner && !HandSocketName.IsNone() && GetAttachParentActor() == OwnerMiner && GetAttachParentSocketName() == HandSocketName;
}

ARRWeapon* ARRWeapon::FindHandWeapon() const
{
	if (OwnerMiner == nullptr)
	{
		return nullptr;
	}
	for (ARRWeapon* Weapon : OwnerMiner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->IsHeldOnHandSocket())
		{
			return Weapon;
		}
	}
	return nullptr;
}

FRRShotPlacement ARRWeapon::GetShotPlacement() const
{
	FRRShotPlacement Placement;
	double FloorZ = 0.0;
	if (!GetFloorZUnderMiner(FloorZ))
	{
		// No miner: the own Muzzle, or the override of a weapon not held by a miner (turret).
		Placement.Origin = GetShotOrigin();
		return Placement;
	}
	const UCapsuleComponent* Capsule = OwnerMiner->GetCapsuleComponent();
	const FVector MinerAxis = Capsule->GetComponentLocation();

	const ARRWeapon* HandWeapon = FindHandWeapon();
	const USceneComponent* HandMuzzle = HandWeapon ? HandWeapon->GetMuzzle() : nullptr;
	if (HandMuzzle == nullptr)
	{
		// No shown weapon (fallback, not polished: the demo always starts with the burst rifle): pre-task-412 point and sphere.
		const FVector Forward = GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
		const FVector Base(MinerAxis.X, MinerAxis.Y, FloorZ + ShotHeight);
		Placement.Origin = Base + Forward * (Capsule->GetScaledCapsuleRadius() + ShotForwardOffset);
		return Placement;
	}

	// task-412: every weapon of the miner shoots from the shown weapon's Muzzle, read now (it follows the hand animation).
	const FVector MuzzleLocation = HandMuzzle->GetComponentLocation();
	const float Radius = ARRProjectile::GetDefaultCollisionRadius(ProjectileClass);
	const double CapsuleBottom = FloorZ + ShotFloorClearance;
	// Capsule centred on the Muzzle (simplest: no offset between the shot mesh and its collision): bottom at floor + clearance,
	// top as far above the Muzzle. It needs a half height of at least the radius, i.e. a Muzzle at least clearance + radius above the floor.
	const double HalfHeight = MuzzleLocation.Z - CapsuleBottom;
	Placement.bSweepFromMiner = true;
	if (HalfHeight >= Radius)
	{
		Placement.Origin = MuzzleLocation;
		Placement.CapsuleHalfHeight = static_cast<float>(HalfHeight);
	}
	else
	{
		if (!bLowMuzzleWarned)
		{
			bLowMuzzleWarned = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: Muzzle of %s is %.1f uu above the floor, below ShotFloorClearance %.1f + projectile radius %.1f; shots fall back to a sphere %.1f uu above the floor under the Muzzle."),
				*GetName(), *HandWeapon->GetName(), MuzzleLocation.Z - FloorZ, ShotFloorClearance, Radius, ShotHeight);
		}
		Placement.Origin = FVector(MuzzleLocation.X, MuzzleLocation.Y, FloorZ + ShotHeight);
	}
	Placement.SweepStart = FVector(MinerAxis.X, MinerAxis.Y, Placement.Origin.Z);
	return Placement;
}

ARRProjectile* ARRWeapon::SpawnShot(const FRRShotPlacement& Placement, const FRotator& ShotRotation, const FGameplayEffectSpecHandle& Spec, int32 MaxHits, float KnockbackSpeed)
{
	APawn* InstigatorPawn = OwnerMiner ? static_cast<APawn*>(OwnerMiner) : GetInstigator();
	const float Lifetime = ProjectileSpeed > 0.f ? Range / ProjectileSpeed : 0.f;
	const FVector SpawnLocation = Placement.bSweepFromMiner ? Placement.SweepStart : Placement.Origin;
	ARRProjectile* Projectile = ARRProjectile::SpawnProjectile(GetWorld(), ProjectileClass, FTransform(ShotRotation, SpawnLocation), this, InstigatorPawn,
		Spec, ProjectileSpeed, Lifetime, MaxHits, KnockbackSpeed, Placement.CapsuleHalfHeight);
	// task-412 Q3: the capsule is swept from the miner axis to the Muzzle (one sweep): a rock on the way stops the shot there,
	// robots on the way are hit as in flight; this also keeps a shot from being born inside a rock the miner hugs.
	if (Projectile && Placement.bSweepFromMiner)
	{
		const bool bFlying = Projectile->SweepTo(Placement.Origin);
		// task-413: spawned during the tick, the shot would fly one frame before its first draw (27 uu at 110 fps, 50 at 60 fps,
		// measured in -game); held at the Muzzle for that frame, it is first drawn at the barrel tip. Range kept (life span extended).
		if (bFlying)
		{
			Projectile->HoldFirstMove();
		}
		if (RRShotFirstFrameDebug::IsEnabled() && bFlying)
		{
			// task-413 measure (rr.Debug.ShotFirstFrame): the hand weapon Muzzle, read again on each logged frame.
			const ARRWeapon* HandWeapon = FindHandWeapon();
			const USceneComponent* HandMuzzle = HandWeapon ? HandWeapon->GetMuzzle() : nullptr;
			RRShotFirstFrameDebug::TrackShot(Projectile, HandMuzzle, HandMuzzle ? HandMuzzle->GetComponentLocation() : Placement.Origin, SpawnLocation, Projectile->GetActorLocation());
		}
	}
	return Projectile;
}

FRotator ARRWeapon::GetAimAssistedRotation(const FRRShotPlacement& Placement, const FRotator& ShotRotation, const ARREnemy** OutTarget) const
{
	if (OutTarget)
	{
		*OutTarget = nullptr;
	}
	const UWorld* World = GetWorld();
	const ARRProjectile* ProjectileDefault = ProjectileClass ? ProjectileClass->GetDefaultObject<ARRProjectile>() : nullptr;
	if (AimAssistDeg <= 0.f || World == nullptr || ProjectileDefault == nullptr || ProjectileDefault->GetCollisionCapsule() == nullptr)
	{
		return ShotRotation;
	}

	RRAimAssist::FShotQuery Query;
	Query.SweepStart = Placement.SweepStart;
	Query.bSweepFromMiner = Placement.bSweepFromMiner;
	Query.Origin = Placement.Origin;
	Query.Radius = ProjectileDefault->GetCollisionCapsule()->GetUnscaledCapsuleRadius();
	Query.HalfHeight = FMath::Max(Placement.CapsuleHalfHeight, Query.Radius);
	Query.Range = Range;
	Query.Responses = FCollisionResponseParams(ProjectileDefault->GetCollisionCapsule()->GetCollisionResponseToChannels());
	// What the projectile ignores (ARRProjectile::IsIgnoredActor): this weapon (its owner), the weapon owner and the instigator.
	Query.IgnoredActors.Add(this);
	if (GetOwner())
	{
		Query.IgnoredActors.Add(GetOwner());
	}
	Query.IgnoredActors.Add(OwnerMiner ? static_cast<const AActor*>(OwnerMiner) : GetInstigator());

	FVector Direction;
	if (!RRAimAssist::FindAssistedDirection(*World, Query, ShotRotation.Vector(), AimAssistDeg, Direction, OutTarget))
	{
		return ShotRotation;
	}
	return FRotator(0.f, Direction.Rotation().Yaw, 0.f);
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
	const FRRShotPlacement Placement = GetShotPlacement();
	// task-463: light aim assist on the shot as fired (spread included), each bullet decided on its own.
	const FRotator ShotRotation = GetAimAssistedRotation(Placement, FRotator(0.f, GetAimDirection().Rotation().Yaw + SpreadYaw, 0.f));
	if (SpawnShot(Placement, ShotRotation, Spec, GetPierceCount(), 0.f))
	{
		// task-414 Q3: one flash per bullet (3 per rifle burst); none for a turret (no hand weapon).
		SpawnMuzzleFlash(ShotRotation);
	}
}

UNiagaraComponent* ARRWeapon::SpawnMuzzleFlash(const FRotator& ShotRotation) const
{
	// Niagara never runs without rendering (-nullrhi, commandlets): no component there.
	if (MuzzleFlashSystem == nullptr || !FApp::CanEverRender())
	{
		return nullptr;
	}
	const ARRWeapon* HandWeapon = FindHandWeapon();
	USceneComponent* HandMuzzle = HandWeapon ? HandWeapon->GetMuzzle() : nullptr;
	if (HandMuzzle == nullptr)
	{
		return nullptr;
	}
	// Attached so it follows the hand; placed at the Muzzle in world space with +X along the shot (the system is local space).
	return UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashSystem, HandMuzzle, NAME_None, HandMuzzle->GetComponentLocation(),
		ShotRotation, EAttachLocation::KeepWorldPosition, /*bAutoDestroy*/ true, /*bAutoActivate*/ true, ENCPoolMethod::None);
}

int32 ARRWeapon::GetPierceCount() const
{
	const float Pierce = AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetPierceAttribute()) : 0.f;
	// Small tolerance so 3.9999 from stacked modifiers still counts as 4.
	return FMath::Max(FMath::FloorToInt32(Pierce + KINDA_SMALL_NUMBER), 1);
}

void ARRWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// task-263-01: only the grid rocks cast shadows (from the player light); no mesh of this actor or its Blueprint does.
	RRShadows::DisableCastShadows(this);
}

void ARRWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AutoFire)
	{
		AutoFire->StopFiring();
	}
	if (AbilitySystemComponent && AmmoChangedHandle.IsValid())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRWeaponAttributeSet::GetAmmoAttribute()).Remove(AmmoChangedHandle);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRWeaponAttributeSet::GetMagazineSizeAttribute()).Remove(MagazineSizeChangedHandle);
	}
	AmmoChangedHandle.Reset();
	MagazineSizeChangedHandle.Reset();
	Super::EndPlay(EndPlayReason);
}

int32 ARRWeapon::GetRounds() const
{
	const float Ammo = AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()) : 0.f;
	return FMath::Max(FMath::FloorToInt32(Ammo + KINDA_SMALL_NUMBER), 0);
}

int32 ARRWeapon::GetMagazineRounds() const
{
	const float Magazine = AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRWeaponAttributeSet::GetMagazineSizeAttribute()) : 0.f;
	return FMath::Max(FMath::FloorToInt32(Magazine + KINDA_SMALL_NUMBER), 0);
}

void ARRWeapon::HandleAmmoAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	const int32 Rounds = GetRounds();
	const int32 Magazine = GetMagazineRounds();
	if (Rounds == LastBroadcastRounds && Magazine == LastBroadcastMagazine)
	{
		return;
	}
	LastBroadcastRounds = Rounds;
	LastBroadcastMagazine = Magazine;
	OnAmmoChanged.Broadcast(this, Rounds, Magazine);
}
