// Rogue Rigs - grey-box explosion visual: a mesh growing from 0 to its radius while fading (or growing, holding, then fading), then destroyed.

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
	ApplyAge();
}

float ARRExplosionFX::GetLifetime() const
{
	const float GrowSeconds = FMath::Max(Duration, KINDA_SMALL_NUMBER);
	return HoldSeconds > 0.f ? GrowSeconds + HoldSeconds + FMath::Max(FadeSeconds, KINDA_SMALL_NUMBER) : GrowSeconds;
}

void ARRExplosionFX::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Age += DeltaSeconds;
	ApplyAge();
	if (Age >= GetLifetime())
	{
		Destroy();
	}
}

void ARRExplosionFX::ApplyAge()
{
	const float GrowSeconds = FMath::Max(Duration, KINDA_SMALL_NUMBER);
	const float GrowAlpha = FMath::Clamp(Age / GrowSeconds, 0.f, 1.f);
	if (HoldSeconds <= 0.f)
	{
		// Legacy: grow and fade together.
		ApplyState(GrowAlpha, 1.f - GrowAlpha);
		return;
	}
	// Grow at full opacity, hold the final state, then fade at full size.
	const float FadeAge = Age - GrowSeconds - HoldSeconds;
	const float FadeAlpha = FMath::Clamp(FadeAge / FMath::Max(FadeSeconds, KINDA_SMALL_NUMBER), 0.f, 1.f);
	ApplyState(GrowAlpha, 1.f - FadeAlpha);
}

void ARRExplosionFX::ApplyState(float GrowAlpha, float Opacity)
{
	const float Scale = GrowAlpha * Radius / FMath::Max(MeshRadius, KINDA_SMALL_NUMBER);
	Mesh->SetRelativeScale3D(FVector(Scale));
	if (FadeMaterial)
	{
		FadeMaterial->SetScalarParameterValue(FadeParameterName, Opacity);
	}
}
