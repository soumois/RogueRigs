// Rogue Rigs - XP status HUD panel (parent of WBP_XPStatus): drives the XP bar fill and a pale ghost fill from the miner progression events; on a gain the ghost shows the new XP at once and the fill catches up after a short delay.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "UI/RRHUDPanelWidget.h"
#include "RRXPStatusWidget.generated.h"

class ARRMiner;
class UProgressBar;
class URRProgressionComponent;

/**
 * XP status panel. Binds the progression OnXPChanged and OnLevelUp of the miner natively (NativeBindToMiner, so the Blueprint
 * BindToMiner graph keeps running for the level text). XPGhostBar (drawn behind XPBar) jumps to the new XP fraction on each gain;
 * XPBar holds for CatchUpDelay (a new gain restarts it), then glides to the ghost at CatchUpRate, the mirror of the health bar
 * damage chunk. Level-up: the ghost goes to the end, the fill catches up to it, then both empty and the remainder of the new level
 * shows the same way (ghost at once, fill after the delay); several levels in one gain make one pass to the end. Per-frame
 * update armed by an XP event through a next-tick timer, stopped once settled: no permanent tick.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRXPStatusWidget : public URRHUDPanelWidget
{
	GENERATED_BODY()

public:
	/** Fraction (0..1) the cyan fill shows now; catches up with the ghost. */
	UFUNCTION(BlueprintPure, Category = "HUD|XP")
	float GetFillFraction() const { return FillFraction; }

	/** Fraction (0..1) the ghost shows now: the new XP at once, 1 while a level-up pass runs. */
	UFUNCTION(BlueprintPure, Category = "HUD|XP")
	float GetGhostFraction() const { return GhostFraction; }

	/** Real XP fraction of the current level (XP / need, 0 when the need is 0 or no miner). */
	UFUNCTION(BlueprintPure, Category = "HUD|XP")
	float GetTargetFraction() const { return TargetFraction; }

	/** True while a level-up pass runs (ghost at the end, the bar empties once the fill reaches it). */
	bool IsLevelUpPassPending() const { return bLevelUpPending; }

	/** True while the per-frame update is armed (tests: no per-frame work once settled). */
	bool IsXPBarUpdateRunning() const { return bXPBarUpdateRunning; }

	/** Advances the ghost and fill by DeltaSeconds and draws them (the per-frame update; public for the tests). */
	void AdvanceXPBar(float DeltaSeconds);

	/** True when the fill reached the ghost, no delay is left and no level-up pass runs. */
	bool IsXPBarSettled() const;

protected:
	/** Cyan fill (optional: the class works on a Blueprint without it). Percent = fill fraction; colour set in the Blueprint. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|XP", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> XPBar;

	/** Ghost bar (optional), drawn behind XPBar, whose background must be transparent. Percent = ghost fraction, fill colour = GhostColor. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|XP", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> XPGhostBar;

	/** Seconds the fill waits after a gain before catching up with the ghost; each new gain restarts it. Same as the health chunk, provisional, owner 2026-11-05 (Q32). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|XP", meta = (ClampMin = "0"))
	float CatchUpDelay = 0.4f;

	/** Catch-up speed of the fill, per second: Fill += (Ghost - Fill) * min(1, CatchUpRate * dt). Same as the health chunk, provisional, owner 2026-11-05 (Q32). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|XP", meta = (ClampMin = "0"))
	float CatchUpRate = 7.f;

	/** Gap under which the fill snaps to the ghost. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|XP", meta = (ClampMin = "0"))
	float CatchUpSnapThreshold = 0.001f;

	/** Ghost fill colour, very pale cyan #c8f6ff (sRGB, converted to linear). Provisional, owner 2026-11-05 (Q32: "cyan très clair"). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|XP")
	FLinearColor GhostColor = FLinearColor(FColor(0xC8, 0xF6, 0xFF, 0xFF));

	/** Fraction shown by XPBar. */
	float FillFraction = 0.f;

	/** Fraction shown by XPGhostBar (>= FillFraction). */
	float GhostFraction = 0.f;

	/** Real XP fraction of the current level; the ghost takes it at once, or after the level-up pass. */
	float TargetFraction = 0.f;

	/** Seconds left before the fill starts to catch up (0 = catching up or settled). */
	float DelayRemaining = 0.f;

	/** True from an OnLevelUp until the fill reached the end and the bar emptied. */
	bool bLevelUpPending = false;

	/** True between an OnLevelUp and the OnXPChanged that follows it (the gain that levelled up). */
	bool bLevelUpReceived = false;

	/** Level of the last XP event shown; a level change without OnLevelUp (run carry-over restore) snaps. */
	int32 ShownLevel = 0;

	/** True while the next-tick timer of UpdateXPBar is armed. */
	bool bXPBarUpdateRunning = false;

	/** Handle of the next-tick timer that runs UpdateXPBar. */
	FTimerHandle XPBarUpdateTimer;

	/** Rebinds the progression if a Blueprint Destruct unbound it, draws the current values. */
	virtual void NativeConstruct() override;

	/** Stops the per-frame update. */
	virtual void NativeDestruct() override;

	/** Unbinds PreviousMiner's progression, binds Miner's and shows its values at once (no ghost at bind; null = empty bar). */
	virtual void NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner) override;

	/** OnXPChanged handler: a gain moves the ghost at once and restarts the delay; after an OnLevelUp it starts the level-up pass. */
	UFUNCTION()
	void HandleXPChanged(float Current, float Needed, int32 Level);

	/** OnLevelUp handler (broadcast before OnXPChanged): marks the next XP event as a level-up gain. */
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);

	/** Binds this widget's handlers to Progression (no duplicate). */
	void BindProgression(URRProgressionComponent* Progression);

	/** Shows the values at once: fill = ghost = target, no delay, no pass. */
	void SnapXPBar();

	/** Arms the per-frame update if it is not running. */
	void StartXPBarUpdate();

	/** Disarms the per-frame update. */
	void StopXPBarUpdate();

	/** One frame: advance by the world delta, re-arm unless settled. */
	void UpdateXPBar();

	/** Writes the fill and ghost percents and the ghost colour. */
	void ApplyXPBarVisuals();
};
