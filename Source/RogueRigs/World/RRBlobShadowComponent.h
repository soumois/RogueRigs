// Rogue Rigs - blob shadow (task-264-02): a flat dark disc (engine Plane + MI_BlobShadow) on the floor under a robot, the miner or the drone.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "RRBlobShadowComponent.generated.h"

/**
 * Fake round shadow under an actor (lights-no-shadows: only the grid rocks cast real shadows). A static mesh component showing
 * the engine Plane (100 x 100 uu) with MI_BlobShadow (unlit translucent radial disc, bp-blob-shadow-material), both loaded in
 * the constructor (a missing asset leaves the slot empty: nothing drawn). Fixed setup: no collision, no overlap events, no
 * navigation, no cast shadow, no decals, no distance field or indirect lighting, not an occluder, not in ray tracing or
 * captures, a low TranslucencySortPriority (drawn under the effects), and an absolute world rotation of zero, so the disc never
 * tilts or turns with its owner. Two placements, both without a tick of their own:
 * - attached (robots, miner): PlaceUnderCapsule once; the disc then follows the owner like any attached part.
 * - floor (hovering drone): the owner switches it to absolute location and calls PlaceOnFloor from its own Tick.
 * The owner sizes it with SetFootprintDiameter (scale X = Y = diameter / 100). Hidden with its owner (pool parking, actor
 * hidden in game); robots also hide it when their death starts.
 */
UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRBlobShadowComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	/** Loads the Plane mesh and MI_BlobShadow; applies the fixed render, collision and rotation setup. */
	URRBlobShadowComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Side of the engine Plane mesh in uu at scale 1 (the disc diameter at scale 1). */
	static constexpr float PlaneSize = 100.f;

	/** Sets the disc diameter in uu: scale X = Y = Diameter / PlaneSize (Z 1). Diameter <= 0 hides the disc (the owner shows it again). */
	void SetFootprintDiameter(float Diameter);

	/** Disc diameter in uu set by SetFootprintDiameter (0 before). */
	float GetFootprintDiameter() const { return FootprintDiameter; }

	/**
	 * Attached placement: relative location (0, 0, FloorOffset - CapsuleHalfHeight), i.e. FloorOffset above the bottom of a
	 * capsule of CapsuleHalfHeight (unscaled) centred on the parent. Call once (PostInitializeComponents); no update after.
	 */
	void PlaceUnderCapsule(float CapsuleHalfHeight);

	/**
	 * Floor placement for a hovering owner (the component uses an absolute location): world location (OwnerLocation.X,
	 * OwnerLocation.Y, FloorZ + FloorOffset), disc scaled by FootprintDiameter x SizeScale. Skips the update when nothing changed.
	 */
	void PlaceOnFloor(const FVector& OwnerLocation, double FloorZ, float SizeScale = 1.f);

	/** Height (uu) of the disc above the floor (z-fighting margin; the floor is the bottom of the owner's capsule). */
	float GetFloorOffset() const { return FloorOffset; }

protected:
	/** Height (uu) of the disc above the floor: a few uu so it never z-fights the floor tiles (3, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blob Shadow", meta = (ClampMin = "0", Units = "cm"))
	float FloorOffset = 3.f;

	/** Disc diameter in uu (SetFootprintDiameter). */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Blob Shadow")
	float FootprintDiameter = 0.f;

	/** Size multiplier of the last PlaceOnFloor (the disc scale is FootprintDiameter / PlaneSize x this). */
	float FloorSizeScale = 1.f;

	/** Applies the disc scale (FootprintDiameter / PlaneSize x SizeScale) and its visibility (hidden when the diameter is 0). */
	void ApplyDiscScale(float SizeScale);
};
