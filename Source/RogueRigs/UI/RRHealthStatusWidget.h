// Rogue Rigs - health status HUD panel (future parent of WBP_HealthStatus): drives the health bar and text from the miner health events, with a smooth drain of the shown fill.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "UI/RRHUDPanelWidget.h"
#include "RRHealthStatusWidget.generated.h"

class ARRMiner;
class UProgressBar;
class UTextBlock;

/**
 * Health status panel. Binds ARRMiner::OnHealthChanged and OnDamaged natively (NativeBindToMiner, so a Blueprint override of
 * BindToMiner keeps running for the vignette and GAME OVER graphs). HealthText shows the real value at once; HealthBar shows a
 * fraction that glides to the real one (drain on damage, fill on heal). Bar effects run in one per-frame update (UpdateBarEffects),
 * armed by a health event through a next-tick timer and stopped once every effect has settled: no permanent tick.
 * Extension point for later bar effects (damage chunk, flash, shake, punch, colour): add state here, start it in the event
 * handlers, advance it in AdvanceBarEffects, report it in AreBarEffectsSettled, draw it in ApplyBarVisuals.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRHealthStatusWidget : public URRHUDPanelWidget
{
	GENERATED_BODY()

public:
	/** Fraction (0..1) the bar currently shows; glides to GetTargetFraction. */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	float GetShownFraction() const { return ShownFraction; }

	/** Real health fraction (Current / Max, 0 when Max <= 0 or no miner). */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	float GetTargetFraction() const { return TargetFraction; }

	/** True while the per-frame bar update is armed (tests: no per-frame work once settled). */
	bool IsBarUpdateRunning() const { return bBarUpdateRunning; }

protected:
	/** Bar fill (optional: the class works on a Blueprint without it). Percent = shown fraction, fill colour = FillColor. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Health", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	/** Health text (optional): "round(Current) / round(Max)", updated at once on every change. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Health", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;

	/** Drain speed of the shown fraction, per second: Shown += (Target - Shown) * min(1, DrainRate * dt). Both ways (damage, heal). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float DrainRate = 10.f;

	/** Gap between shown and real fraction under which the shown one snaps to the real one and the drain settles. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float DrainSnapThreshold = 0.001f;

	/** Bar fill colour (the former WBP_HealthStatus red). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health")
	FLinearColor FillColor = FLinearColor(0.9f, 0.05f, 0.05f, 1.f);

	/** Shown fraction of the bar. */
	float ShownFraction = 0.f;

	/** Real health fraction. */
	float TargetFraction = 0.f;

	/** True while the next-tick timer of UpdateBarEffects is armed. */
	bool bBarUpdateRunning = false;

	/** Handle of the next-tick timer that runs UpdateBarEffects. */
	FTimerHandle BarUpdateTimer;

	/** Applies the fill colour and the current values once the widget tree exists; rebinds the miner if Destruct unbound it. */
	virtual void NativeConstruct() override;

	/** Stops the per-frame update. */
	virtual void NativeDestruct() override;

	/** Unbinds PreviousMiner's health events, binds Miner's, shows its current values at once (no glide at bind; null = 0 / 0). */
	virtual void NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner) override;

	/** OnHealthChanged handler: text at once, new target fraction, starts the drain. */
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	/** OnDamaged handler: start point of the hit effects of later subtasks (chunk, flash, shake, punch); arms the update. */
	UFUNCTION()
	void HandleDamaged(float Amount, float HealthFraction);

	/** Writes the text and the target fraction; snaps the shown fraction when bSnap. */
	void ShowHealth(float Current, float Max, bool bSnap);

	/** Arms the per-frame update if it is not running. */
	void StartBarUpdate();

	/** Disarms the per-frame update. */
	void StopBarUpdate();

	/** One frame of the bar effects: advance, draw, re-arm for the next frame unless everything settled. */
	void UpdateBarEffects();

	/** Advances every bar effect by DeltaSeconds (drain now; later effects add their step here). */
	virtual void AdvanceBarEffects(float DeltaSeconds);

	/** True when no bar effect needs another frame. */
	virtual bool AreBarEffectsSettled() const;

	/** Draws the current effect state on the widgets (percent, fill colour). */
	virtual void ApplyBarVisuals();

	/** Base fill colour of the bar (FillColor; the colour-by-health subtask replaces it). */
	virtual FLinearColor GetBaseFillColor() const { return FillColor; }
};
