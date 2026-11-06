// Rogue Rigs - actor shadow (task-264-02, light-independent purple-grey shadow since task-472-01): a flat soft disc (engine Plane +
// MI_BlobShadow) on the floor under the miner, the drone, a robot, a turret, a barrel or the bonus chest.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DeveloperSettings.h"
#include "RRBlobShadowComponent.generated.h"

/**
 * Global multipliers of every actor shadow (task-472-01, owner 2026-11-06 Q3, provisional): Project Settings > Game > Actor
 * Shadows, saved in Config/DefaultGame.ini [/Script/RogueRigs.RRBlobShadowSettings]. Read when a shadow is sized or its opacity
 * applied (spawn, pool reuse, grid build), so a change shows on the next actors or the next play.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Actor Shadows"))
class ROGUERIGS_API URRBlobShadowSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Multiplies the opacity of every actor shadow (1 = each actor type's own Opacity). The product is clamped to [0, 1]. */
	UPROPERTY(Config, EditAnywhere, Category = "Actor Shadows", meta = (ClampMin = "0", UIMax = "2"))
	float GlobalOpacityMultiplier = 1.f;

	/** Multiplies the size of every actor shadow (1 = each actor type's own SizeScale x its footprint). */
	UPROPERTY(Config, EditAnywhere, Category = "Actor Shadows", meta = (ClampMin = "0", UIMax = "3"))
	float GlobalScaleMultiplier = 1.f;
};

/**
 * Fake shadow under an actor (task-472, owner 2026-11-05): a flat disc that ignores every light (unlit translucent material),
 * dark desaturated purple-grey with a soft outer edge (colour and edge in MI_BlobShadow, bp-blob-shadow-material). A static mesh
 * component showing the engine Plane (100 x 100 uu) with MI_BlobShadow, both loaded in the constructor (a missing asset leaves
 * the slot empty: nothing drawn). Fixed setup: no collision, no overlap events, no navigation, no cast shadow, no decals, no
 * distance field or indirect lighting, not an occluder, not in ray tracing or captures, a low TranslucencySortPriority (drawn
 * under the effects), and by default an absolute world rotation of zero (the disc never tilts or turns with its owner).
 * Size: footprint (set by the owner, uu, X and Y) x SizeScale (per actor type) x GlobalScaleMultiplier (URRBlobShadowSettings).
 * Opacity: Opacity (per actor type) x GlobalOpacityMultiplier, clamped to [0, 1], written in custom primitive data float
 * OpacityDataIndex, which the material multiplies into its opacity (one shared material instance, no dynamic instance).
 * Placements, none with a tick of its own:
 * - attached under a capsule (robots, miner): PlaceUnderCapsule once; the disc then follows the owner like any attached part.
 * - floor (hovering drone): the owner switches it to absolute location and calls PlaceOnFloor from its own Tick.
 * - fitted (turret, bonus chest): FitToOwnerMeshes once at BeginPlay, from the owner's visible meshes; follows the owner's yaw.
 * - grid (barrels): ARRArenaGrid creates one per barrel cell and places it.
 * Hidden with its owner (pool parking, actor hidden in game); robots also hide it when their death starts.
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

	/** Custom primitive data float read by the material as the shadow opacity (task-472-02). */
	static constexpr int32 OpacityDataIndex = 0;

	/** Sets a round footprint of Diameter uu (SetFootprintSize(Diameter, Diameter)). */
	void SetFootprintDiameter(float Diameter);

	/**
	 * Sets the footprint in uu (X and Y of the disc before SizeScale and the global multiplier). A footprint with a side <= 0
	 * hides the disc (the owner shows it again).
	 */
	void SetFootprintSize(const FVector2D& Size);

	/** Footprint diameter in uu (X of the footprint; 0 before any SetFootprint call). */
	float GetFootprintDiameter() const { return FootprintSize.X; }

	/** Footprint in uu (X, Y). */
	FVector2D GetFootprintSize() const { return FootprintSize; }

	/** Per actor type opacity (before the global multiplier); re-applies the custom primitive data. */
	void SetShadowOpacity(float InOpacity);

	/** Per actor type size multiplier of the footprint (before the global multiplier); re-applies the disc scale. */
	void SetShadowScale(float InSizeScale);

	float GetShadowOpacity() const { return Opacity; }
	float GetShadowScale() const { return SizeScale; }

	/** Opacity sent to the material: Opacity x GlobalOpacityMultiplier, clamped to [0, 1]. */
	float GetEffectiveOpacity() const;

	/** Drawn disc size in uu: footprint x SizeScale x GlobalScaleMultiplier x the floor size scale of PlaceOnFloor. */
	FVector2D GetDiscSize() const;

	/** Re-reads URRBlobShadowSettings and re-applies the disc scale and the opacity (after a settings change). */
	void RefreshFromSettings();

	/**
	 * Attached placement: relative location (0, 0, FloorOffset - CapsuleHalfHeight), i.e. FloorOffset above the bottom of a
	 * capsule of CapsuleHalfHeight (unscaled) centred on the parent. Call once (PostInitializeComponents); no update after.
	 */
	void PlaceUnderCapsule(float CapsuleHalfHeight);

	/**
	 * Floor placement for a hovering owner (the component uses an absolute location): world location (OwnerLocation.X,
	 * OwnerLocation.Y, FloorZ + FloorOffset). The disc size never depends on the height (owner 2026-11-06 Q3). SizeScale is an
	 * extra multiplier (1 by default). Skips the update when nothing changed.
	 */
	void PlaceOnFloor(const FVector& OwnerLocation, double FloorZ, float FloorSizeMultiplier = 1.f);

	/**
	 * Fitted placement for a static owner (turret, bonus chest): the footprint is the X / Y size of the union of the owner's
	 * visible static and skeletal mesh bounds (actor space, this component excluded), the disc is centred under that box at
	 * its bottom + FloorOffset, and turns with the owner's rotation (relative rotation). Returns false (nothing changed) when
	 * the owner has no such mesh.
	 */
	bool FitToOwnerMeshes();

	/** Height (uu) of the disc above the floor (z-fighting margin; the floor is the bottom of the owner's capsule). */
	float GetFloorOffset() const { return FloorOffset; }

