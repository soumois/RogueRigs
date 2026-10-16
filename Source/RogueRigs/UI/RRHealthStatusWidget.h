// Rogue Rigs - health status HUD panel (future parent of WBP_HealthStatus): drives the health bar and text from the miner health events, with a smooth drain of the shown fill, a damage chunk behind it, a hit flash of the fill, a hit shake and a hit punch of the bar root.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Slate/WidgetTransform.h"
#include "UI/RRHUDPanelWidget.h"
#include "RRHealthStatusWidget.generated.h"

class ARRMiner;
class UProgressBar;
class UTextBlock;
class UWidget;

/**
 * Health status panel. Binds ARRMiner::OnHealthChanged and OnDamaged natively (NativeBindToMiner, so a Blueprint override of
 * BindToMiner keeps running for the vignette and GAME OVER graphs). HealthText shows the real value at once; HealthBar shows a
 * fraction that glides to the real one (drain on damage, fill on heal). Bar effects run in one per-frame update (UpdateBarEffects),
 * armed by a health event through a next-tick timer and stopped once every effect has settled: no permanent tick.
 * Damage chunk: DamageChunkBar, drawn behind HealthBar, keeps the lost health visible for ChunkDelay after each hit, then shrinks
 * to the shown fraction; it is never below the shown fraction.
 * Hit flash: on each hit the fill colour jumps to FlashColor and fades back to GetBaseFillColor (Flash decays at FlashDecay per second).
 * Hit shake: on each hit HealthBarRoot is translated by a random X and Y offset within ShakeAmplitude, re-rolled each frame, for
 * ShakeDuration; the amplitude decays linearly to 0. Its render transform is composed in one place (GetBarRootTransform) so later
 * effects combine with the shake; it is identity once settled.
 * Hit punch: on each hit the render scale Y of HealthBarRoot jumps to PunchScale and a damped spring (PunchStiffness, PunchDamping)
 * brings it back to 1, substepped so a large frame time stays stable, snapped to exactly 1 once near rest. Scale X stays 1. The pivot
 * of HealthBarRoot is set from C++ (BarRootPivot, centre) when the widget is initialised.
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

	/** Fraction (0..1) the damage chunk currently shows; never below GetShownFraction. */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	float GetChunkFraction() const { return ChunkFraction; }

	/** Hit flash strength (0..1): 0 = base fill colour, 1 = FlashColor. */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	float GetFlashAlpha() const { return FlashAlpha; }

	/** Fill colour of HealthBar now: lerp(GetBaseFillColor, FlashColor, flash strength). */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	FLinearColor GetCurrentFillColor() const;

	/** Current shake translation of HealthBarRoot, in slate units (zero when no shake runs). */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	FVector2D GetShakeOffset() const { return ShakeOffset; }

	/** Current punch render scale Y of HealthBarRoot (1 when no punch runs). */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	float GetPunchScaleY() const { return PunchScaleY; }

	/** Render transform written on HealthBarRoot: translation = shake offset, scale = (1, punch scale Y); identity once every effect settled. */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	FWidgetTransform GetBarRootTransform() const;

	/** True while the per-frame bar update is armed (tests: no per-frame work once settled). */
	bool IsBarUpdateRunning() const { return bBarUpdateRunning; }

