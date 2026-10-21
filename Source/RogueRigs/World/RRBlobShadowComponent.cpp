// Rogue Rigs - blob shadow (task-264-02): a flat dark disc (engine Plane + MI_BlobShadow) on the floor under a robot, the miner or the drone.

#include "World/RRBlobShadowComponent.h"

#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
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

	// Always flat: the world rotation stays zero whatever the owner's yaw or tilt.
	SetUsingAbsoluteRotation(true);
	SetRelativeRotation(FRotator::ZeroRotator);
}

void URRBlobShadowComponent::SetFootprintDiameter(float Diameter)
{
	FootprintDiameter = FMath::Max(Diameter, 0.f);
	ApplyDiscScale(FloorSizeScale);
}

void URRBlobShadowComponent::PlaceUnderCapsule(float CapsuleHalfHeight)
{
	SetRelativeLocation(FVector(0.f, 0.f, FloorOffset - CapsuleHalfHeight));
}

void URRBlobShadowComponent::PlaceOnFloor(const FVector& OwnerLocation, double FloorZ, float SizeScale)
{
	const FVector Target(OwnerLocation.X, OwnerLocation.Y, FloorZ + FloorOffset);
	if (!GetComponentLocation().Equals(Target, 0.01))
	{
		SetWorldLocation(Target);
	}
	if (!FMath::IsNearlyEqual(SizeScale, FloorSizeScale, 1e-3f))
	{
		ApplyDiscScale(SizeScale);
	}
}

void URRBlobShadowComponent::ApplyDiscScale(float SizeScale)
{
	FloorSizeScale = SizeScale;
	const float Scale = FootprintDiameter * SizeScale / PlaneSize;
	if (Scale > UE_KINDA_SMALL_NUMBER)
	{
		SetRelativeScale3D(FVector(Scale, Scale, 1.f));
	}
	else if (IsVisible())
	{
		// A zero footprint means no disc; showing it again is the owner's call (robots restore their parts on pool reuse).
		SetVisibility(false);
	}
}
