// Rogue Rigs - base projectile: flies straight, carries a damage spec, hurts the first actor with an ASC, stops on rock.

#include "Projectiles/RRProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectiles/RRCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRProjectile)

ARRProjectile::ARRProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(10.f);
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

void ARRProjectile::InitProjectile(const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime)
{
	DamageSpec = InDamageSpec;

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

void ARRProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasHit || !IsValid(this) || IsIgnoredActor(OtherActor))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (TargetASC == nullptr)
	{
		return;
	}

	bHasHit = true;
	if (const FGameplayEffectSpec* Spec = DamageSpec.Data.Get())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
	Destroy();
}

void ARRProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	Destroy();
}
