// Rogue Rigs - base projectile: vertical capsule collision, flies straight, carries a damage spec, hurts up to MaxHits distinct hittable actors with an ASC (pierce), stops on rock.

#include "Projectiles/RRProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/RREnemy.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Grid/RRArenaGrid.h"
#include "NiagaraSystem.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "World/RRShadows.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRProjectile)

ARRProjectile::ARRProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// The movement component moves the projectile; the actor tick only runs once, at the end of the frame HoldFirstMove holds
	// (task-413): TG_PostUpdateWork comes after the movement tick of that frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	// task-412: a capsule (was the sphere CollisionSphere); half height = radius makes it the same sphere until a shot sets a taller one.
	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	// Miner shots (rifle, shotgun pellet, turret bullet): 14 uu (owner 2026-10-06 fourth test; was 12, before that 10); the mesh is not scaled. Robot shots keep 10 uu (ARREnemyProjectile).
	CollisionCapsule->InitCapsuleSize(14.f, 14.f);
	CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionCapsule->SetCollisionObjectType(RRCollisionChannels::Projectile);
	CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionCapsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionCapsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionCapsule->SetGenerateOverlapEvents(true);
	CollisionCapsule->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionCapsule);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionCapsule;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bForceSubStepping = false;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->Velocity = FVector::ForwardVector;

	// task-432: constructor reference (RRDrone pattern), so the shot Blueprints need no edit; ARREnemyProjectile clears it.
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ImpactSparkFinder(TEXT("/Game/RogueRigs/FX/NS_ShotImpactSpark.NS_ShotImpactSpark"));
	if (ImpactSparkFinder.Succeeded())
	{
		ImpactSparkSystem = ImpactSparkFinder.Object;
	}
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
		CollisionCapsule->IgnoreActorWhenMoving(OwnerActor, true);
		if (AActor* OwnerOwner = OwnerActor->GetOwner())
		{
			CollisionCapsule->IgnoreActorWhenMoving(OwnerOwner, true);
		}
	}
	if (AActor* InstigatorActor = GetInstigator())
	{
		CollisionCapsule->IgnoreActorWhenMoving(InstigatorActor, true);
	}

	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &ARRProjectile::OnCollisionBeginOverlap);
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ARRProjectile::OnProjectileStop);
}

void ARRProjectile::DisableVisualCollision()
{
	// The capsule is the only collision of a projectile: the Blueprint shot mesh is visual (it pushed robots and stopped bullets when it blocked, task-180-01).
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive != CollisionCapsule)
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
	AActor* InOwner, APawn* InInstigator, const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime, int32 MaxHits, float KnockbackSpeed,
	float CapsuleHalfHeight)
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
	if (CapsuleHalfHeight > 0.f)
	{
		Projectile->SetCapsuleHalfHeight(CapsuleHalfHeight);
	}
	Projectile->FinishSpawning(SpawnTransform);
	return Projectile;
}

void ARRProjectile::SetCapsuleHalfHeight(float HalfHeight)
{
	const float Radius = CollisionCapsule->GetUnscaledCapsuleRadius();
	// No overlap update here: before FinishSpawning the actor is not placed yet; the spawn and the first move update the overlaps.
	CollisionCapsule->SetCapsuleHalfHeight(HalfHeight > 0.f ? FMath::Max(HalfHeight, Radius) : Radius, /*bUpdateOverlaps*/ false);
}

float ARRProjectile::GetDefaultCollisionRadius(TSubclassOf<ARRProjectile> Class)
{
	const ARRProjectile* Default = Class ? Class->GetDefaultObject<ARRProjectile>() : nullptr;
	return Default && Default->CollisionCapsule ? Default->CollisionCapsule->GetUnscaledCapsuleRadius() : 0.f;
}

void ARRProjectile::PrewarmImpactSparks(UWorld* World, TSubclassOf<ARRProjectile> Class)
{
	const ARRProjectile* Default = Class ? Class->GetDefaultObject<ARRProjectile>() : nullptr;
	URRNiagaraPoolSubsystem* FXPool = World ? World->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	if (Default && Default->ImpactSparkSystem && FXPool)
	{
		FXPool->Prewarm(Default->ImpactSparkSystem, Default->ImpactSparkPoolSize, Default->ImpactSparkPoolSize);
	}
}

void ARRProjectile::PlayImpactSpark(const FHitResult* Impact)
{
	URRNiagaraPoolSubsystem* FXPool = ImpactSparkSystem && GetWorld() ? GetWorld()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	if (FXPool == nullptr)
	{
		return;
	}
	// The shot's own height, where the player sees the shot: a tall Muzzle capsule (task-412) may touch far below the shot mesh.
	FVector Location = GetActorLocation();
	if (Impact && !Impact->bStartPenetrating)
	{
		Location.X = Impact->ImpactPoint.X;
		Location.Y = Impact->ImpactPoint.Y;
	}
	FXPool->Spawn(ImpactSparkSystem, Location, FRotator::ZeroRotator, ImpactSparkPoolSize);
}

bool ARRProjectile::SweepTo(const FVector& TargetLocation)
{
	if (bSpent || !IsValid(this))
	{
		return false;
	}
	// Swept move of the root capsule: pawns overlapped on the way fire OnCollisionBeginOverlap (the normal hit path), overlaps beyond
	// a blocking hit are dropped by the move, and the owner, its miner and the instigator are ignored (IgnoreActorWhenMoving).
	FHitResult Hit;
	SetActorLocation(TargetLocation, /*bSweep*/ true, &Hit, ETeleportType::None);
	if (bSpent || !IsValid(this) || IsActorBeingDestroyed())
	{
		return false;
	}
	if (Hit.bBlockingHit)
	{
		OnProjectileStop(Hit);
		return false;
	}
	return true;
}

void ARRProjectile::HoldFirstMove()
{
	UWorld* World = GetWorld();
	// Outside the world tick (tests firing between ticks) the movement first runs on the next tick: nothing to hold.
	if (World == nullptr || !World->bInTick || bSpent || IsActorBeingDestroyed() || IsHoldingFirstMove() || !ProjectileMovement->bSimulationEnabled)
	{
		return;
	}
	// Spawned during the tick, the movement tick is queued as newly spawned and runs this same frame: one full frame of flight
	// before the first draw. With the simulation off that tick does not move the projectile. Tick (TG_PostUpdateWork, after the
	// movement) turns it back on at the end of this frame, so the flight starts with the next frame's step.
	ProjectileMovement->bSimulationEnabled = false;
	HeldFrameTime = World->GetTimeSeconds();
	// The held frame is flight time lost: extend the life span by it so the range flown stays Range.
	const float LifeSpanLeft = GetLifeSpan();
	if (LifeSpanLeft > 0.f)
	{
		SetLifeSpan(LifeSpanLeft + World->GetDeltaSeconds());
	}
	SetActorTickEnabled(true);
}

void ARRProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!IsHoldingFirstMove())
	{
		SetActorTickEnabled(false);
		return;
	}
	// End of the held frame (this tick runs in TG_PostUpdateWork, after this frame's movement tick): the next movement tick flies.
	HeldFrameTime = -1.0;
	ProjectileMovement->bSimulationEnabled = true;
	SetActorTickEnabled(false);
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

void ARRProjectile::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		PlayImpactSpark(bFromSweep ? &SweepResult : nullptr);
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
				PlayImpactSpark(&ImpactResult);
				Grid->ApplyShotDamage(Cell, GetCarriedDamage());
			}
		}
	}
	bSpent = true;
	Destroy();
}