protected:
	virtual void OnRegister() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/**
	 * Opacity of this actor type's shadow before the global multiplier (0.6, owner 2026-11-06 Q3, provisional). Set per actor
	 * type on the BlobShadow component of its Blueprint (barrels: BarrelShadowOpacity on the arena grid).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blob Shadow", meta = (ClampMin = "0", ClampMax = "1"))
	float Opacity = 0.6f;

	/**
	 * Disc size as a multiple of the actor's footprint before the global multiplier (1.2, owner 2026-11-06 Q3, provisional). Set
	 * per actor type on the BlobShadow component of its Blueprint (barrels: BarrelShadowScale on the arena grid).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blob Shadow", meta = (ClampMin = "0", UIMax = "3"))
	float SizeScale = 1.2f;

	/** Height (uu) of the disc above the floor: a few uu so it never z-fights the floor tiles (3, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blob Shadow", meta = (ClampMin = "0", Units = "cm"))
	float FloorOffset = 3.f;

	/** Footprint in uu (SetFootprintSize). */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Blob Shadow")
	FVector2D FootprintSize = FVector2D::ZeroVector;

	/** Extra size multiplier of the last PlaceOnFloor. */
	float FloorSizeScale = 1.f;

	/** Applies the disc scale (GetDiscSize / PlaneSize); does nothing while the disc size is 0. */
	void ApplyDiscScale();

	/** Writes GetEffectiveOpacity into custom primitive data float OpacityDataIndex. */
	void ApplyOpacity();
};
