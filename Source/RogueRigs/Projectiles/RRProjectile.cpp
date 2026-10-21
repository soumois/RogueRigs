// Rogue Rigs - base projectile: flies straight, carries a damage spec, hurts up to MaxHits distinct hittable actors with an ASC (pierce), stops on rock.

#include "Projectiles/RRProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "Enemies/RREnemy.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "World/RRShadows.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRProjectile)

ARRProjectile::ARRProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	// Miner shots (rifle, shotgun pellet, turret bullet): 14 uu (owner 2026-10-06 fourth test; was 12, before that 10); the mesh is not scaled. Robot shots keep 10 uu (ARREnemyProjectile).
	CollisionSphere->InitSphereRadius(14.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(RRCollisionChannels::Projectile);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionSphere);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bForceSubStepping = false;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->Velocity = FVector::ForwardVector;
}

void ARRProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	DisableVisualCollision();
	// task-263-01: only the grid rocks cast shadows (from the player light); no projectile mesh does.
	RRShadows::DisableCastShadows(this);

	// The miner's capsule and mesh, and the weapon's mesh, never stop or receive this projectile.
	if (AActor* OwnerActor = GetOwner())
	{
		CollisionSphere->IgnoreActorWhenMoving(OwnerActor, true);
		if (AActor* OwnerOwner = OwnerActor->GetOwner())
		{
			CollisionSphere->IgnoreActorWhenMoving(OwnerOwner, true);
		}
	}
	if (AActor* InstigatorActor = GetInstigator())
	{
		CollisionSphere->IgnoreActorWhenMoving(InstigatorActor, true);
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARRProjectile::OnSphereBeginOverlap);
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ARRProjectile::OnProjectileStop);
}

void ARRProjectile::DisableVisualCollision()
{
	// The sphere is the only collision of a projectile: the Blueprint shot mesh is visual (it pushed robots and stopped bullets when it blocked, task-180-01).
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive != CollisionSphere)
		{
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Primitive->SetGenerateOverlapEvents(false);
		}
	}
}

void ARRProjectile::InitProjectile(const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime, int32 InMaxHits, float InKnockbackSpeed)
{
	DamageSpec = InDamageSpec;
	MaxHits = FMath::Max(InMaxHits, 1);
	KnockbackSpeed = FMath::Max(InKnockbackSpeed, 0.f);

	const float ClampedSpeed = FMath::Max(Speed, 0.f);
	ProjectileMovement->InitialSpeed = ClampedSpeed;
	ProjectileMovement->MaxSpeed = ClampedSpeed;

	const float ClampedLifetime = FMath::Max(Lifetime, 0.f);
	if (HasActorBegunPlay())
	{
		ProjectileMovement->Velocity = GetActorForwardVector() * ClampedSpeed;
		SetLifeSpan(ClampedLifetime);
	}
	else
	{
		// AActor::BeginPlay applies InitialLifeSpan; the movement component turns InitialSpeed into Velocity at initialization.
		InitialLifeSpan = ClampedLifetime;
	}
}

ARRProjectile* ARRProjectile::SpawnProjectile(UWorld* World, TSubclassOf<ARRProjectile> Class, const FTransform& SpawnTransform,
	AActor* InOwner, APawn* InInstigator, const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime, int32 MaxHits, float KnockbackSpeed)
{
	if (!World || !Class)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRProjectile::SpawnProjectile: no world or no class (owner %s)."), *GetNameSafe(InOwner));
		return nullptr;
	}
	ARRProjectile* Projectile = World->SpawnActorDeferred<ARRProjectile>(Class, SpawnTransform, InOwner, InInstigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Projectile == nullptr)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: projectile of class %s could not be spawned."), *GetNameSafe(InOwner), *Class->GetName());
		return nullptr;
	}
	Projectile->InitProjectile(InDamageSpec, Speed, Lifetime, MaxHits, KnockbackSpeed);
	Projectile->FinishSpawning(SpawnTransform);
	return Projectile;
}

