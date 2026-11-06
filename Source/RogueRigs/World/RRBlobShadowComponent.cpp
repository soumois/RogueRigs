// Rogue Rigs - actor shadow (task-264-02, light-independent purple-grey shadow since task-472-01): a flat soft disc (engine Plane +
// MI_BlobShadow) on the floor under the miner, the drone, a robot, a turret, a barrel or the bonus chest.

#include "World/RRBlobShadowComponent.h"

#include "Components/MeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRBlobShadowComponent)

URRBlobShadowComponent::URRBlobShadowComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	// Shared assets: one mesh and one material instance for every disc (no dynamic instance per actor).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneFinder.Succeeded())
	{
		SetStaticMesh(PlaneFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/RogueRigs/FX/BlobShadow/MI_BlobShadow.MI_BlobShadow"));
	if (MaterialFinder.Succeeded())
	{
		SetMaterial(0, MaterialFinder.Object);
	}

	// Pure visual: no collision, no overlap, no navigation.
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);
	SetCanEverAffectNavigation(false);
	CanCharacterStepUpOn = ECB_No;

	// No lighting role: no shadow, no decals, not in Lumen / distance fields / ray tracing / captures, never an occluder.
	CastShadow = false;
	bReceivesDecals = false;
	bAffectDistanceFieldLighting = false;
	bAffectDynamicIndirectLighting = false;
	bUseAsOccluder = false;
	bVisibleInRayTracing = false;
	bVisibleInReflectionCaptures = false;
	bVisibleInRealTimeSkyCaptures = false;
	bSelectable = false;

	// Drawn before the other translucent effects (sparks, telegraph meshes, damage numbers).
	TranslucencySortPriority = -10;

	// Always flat: the world rotation stays zero whatever the owner's yaw or tilt (FitToOwnerMeshes switches to relative).
	SetUsingAbsoluteRotation(true);
	SetRelativeRotation(FRotator::ZeroRotator);

	// Default opacity in the custom data (a material read before OnRegister sees the per-type value, not 0).
	SetDefaultCustomPrimitiveDataFloat(OpacityDataIndex, Opacity);
}

void URRBlobShadowComponent::OnRegister()
{
	Super::OnRegister();
	ApplyOpacity();
}

#if WITH_EDITOR
void URRBlobShadowComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName Name = PropertyChangedEvent.GetMemberPropertyName();
	if (Name == GET_MEMBER_NAME_CHECKED(URRBlobShadowComponent, Opacity) || Name == GET_MEMBER_NAME_CHECKED(URRBlobShadowComponent, SizeScale))
	{
		SetDefaultCustomPrimitiveDataFloat(OpacityDataIndex, GetEffectiveOpacity());
		RefreshFromSettings();
	}
}
#endif

void URRBlobShadowComponent::SetFootprintDiameter(float Diameter)
{
	SetFootprintSize(FVector2D(Diameter, Diameter));
}

void URRBlobShadowComponent::SetFootprintSize(const FVector2D& Size)
{
	FootprintSize = FVector2D(FMath::Max(Size.X, 0.f), FMath::Max(Size.Y, 0.f));
	if (FootprintSize.X <= UE_KINDA_SMALL_NUMBER || FootprintSize.Y <= UE_KINDA_SMALL_NUMBER)
	{
		// A zero footprint means no disc; showing it again is the owner's call (robots restore their parts on pool reuse).
		if (IsVisible())
		{
			SetVisibility(false);
		}
		return;
	}
	ApplyDiscScale();
}

void URRBlobShadowComponent::SetShadowOpacity(float InOpacity)
{
	Opacity = FMath::Clamp(InOpacity, 0.f, 1.f);
	ApplyOpacity();
}

void URRBlobShadowComponent::SetShadowScale(float InSizeScale)
{
	SizeScale = FMath::Max(InSizeScale, 0.f);
	ApplyDiscScale();
}

float URRBlobShadowComponent::GetEffectiveOpacity() const
{
	const URRBlobShadowSettings* Settings = GetDefault<URRBlobShadowSettings>();
	return FMath::Clamp(Opacity * Settings->GlobalOpacityMultiplier, 0.f, 1.f);
}

FVector2D URRBlobShadowComponent::GetDiscSize() const
{
	const URRBlobShadowSettings* Settings = GetDefault<URRBlobShadowSettings>();
	return FootprintSize * (SizeScale * Settings->GlobalScaleMultiplier * FloorSizeScale);
}

void URRBlobShadowComponent::RefreshFromSettings()
{
	ApplyDiscScale();
	ApplyOpacity();
}

void URRBlobShadowComponent::PlaceUnderCapsule(float CapsuleHalfHeight)
{
	SetRelativeLocation(FVector(0.f, 0.f, FloorOffset - CapsuleHalfHeight));
}

void URRBlobShadowComponent::PlaceOnFloor(const FVector& OwnerLocation, double FloorZ, float FloorSizeMultiplier)
{
	const FVector Target(OwnerLocation.X, OwnerLocation.Y, FloorZ + FloorOffset);
	if (!GetComponentLocation().Equals(Target, 0.01))
	{
		SetWorldLocation(Target);
	}
	if (!FMath::IsNearlyEqual(FloorSizeMultiplier, FloorSizeScale, 1e-3f))
	{
		FloorSizeScale = FloorSizeMultiplier;
		ApplyDiscScale();
	}
}

bool URRBlobShadowComponent::FitToOwnerMeshes()
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}
	const FTransform ActorTransform = Owner->GetActorTransform();
	FBox Box(ForceInit);
	TInlineComponentArray<UMeshComponent*> Meshes(Owner);
	for (const UMeshComponent* MeshComponent : Meshes)
	{
		if (MeshComponent == this || MeshComponent->IsA<URRBlobShadowComponent>() || !MeshComponent->IsVisible() || MeshComponent->bHiddenInGame)
		{
			continue;
		}
		const FTransform Local = MeshComponent->GetComponentTransform().GetRelativeTransform(ActorTransform);
		const FBox MeshBox = MeshComponent->CalcBounds(Local).GetBox();
		if (MeshBox.IsValid && MeshBox.GetSize().SizeSquared2D() > UE_KINDA_SMALL_NUMBER)
		{
			Box += MeshBox;
		}
	}
	if (!Box.IsValid)
	{
		return false;
	}

	// Actor space, attached under the root: the disc turns with the owner (a static actor never tilts).
	SetUsingAbsoluteRotation(false);
	SetRelativeRotation(FRotator::ZeroRotator);
	const FVector Center = Box.GetCenter();
	SetRelativeLocation(FVector(Center.X, Center.Y, Box.Min.Z + FloorOffset));
	const FVector Size = Box.GetSize();
	SetFootprintSize(FVector2D(Size.X, Size.Y));
	return true;
}

void URRBlobShadowComponent::ApplyDiscScale()
{
	// No footprint yet (template, before the owner sizes it): keep the scale; SetFootprintSize hides a zero footprint.
	const FVector2D Disc = GetDiscSize() / PlaneSize;
	if (Disc.X > UE_KINDA_SMALL_NUMBER && Disc.Y > UE_KINDA_SMALL_NUMBER)
	{
		SetRelativeScale3D(FVector(Disc.X, Disc.Y, 1.f));
	}
}

void URRBlobShadowComponent::ApplyOpacity()
{
	SetCustomPrimitiveDataFloat(OpacityDataIndex, GetEffectiveOpacity());
}
