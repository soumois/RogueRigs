// Rogue Rigs - grey-box explosion visual: a mesh growing from 0 to its radius while fading (or growing, holding, then fading), with an optional Niagara burst and ground shockwave decal, then destroyed.

#include "Explosions/RRExplosionFX.h"

#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRExplosionFX)

const FName ARRExplosionFX::ShockwaveRadiusParameterName(TEXT("Radius"));
const FName ARRExplosionFX::ShockwaveProgressParameterName(TEXT("Progress"));
const FName ARRExplosionFX::ShockwaveOpacityParameterName(TEXT("Opacity"));

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
	CreateShockwave();
	StartBurst();
	Age = 0.f;
	bVisualHidden = false;
	ApplyAge();
}

void ARRExplosionFX::StartBurst()
{
	if (!BurstSystem)
	{
		return;
	}
	// Attached to the root for its location only: the root mesh scales with the growth, the burst must not.
	BurstComponent = NewObject<UNiagaraComponent>(this, TEXT("Burst"), RF_Transient);
	BurstComponent->SetAsset(BurstSystem);
	BurstComponent->bAutoActivate = false;
	BurstComponent->SetAutoDestroy(false);
	BurstComponent->SetVisibleInRayTracing(false);
	BurstComponent->SetCanEverAffectNavigation(false);
	BurstComponent->SetUsingAbsoluteRotation(true);
	BurstComponent->SetUsingAbsoluteScale(true);
	BurstComponent->SetupAttachment(Mesh);
	BurstComponent->RegisterComponent();
	BurstComponent->SetWorldRotation(FRotator::ZeroRotator);
	BurstComponent->SetWorldScale3D(FVector::OneVector);
	BurstComponent->SetVariableFloat(BurstRadiusParameterName, Radius);
	BurstComponent->Activate(/*bReset*/ true);
}

void ARRExplosionFX::CreateShockwave()
{
	if (!ShockwaveMaterial)
	{
		return;
	}
	// Projects along its X axis: straight down whatever the actor rotation; absolute scale so the mesh growth does not
	// scale the box. The box half size equals Radius, so the ring at full progress sits on the box edge (UV distance 0.5).
	ShockwaveDecal = NewObject<UDecalComponent>(this, TEXT("Shockwave"), RF_Transient);
	ShockwaveDecal->SetCanEverAffectNavigation(false);
	ShockwaveDecal->FadeScreenSize = 0.f;
	ShockwaveDecal->SortOrder = ShockwaveSortOrder;
	ShockwaveDecal->DecalSize = FVector(ShockwaveHalfDepth, Radius, Radius);
	ShockwaveDecal->SetUsingAbsoluteRotation(true);
	ShockwaveDecal->SetUsingAbsoluteScale(true);
	ShockwaveDecal->SetupAttachment(Mesh);
	ShockwaveMaterialInstance = UMaterialInstanceDynamic::Create(ShockwaveMaterial, this);
	ShockwaveDecal->SetDecalMaterial(ShockwaveMaterialInstance);
	ShockwaveDecal->RegisterComponent();
	ShockwaveDecal->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
	ShockwaveDecal->SetWorldScale3D(FVector::OneVector);
	ShockwaveMaterialInstance->SetScalarParameterValue(ShockwaveRadiusParameterName, Radius);
}

float ARRExplosionFX::GetVisualLifetime() const
{
	const float GrowSeconds = FMath::Max(Duration, KINDA_SMALL_NUMBER);
	return HoldSeconds > 0.f ? GrowSeconds + HoldSeconds + FMath::Max(FadeSeconds, KINDA_SMALL_NUMBER) : GrowSeconds;
}

float ARRExplosionFX::GetLifetime() const
{
	const float VisualLifetime = GetVisualLifetime();
	return BurstSystem ? FMath::Max(VisualLifetime, BurstSeconds) : VisualLifetime;
}

void ARRExplosionFX::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Age += DeltaSeconds;
	if (Age >= GetLifetime())
	{
		ApplyAge();
		Destroy();
		return;
	}
	if (bVisualHidden)
	{
		return;
	}
	ApplyAge();
	if (Age >= GetVisualLifetime())
	{
		// Visual over, the burst still plays: hide the faded sphere and ring until the destruction.
		bVisualHidden = true;
		Mesh->SetVisibility(false);
		if (ShockwaveDecal)
		{
			ShockwaveDecal->SetVisibility(false);
		}
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
	if (ShockwaveMaterialInstance)
	{
		ShockwaveMaterialInstance->SetScalarParameterValue(ShockwaveProgressParameterName, GrowAlpha);
		ShockwaveMaterialInstance->SetScalarParameterValue(ShockwaveOpacityParameterName, Opacity);
	}
}
