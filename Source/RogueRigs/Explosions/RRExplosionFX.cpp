// Rogue Rigs - grey-box explosion visual: a mesh growing from 0 to the explosion radius while fading, then destroyed.

#include "Explosions/RRExplosionFX.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRExplosionFX)

ARRExplosionFX::ARRExplosionFX()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCastShadow(false);
	Mesh->SetCanEverAffectNavigation(false);
	RootComponent = Mesh;
}

void ARRExplosionFX::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh->GetNumMaterials() > 0 && Mesh->GetMaterial(0))
	{
		FadeMaterial = Mesh->CreateDynamicMaterialInstance(0);
	}
	Age = 0.f;
	ApplyProgress(0.f);
}

void ARRExplosionFX::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Age += DeltaSeconds;
	const float Alpha = FMath::Clamp(Age / FMath::Max(Duration, KINDA_SMALL_NUMBER), 0.f, 1.f);
	ApplyProgress(Alpha);
	if (Alpha >= 1.f)
	{
		Destroy();
	}
}

void ARRExplosionFX::ApplyProgress(float Alpha)
{
	const float Scale = Alpha * Radius / FMath::Max(MeshRadius, KINDA_SMALL_NUMBER);
	Mesh->SetRelativeScale3D(FVector(Scale));
	if (FadeMaterial)
	{
		FadeMaterial->SetScalarParameterValue(FadeParameterName, 1.f - Alpha);
	}
}