bool ARRProjectile::CanHit(const AActor* OtherActor) const
{
	return OtherActor && (!HitActorClass || OtherActor->IsA(HitActorClass));
}

bool ARRProjectile::IsIgnoredActor(const AActor* OtherActor) const
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return true;
	}
	const AActor* OwnerActor = GetOwner();
	if (OwnerActor && (OtherActor == OwnerActor || OtherActor == OwnerActor->GetOwner()))
	{
		return true;
	}
	return OtherActor == GetInstigator();
}

int32 ARRProjectile::GetHitLife(const AActor* OtherActor)
{
	const ARREnemy* Enemy = Cast<ARREnemy>(OtherActor);
	return Enemy ? Enemy->GetPoolLifeCount() : 0;
}

void ARRProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bSpent || !IsValid(this) || IsIgnoredActor(OtherActor) || !CanHit(OtherActor) || (HitActors.Contains(OtherActor) && HitActors[OtherActor] == GetHitLife(OtherActor)))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (TargetASC == nullptr)
	{
		return;
	}

	// Pierce: each distinct actor is hit once; the projectile flies on until MaxHits actors were hit.
	HitActors.Add(OtherActor, GetHitLife(OtherActor));
	++HitCount;
	if (HitCount >= MaxHits)
	{
		bSpent = true;
	}
	// Robots throw their death debris along the flight of the last shot (enemy-death); recorded before a possibly lethal hit.
	if (ARREnemy* HitRobot = Cast<ARREnemy>(OtherActor))
	{
		HitRobot->RecordHitDirection(ProjectileMovement->Velocity.IsNearlyZero() ? GetActorForwardVector() : ProjectileMovement->Velocity);
	}
	if (const FGameplayEffectSpec* Spec = DamageSpec.Data.Get())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
	// Knockback after the damage: ApplyKnockback ignores a robot this hit killed.
	if (KnockbackSpeed > 0.f)
	{
		if (ARREnemy* Enemy = Cast<ARREnemy>(OtherActor))
		{
			const FVector Flight = ProjectileMovement->Velocity.GetSafeNormal2D(UE_SMALL_NUMBER, GetActorForwardVector().GetSafeNormal2D());
			Enemy->ApplyKnockback(Flight * KnockbackSpeed);
		}
	}
	if (bSpent)
	{
		Destroy();
	}
}

float ARRProjectile::GetCarriedDamage() const
{
	const FGameplayEffectSpec* Spec = DamageSpec.Data.Get();
	if (Spec == nullptr)
	{
		return 0.f;
	}
	// Same capture definition as URRDamageExecution (source, snapshot): the weapon's Damage at fire time.
	const FGameplayEffectAttributeCaptureDefinition DamageDef(URRWeaponAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	const FGameplayEffectAttributeCaptureSpec* Capture = Spec->CapturedRelevantAttributes.FindCaptureSpecByDefinition(DamageDef, true);
	float Damage = 0.f;
	if (Capture == nullptr || !Capture->AttemptCalculateAttributeMagnitude(FAggregatorEvaluateParameters(), Damage))
	{
		return 0.f;
	}
	return FMath::Max(Damage, 0.f);
}

void ARRProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	// Barrels are grid cells without an ASC: the grid takes the carried damage as shot damage (other blocks ignore it).
	if (!bSpent && bDamagesTerrain)
	{
		if (ARRArenaGrid* Grid = Cast<ARRArenaGrid>(ImpactResult.GetActor()))
		{
			const FIntPoint Cell = Grid->GetCellFromHit(ImpactResult);
			if (Grid->GetCellType(Cell) == ERRCellType::Barrel)
			{
				Grid->ApplyShotDamage(Cell, GetCarriedDamage());
			}
		}
	}
	bSpent = true;
	Destroy();
}