protected:
	/** Bar fill (optional: the class works on a Blueprint without it). Percent = shown fraction, fill colour = GetCurrentFillColor (base colour, flashed on hit). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Health", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	/** Damage chunk bar (optional), drawn behind HealthBar. Percent = chunk fraction, fill colour = ChunkColor. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Health", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> DamageChunkBar;

	/** Root of the bar (optional; holds DamageChunkBar and HealthBar; added to WBP_HealthStatus by task-203-07). Its render transform is owned by this class (GetBarRootTransform). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Health", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> HealthBarRoot;

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

	/** Seconds the damage chunk holds its value after a hit before it shrinks; each new hit restarts it. Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float ChunkDelay = 0.4f;

	/** Shrink speed of the damage chunk after its delay, per second: Chunk += (Shown - Chunk) * min(1, ChunkRate * dt). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float ChunkRate = 7.f;

	/** Damage chunk fill colour, #fff0e0 off-white (sRGB, converted to linear). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health")
	FLinearColor ChunkColor = FLinearColor(FColor(0xFF, 0xF0, 0xE0, 0xFF));

	/** Colour the fill flashes towards on a hit (white). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health")
	FLinearColor FlashColor = FLinearColor::White;

	/** Flash strength set by each hit (1 = full FlashColor); lerp alpha between base fill colour and FlashColor. Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0", ClampMax = "1"))
	float FlashStrength = 1.f;

	/** Fade speed of the hit flash, per second: Flash -= FlashDecay * dt, floor 0 (about 0.11 s from 1). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float FlashDecay = 9.f;

	/** Maximum shake offset on each axis right after a hit, in slate units (px at DPI scale 1); decays linearly to 0 over ShakeDuration. Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float ShakeAmplitude = 4.f;

	/** Seconds the bar shakes after a hit; each new hit restarts it (0 = no shake). Provisional, owner 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float ShakeDuration = 0.15f;

	/** Render scale Y of HealthBarRoot set by each hit; the spring brings it back to 1. Provisional, owner 2026-10-16 (reference video). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float PunchScale = 1.3f;

	/** Spring stiffness of the punch, per second squared: acceleration = -PunchStiffness * (ScaleY - 1) - PunchDamping * velocity. Provisional, owner "ok" 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float PunchStiffness = 400.f;

	/** Spring damping of the punch, per second (25 with stiffness 400: a small overshoot, settled in about 0.25 s). Provisional, owner "ok" 2026-10-16. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float PunchDamping = 25.f;

	/** Longest integration substep of the punch spring, in seconds: a frame is split into substeps no longer than this (stable with a large dt). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0.0001"))
	float PunchMaxSubstep = 1.f / 120.f;

	/** Most punch spring substeps in one frame; a longer frame integrates only PunchMaxSubsteps * PunchMaxSubstep seconds (bounded cost). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "1"))
	int32 PunchMaxSubsteps = 60;

	/** Distance of the punch scale Y to 1 under which (velocity also under PunchVelocitySnapThreshold) it snaps to exactly 1 and settles. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float PunchSnapThreshold = 0.001f;

	/** Punch scale Y speed, per second, under which (distance also under PunchSnapThreshold) the spring snaps to rest. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health", meta = (ClampMin = "0"))
	float PunchVelocitySnapThreshold = 0.01f;

	/** Render transform pivot set on HealthBarRoot when the widget is initialised (centre: the punch grows up and down). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Health")
	FVector2D BarRootPivot = FVector2D(0.5f, 0.5f);

	/** Shown fraction of the bar. */
	float ShownFraction = 0.f;

	/** Real health fraction. */
	float TargetFraction = 0.f;

	/** Fraction shown by the damage chunk (>= ShownFraction). */
	float ChunkFraction = 0.f;

	/** Seconds left before the damage chunk starts to shrink (0 = shrinking or settled). */
	float ChunkDelayRemaining = 0.f;

	/** Current hit flash strength (0 = no flash). */
	float FlashAlpha = 0.f;

	/** Seconds of shake left (0 = no shake). */
	float ShakeTimeRemaining = 0.f;

	/** Current shake translation of HealthBarRoot, re-rolled each frame while the shake runs. */
	FVector2D ShakeOffset = FVector2D::ZeroVector;

	/** Current punch render scale Y of HealthBarRoot (1 = rest). */
	float PunchScaleY = 1.f;

	/** Current punch scale Y velocity, per second (0 = rest). */
	float PunchVelocity = 0.f;

	/** True while the next-tick timer of UpdateBarEffects is armed. */
	bool bBarUpdateRunning = false;

	/** Handle of the next-tick timer that runs UpdateBarEffects. */
	FTimerHandle BarUpdateTimer;

	/** Sets the render transform pivot of HealthBarRoot (BarRootPivot) once the bound widgets exist. */
	virtual void NativeOnInitialized() override;

	/** Applies the fill colour and the current values once the widget tree exists; rebinds the miner if Destruct unbound it. */
	virtual void NativeConstruct() override;

	/** Stops the per-frame update. */
	virtual void NativeDestruct() override;

	/** Unbinds PreviousMiner's health events, binds Miner's, shows its current values at once (no glide at bind; null = 0 / 0). */
	virtual void NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner) override;

	/** OnHealthChanged handler: text at once, new target fraction, starts the drain. */
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	/** OnDamaged handler: restarts the chunk delay, the hit flash, the shake and the punch; start point of the hit effects; arms the update. */
	UFUNCTION()
	void HandleDamaged(float Amount, float HealthFraction);

	/** Writes the text and the target fraction; snaps the shown and chunk fractions when bSnap. */
	void ShowHealth(float Current, float Max, bool bSnap);

	/** Puts every bar effect at its end state (shown = target, chunk = shown, no flash, shake or punch); does not draw. */
	void SnapBarEffects();

	/** Arms the per-frame update if it is not running. */
	void StartBarUpdate();

	/** Disarms the per-frame update. */
	void StopBarUpdate();

	/** One frame of the bar effects: advance, draw, re-arm for the next frame unless everything settled. */
	void UpdateBarEffects();

	/** Advances every bar effect by DeltaSeconds (drain, damage chunk, hit flash, shake, punch; later effects add their step here). */
	virtual void AdvanceBarEffects(float DeltaSeconds);

	/** True when no bar effect needs another frame. */
	virtual bool AreBarEffectsSettled() const;

	/** Draws the current effect state on the widgets (bar and chunk percent, bar fill colour with the flash, chunk colour, HealthBarRoot render transform). */
	virtual void ApplyBarVisuals();

	/** Rolls a new shake offset within the amplitude left (ShakeAmplitude * remaining / duration); zero once the shake ended. */
	void RollShakeOffset();

	/** Integrates the punch spring by DeltaSeconds in substeps of at most PunchMaxSubstep, then snaps it to rest near 1. */
	void AdvancePunch(float DeltaSeconds);

	/** Base fill colour of the bar (FillColor; the colour-by-health subtask replaces it). */
	virtual FLinearColor GetBaseFillColor() const { return FillColor; }
};
