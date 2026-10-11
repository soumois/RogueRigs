// Rogue Rigs - XP and level of the miner: accumulates XP scaled by XPGain, levels up along the CSV XP curve with carry-over.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RRProgressionComponent.generated.h"

/** Fired after every XP change: XP in the current level, XP needed to leave it, current level. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnXPChangedSignature, float, Current, float, Needed, int32, Level);

/** Fired once per level gained, with the level just reached. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnLevelUpSignature, int32, NewLevel);

/** Fired by each AddXP that added XP: the amount after the XPGain multiplier (XP pop on the HUD). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnXPGainedSignature, float, Amount);

/**
 * XP and level progression of the miner (a default subobject of ARRMiner). The XP curve is read from XPCurve.csv
 * (FRRXPCurveRow, row name = level) when the component initializes; levels above the last row reuse its value.
 * AddXP multiplies the raw amount by the owner's XPGain attribute (URRPlayerAttributeSet, 1 when the owner has none),
 * then levels up as long as the XP reaches the current need, carrying the excess over (several levels at once are
 * possible): OnLevelUp fires once per level gained, then OnXPChanged once. XP pickups call AddXP on collection
 * (ARRPickup). The run starts at level 1 with 0 XP. No upgrade choice yet (xp-level-up, M1).
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Path of the XP curve CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* XPCurvePath;

	/** No tick; initializes the component so the curve is loaded before any BeginPlay. */
	URRProgressionComponent();

	/**
	 * Adds Raw x XPGain XP (Raw <= 0 is ignored), levels up with carry-over while the XP reaches the need of the current
	 * level (OnLevelUp per level gained), then broadcasts OnXPChanged.
	 */
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddXP(float Raw);

	/** Current level (starts at 1). */
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetLevel() const { return Level; }

	/** XP accumulated in the current level (below GetXPToNextLevel). */
	UFUNCTION(BlueprintPure, Category = "Progression")
	float GetXP() const { return XP; }

	/** XP needed to leave the current level (curve value; 0 only when the curve failed to load). */
	UFUNCTION(BlueprintPure, Category = "Progression")
	float GetXPToNextLevel() const { return GetXPToNextLevelAt(Level); }

	/** XP needed to leave InLevel: the curve row of that level, the last row above the curve, 0 without a curve. */
	float GetXPToNextLevelAt(int32 InLevel) const;

	/** Broadcast after every AddXP that changed the XP: (Current, Needed, Level). Bind for the XP bar. */
	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FRROnXPChangedSignature OnXPChanged;

	/** Broadcast once per level gained, before OnXPChanged, with the new level. Bind for the level-up feedback. */
	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FRROnLevelUpSignature OnLevelUp;

	/** Broadcast once per AddXP that added XP (Amount > 0, XPGain applied), after OnLevelUp and OnXPChanged. */
	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FRROnXPGainedSignature OnXPGained;

protected:
	/** Loads the XP curve (LoadCurve). */
	virtual void InitializeComponent() override;

	/** Reads XPCurve.csv into XPToNextLevelByLevel (index 0 = level 1). Logs and leaves it empty on failure. */
	void LoadCurve();

	/** Owner's XPGain attribute (current value); 1 when the owner has no ASC or no player attribute set. */
	float GetXPGain() const;

	/** XP needed per level, index 0 = level 1, copied from the CSV (rows sorted by level). */
	TArray<float> XPToNextLevelByLevel;

	/** Current level (starts at 1). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Progression")
	int32 Level = 1;

	/** XP accumulated in the current level. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Progression")
	float XP = 0.f;
};
