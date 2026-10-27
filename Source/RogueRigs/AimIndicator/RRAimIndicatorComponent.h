// Rogue Rigs - aim indicator (gdd aim-indicator): a decal projected on the ground in front of the miner drawing a row of white-gold chevrons along the aim.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "RRAimIndicatorComponent.generated.h"

class ARRMiner;
class UMaterialInstanceDynamic;
class UMaterialInterface;

/**
 * Aim chevrons of the miner (gdd aim-indicator, task-328-03): a decal projected straight down (follows the floor and the top
 * of rocks) whose material M_AimChevrons draws ChevronCount chevrons along the aim, the first FirstOffset uu from the miner,
 * then every Spacing uu, opacity x Decay per chevron. Direction only: no link to any weapon, no effect on aim, fire, turret
 * placement or the drone. A decal is a scene component, not a primitive: no collision, overlap or navigation.
 * Each tick (TG_PostPhysics, after the miner turned and moved) it is placed at the miner's feet, its local +Z (the material's
 * forward, UV.x) along ARRMiner::GetAimDirection and its projection axis (local X) straight down; the box starts at the
 * miner (UV.x = 0) and runs 2 x HalfLength forward. VisibleLength = min(GetFixedLength(), 2D distance from the miner along
 * the aim to the first block cell, ARRArenaGrid::GetClearDistance); no grid: the fixed length. A chevron whose tip lies
 * beyond VisibleLength is not drawn (material rule), so the row stops at the first rock block.
 * Visible whenever the owning miner is alive, mouse and stick aim alike; hidden at death (ARRMiner::HandleDeath calls
 * HideIndicator, and the tick hides it while the miner IsDead). No brightness compensation for the player light mask.
 * Material: DecalMaterial when set on the component, else DefaultMaterialPath (config); a missing asset logs one warning per
 * run and nothing is drawn (tick off). The material is warmed up through ARRDecalWarmUp at BeginPlay.
 */
UCLASS(Config = Game, ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRAimIndicatorComponent : public UDecalComponent
{
	GENERATED_BODY()

public:
	/** Material parameter names (M_AimChevrons of task-328-02 declares exactly these). */
	static const FName ChevronCountParameterName;
	static const FName FirstOffsetParameterName;
	static const FName SpacingParameterName;
	static const FName OpeningDegParameterName;
	static const FName ArmLengthParameterName;
	static const FName LineWidthParameterName;
	static const FName DecayParameterName;
	static const FName ColorParameterName;
	static const FName OpacityParameterName;
	static const FName HalfLengthParameterName;
	static const FName HalfWidthParameterName;
	static const FName VisibleLengthParameterName;

	/** Hidden until the first refresh; ticks in TG_PostPhysics, absolute location and rotation, never fades with screen size, no navigation. */
	URRAimIndicatorComponent(const FObjectInitializer& ObjectInitializer);

	/** Sizes the box, resolves the material, creates the dynamic instance and writes every parameter, warms the material up, then places the decal once. */
	virtual void BeginPlay() override;

	/** Each frame: RefreshIndicator. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Hidden when the owner is not a living ARRMiner or there is no material; otherwise places the decal at the miner's feet
	 * along the aim, writes VisibleLength (fixed length capped at the first block cell on the aim line) and shows it.
	 */
	void RefreshIndicator();

	/** Hides the decal at once (miner death). The next RefreshIndicator shows it again only while the miner is alive. */
	void HideIndicator();

	/** True while drawn. */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	bool IsIndicatorShown() const { return IsVisible(); }

	/** Fixed length of the row in uu: tip distance of the last chevron, FirstOffset + (ChevronCount - 1) x Spacing. */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float GetFixedLength() const;

	/** VisibleLength written at the last refresh (uu from the miner; the fixed length before the first one). */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float GetVisibleLength() const { return VisibleLength; }

	/** The dynamic instance the parameters are written to (null without a material). */
	UMaterialInstanceDynamic* GetIndicatorMaterialInstance() const { return IndicatorMaterialInstance; }

protected:
	/** Loads DefaultMaterialPath once per run (cached; a missing asset logs one warning and is not retried). */
	UMaterialInterface* ResolveDefaultMaterial() const;

	/** Writes every static parameter (count, spacing, shape, colour, box half sizes) and VisibleLength to the dynamic instance, if any. */
	void ApplyParameters();

	/** The owner as a miner (null for any other owner: the indicator stays hidden). */
	ARRMiner* GetOwnerMiner() const;

	/** Default material when DecalMaterial is not set on the component (task-328-02). Config: [/Script/RogueRigs.RRAimIndicatorComponent]. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Aim Indicator")
	FSoftObjectPath DefaultMaterialPath = FSoftObjectPath(TEXT("/Game/RogueRigs/FX/M_AimChevrons.M_AimChevrons"));

	/** Number of chevrons (aim-indicator: 5, provisional). Material ChevronCount. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "1", ClampMax = "16"))
	int32 ChevronCount = 5;

	/** Distance in uu from the miner to the first chevron tip (aim-indicator: 120, provisional). Material FirstOffset. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float FirstOffset = 120.f;

	/** Distance in uu between two chevron tips (aim-indicator: 74, provisional). Material Spacing. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float Spacing = 74.f;

	/** Full opening angle of a chevron in degrees (aim-indicator: about 60, provisional). Material OpeningDeg. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", ClampMax = "180", Units = "deg"))
	float OpeningDeg = 60.f;

	/** Length in uu of each chevron arm (aim-indicator: 40, provisional). Material ArmLength. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float ArmLength = 40.f;

	/** Line width in uu (aim-indicator: 12, provisional). Material LineWidth. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float LineWidth = 12.f;

	/** Opacity factor from one chevron to the next (aim-indicator: x0.78, provisional). Material Decay. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", ClampMax = "1"))
	float Decay = 0.78f;

	/** Chevron colour, linear RGB (aim-indicator: white-gold sRGB (255, 236, 170), linear about (1.0, 0.84, 0.40)). Material Color. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator")
	FLinearColor Color = FLinearColor(1.f, 0.84f, 0.40f, 1.f);

	/** Opacity of the first chevron (aim-indicator: 1). Material Opacity. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", ClampMax = "1"))
	float Opacity = 1.f;

	/** Half length in uu of the decal box along the aim (material default of task-328-02: 500); must cover GetFixedLength plus half a line and the glow. Material HalfLength. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "1", Units = "cm"))
	float HalfLength = 500.f;

	/** Half width in uu of the decal box across the aim (material default of task-328-02: 64). Material HalfWidth. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "1", Units = "cm"))
	float HalfWidth = 64.f;

	/** Half height in uu of the projection box above and below the miner's feet (as the ground telegraph): reaches the floor and the top of rocks. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "1", Units = "cm"))
	float ProjectionHalfDepth = 256.f;

	/** VisibleLength last written to the material (uu). */
	float VisibleLength = 0.f;

	/** Dynamic instance of the chevron material (null without a material). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> IndicatorMaterialInstance;
};
